/**
 * @file Bootloader.cpp
 * @brief Implements the Bootloader component used by the bootloader.
 *
 * @author Simon Kallweit — original PER|FORMER implementation lineage
 * @author Axel Napolitano — Styr modifications, integration and modernization
 * @copyright 2017-2018 Simon Kallweit
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include "Config.h"
#include "Debug.h"
#include "System.h"
#include "Console.h"
#include "Format.h"
#include "FlashLayout.h"
#include "Encoder.h"
#include "Lcd.h"
#include "Canvas.h"
#include "SdCard.h"
#include "UpdateFile.h"
#include "VersionTag.h"
#include "MD5.h"

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/flash.h>

#include <cstring>

// version tag of firmware in flash
const VersionTag &currentVersion = *reinterpret_cast<VersionTag *>(CONFIG_APPLICATION_ADDR + CONFIG_VERSION_TAG_OFFSET);

extern "C" {

void sys_tick_handler(void) {
    System::tick();
    Encoder::process();
}

} // extern "C"

static void init() {
    System::init();
    Console::init();
    Encoder::init();
    Lcd::init();
    SdCard::init();

    System::startSysTick();
}

static void deinit() {
    System::stopSysTick();

    SdCard::deinit();
    Lcd::deinit();
    Encoder::deinit();
    Console::deinit();
    System::deinit();
}

static void formatVersion(const VersionTag &version, char *str, size_t len) {
    if (version.isValid()) {
        // A damaged application tag must never let %s read beyond name[24].
        char safeName[sizeof(version.name) + 1];
        memcpy(safeName, version.name, sizeof(version.name));
        safeName[sizeof(version.name)] = '\0';
        bootSnprintf(str, len, "%s (%d.%d.%d)", safeName, version.major, version.minor, version.revision);
    } else {
        bootSnprintf(str, len, "invalid image");
    }
}


static void jumpToApplication(void) {
    deinit();

	// disable all interrupts and priorities
	NVIC_ICER(0) = 0xFFFFFFFF;
	NVIC_ICER(1) = 0xFFFFFFFF;
	NVIC_ICER(2) = 0xFFFFFFFF;
	NVIC_ICPR(0) = 0xFFFFFFFF;
	NVIC_ICPR(1) = 0xFFFFFFFF;
	NVIC_ICPR(2) = 0xFFFFFFFF;

	/* store bootloader version in BKP_DR2,3 */
	// BKP_DR2 = VERSION & 0xFFFF;
	// BKP_DR3 = VERSION >> 16;

	// set vector table base address
	SCB_VTOR = CONFIG_APPLICATION_ADDR;
	// initialise master stack pointer
	asm volatile("msr msp, %0" :: "g" (*reinterpret_cast<volatile uint32_t *>(CONFIG_APPLICATION_ADDR)));
	// jump to application
    (*reinterpret_cast<void (**)()>(CONFIG_APPLICATION_ADDR + 4))();
}

static void startApplication() {
    char str[32];
    formatVersion(currentVersion, str, sizeof(str));
    bootPrintf("loading %s ...\n\n\n", str);

    jumpToApplication();
}


static void clearScreen() {
    Canvas::setColor(0);
    Canvas::fill();
    Canvas::show();
}

static void drawScreen(const char *current, const char *update, const char *message = "\0") {
    char title[32];
    bootSnprintf(title, sizeof(title), "bootloader %d.%d", CONFIG_VERSION_MAJOR, CONFIG_VERSION_MINOR);

    Canvas::setColor(0);
    Canvas::fill();

    Canvas::setColor(0xf);
    Canvas::drawText(10, 10, title);

    Canvas::drawText(10, 30, "current:");
    Canvas::drawText(70, 30, current);
    Canvas::drawText(10, 40, "update:");
    Canvas::drawText(70, 40, update);

    const int period = 500;
    Canvas::setColor(3 + std::labs((System::ticks() % period) - (period / 2)) * 12 / (period / 2));
    Canvas::drawText(10, 60, message);

    Canvas::show();
}



