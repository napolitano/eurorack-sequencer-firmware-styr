/**
 * @file TestScale.cpp
 * @brief Implements the TestScale component used by the unit test.
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
#include <unity.h>

#include "core/Debug.h"
#include "core/utils/StringBuilder.h"

#include "model/Scale.h"
#include "model/UserScale.h"

#include <array>
#include <cstdint>

void test_01_notename_notetovolts() {
        for (int i = 0; i < Scale::Count; ++i) {
            const auto &scale = Scale::get(i);
            int notesPerOctave = scale.notesPerOctave();

            DBG("----------------------------------------");
            DBG("%s", Scale::name(i));
            DBG("octave has %d notes", notesPerOctave);
            DBG("----------------------------------------");
            DBG("%-8s %-8s %-8s %-8s %-8s", "note", "volts", "short1", "short2", "long");

            for (int note = -notesPerOctave; note <= notesPerOctave; ++note) {
                FixedStringBuilder<8> short1Name;
                FixedStringBuilder<8> short2Name;
                FixedStringBuilder<16> longName;
                scale.noteName(short1Name, note, 0, Scale::Short1);
                scale.noteName(short2Name, note, 0, Scale::Short2);
                scale.noteName(longName, note, 0, Scale::Long);
                float volts = scale.noteToVolts(note);
                DBG("%-8d %-8.3f %-8s %-8s %-8s", note, volts, (const char *)(short1Name), (const char *)(short2Name), (const char *)(longName));
                // DBG("note = %d, volts = %f", note, volts);
            }
        }
    
}

void test_02_notefromvolts() {
        for (int i = 0; i < Scale::Count; ++i) {
            const auto &scale = Scale::get(i);
            int notesPerOctave = scale.notesPerOctave();

            DBG("----------------------------------------");
            DBG("%s", Scale::name(i));
            DBG("octave has %d notes", notesPerOctave);
            DBG("----------------------------------------");
            DBG("%-8s %-8s %-8s %-8s %-8s %-8s", "voltsin", "note", "volts", "short1", "short2", "long");

            for (int index = -12; index <= 12; ++index) {
                float voltsin = index * (1.f / 12.f);
                int note = scale.noteFromVolts(voltsin);
                FixedStringBuilder<8> short1Name;
                FixedStringBuilder<8> short2Name;
                FixedStringBuilder<16> longName;
                scale.noteName(short1Name, note, 0, Scale::Short1);
                scale.noteName(short2Name, note, 0, Scale::Short2);
                scale.noteName(longName, note, 0, Scale::Long);
                float volts = scale.noteToVolts(note);
                DBG("%-8.3f %-8d %-8.3f %-8s %-8s %-8s", voltsin, note, volts, (const char *)(short1Name), (const char *)(short2Name), (const char *)(longName));
            }

        }
    
}

#ifdef PLATFORM_SIM

void test_03_markdown() {
        const int ColsPerRow = 8;

        DBG("----------------------------------------");
        for (int i = 0; i < Scale::Count - 4; ++i) {
            const auto &scale = Scale::get(i);
            int notesPerOctave = scale.notesPerOctave();

            DBG("<h4>%s</h4>", Scale::name(i));
            DBG("");

            for (int page = 0; page < (notesPerOctave + ColsPerRow - 1) / ColsPerRow; ++page) {
                FixedStringBuilder<4096> indices("| Index |");
                FixedStringBuilder<4096> separators("| :--- |");
                FixedStringBuilder<4096> volts("| Volts |");
                int begin = page * ColsPerRow;
                int end = (page + 1) * ColsPerRow;
                if (notesPerOctave < end) end = notesPerOctave;
                for (int note = begin; note < end; ++note) {
                    indices(" %d |", note + 1);
                    separators(" --- |");
                    volts(" %.3f |", scale.noteToVolts(note));
                }
                DBG("%s", (const char *)(indices));
                DBG("%s", (const char *)(separators));
                DBG("%s", (const char *)(volts));
                DBG("");
            }
        }
        DBG("----------------------------------------");
    
}

#endif // PLATFORM_SIM
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_notename_notetovolts);
    RUN_TEST(test_02_notefromvolts);
    RUN_TEST(test_03_markdown);
    return UNITY_END();
}