static void bootloader() {
    char currentStr[32];
    char updateStr[32];
    char errorStr[32];

    formatVersion(currentVersion, currentStr, sizeof(currentStr));
    bootSnprintf(updateStr, sizeof(updateStr), "checking ...");

    drawScreen(currentStr, updateStr);

    bootPrintf("current image: %s\n", currentStr);
    bootPrintf("checking for update image ...\n");

    VersionTag updateVersion;
    size_t updateSize;
    MD5::Sum updateMd5;

    bool success = UpdateFile::open(updateVersion, updateSize, updateMd5, errorStr, sizeof(errorStr));

    // log update image status
    if (success) {
        formatVersion(updateVersion, updateStr, sizeof(updateStr));
        bootPrintf("found update image: %s\n", updateStr);
        bootPrintf("size: %lu bytes\n", static_cast<unsigned long>(updateSize));
        bootPrintf("md5sum: ");
        for (size_t i = 0; i < sizeof(updateMd5); ++i) {
            bootPrintf("%02x", updateMd5[i]);
        }
        bootPrintf("\n");
    } else {
        bootPrintf("no update image found: %s\n", errorStr);
    }

    // verify update image md5sum
    if (success) {
        bootPrintf("verifying update image ...\n");

        MD5 md5;

        uint32_t buf[1024 / 4];
        size_t bytesLeft = updateSize;
        while (bytesLeft > 0) {
            int progress = ((updateSize - bytesLeft) * 100) / updateSize;
            bootSnprintf(updateStr, sizeof(updateStr), "verifying image %d%%", progress);
            drawScreen(currentStr, updateStr);
            size_t chunkSize = bytesLeft < sizeof(buf) ? bytesLeft : sizeof(buf);
            if (!UpdateFile::read(buf, chunkSize, errorStr, sizeof(errorStr))) {
                success = false;
                break;
            }

            md5.update(buf, chunkSize);

            bytesLeft -= chunkSize;
        }

        MD5::Sum computedMd5;
        md5.finish(computedMd5);

        bootPrintf("computed md5sum: ");
        for (size_t i = 0; i < sizeof(computedMd5); ++i) {
            bootPrintf("%02x", computedMd5[i]);
        }
        bootPrintf("\n");

        success = memcmp(updateMd5, computedMd5, sizeof(MD5::Sum)) == 0;
        if (success) {
            bootPrintf("valid image\n");
        } else {
            bootPrintf("invalid image (md5sum mismatch)\n");
            bootSnprintf(errorStr, sizeof(errorStr), "invalid checksum");
        }
    }

    // wait for user to confirm update
    bool writeUpdate = false;
    if (success) {
        formatVersion(updateVersion, updateStr, sizeof(updateStr));
        Encoder::reset();
        while (!Encoder::pressed()) {
            int value = Encoder::value();
            if (value < 0) {
                writeUpdate = false;
            } else if (value > 0) {
                writeUpdate = true;
            }
            drawScreen(currentStr, updateStr, writeUpdate ? "write update? yes" : "write update? no");
        }
    }

    // write update image to flash
    if (success && writeUpdate) {
        success = UpdateFile::rewind(errorStr, sizeof(errorStr));
    }

    if (success && writeUpdate) {
        bootPrintf("writing update image to 0x%08lx ...\n", static_cast<unsigned long>(CONFIG_APPLICATION_ADDR));

        flash_unlock();

        uint32_t addr = CONFIG_APPLICATION_ADDR;
        uint32_t buf[1024 / 4];
        size_t bytesLeft = updateSize;

        while (bytesLeft > 0) {
            int progress = ((updateSize - bytesLeft) * 100) / updateSize;
            bootSnprintf(updateStr, sizeof(updateStr), "writing image %d%%", progress);
            drawScreen(currentStr, updateStr);
            size_t chunkSize = bytesLeft < sizeof(buf) ? bytesLeft : sizeof(buf);
            if (!UpdateFile::read(buf, chunkSize, errorStr, sizeof(errorStr))) {
                success = false;
                break;
            }

            int sector = BootloaderFlash::sectorIndex(addr);
            if (sector >= 0) {
                bootPrintf("erasing sector %d at 0x%08lx ... ", sector, addr);
                flash_erase_sector(sector, 2);
                flash_wait_for_last_operation();
                bootPrintf("done\n");
            }

            for (size_t i = 0; i < (chunkSize + 3) / 4; ++i) {
                flash_program_word(addr, buf[i]);
                flash_wait_for_last_operation();
                addr += 4;
            }

            bytesLeft -= chunkSize;
        }

        flash_lock();

        if (success) {
            bootPrintf("write successful\n");
        } else {
            bootPrintf("failed to write update image: %s\n", errorStr);
        }
    }

    // verify written image
    if (success && writeUpdate) {
        bootPrintf("verifying written image ...\n");
        drawScreen(currentStr, "verifying");

        MD5 md5;
        md5.update(reinterpret_cast<void *>(CONFIG_APPLICATION_ADDR), updateSize);
        MD5::Sum computedMd5;
        md5.finish(computedMd5);

        bootPrintf("computed md5sum: ");
        for (size_t i = 0; i < sizeof(computedMd5); ++i) {
            bootPrintf("%02x", computedMd5[i]);
        }
        bootPrintf("\n");

        success = memcmp(updateMd5, computedMd5, sizeof(MD5::Sum)) == 0;
        if (success) {
            bootPrintf("verify successful\n");
            bootSnprintf(updateStr, sizeof(updateStr), "successful");
        } else {
            bootPrintf("verify failed (md5sum mismatch)\n");
            bootSnprintf(errorStr, sizeof(errorStr), "writing image failed");

            // invalidate version tag
            flash_unlock();
            for (uint32_t addr = 0; addr < sizeof(VersionTag); addr += 4) {
                flash_program_word(CONFIG_APPLICATION_ADDR + CONFIG_VERSION_TAG_OFFSET + addr, 0);
                flash_wait_for_last_operation();
            }
            flash_lock();
        }
    }

    // final screen
    formatVersion(currentVersion, currentStr, sizeof(currentStr));
    Encoder::reset();
    while (1) {
        drawScreen(currentStr, success ? updateStr : errorStr, "press encoder to reset!");
        if (Encoder::pressed()) {
            System::reset();
        }
    }
}

int main(void) {
    init();

    clearScreen();

    bootPrintf("\nbootloader %d.%d\n", CONFIG_VERSION_MAJOR, CONFIG_VERSION_MINOR);

    // enter bootloader mode if encoder is pressed or no valid image is found
    if (Encoder::down() || !currentVersion.isValid()) {
        bootloader();
    } else {
        startApplication();
    }

    while (1) {}
}
