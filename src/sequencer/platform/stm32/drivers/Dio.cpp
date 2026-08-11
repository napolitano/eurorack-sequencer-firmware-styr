/**
 * @file Dio.cpp
 * @brief Implements the Dio component used by the STM32 sequencer driver.
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
#include "Dio.h"

#include "SystemConfig.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/exti.h>

static Dio *g_dio;

Dio::Dio() {
    g_dio = this;
}

void Dio::init() {
    rcc_periph_clock_enable(RCC_GPIOB);

    clockInput.init();
    resetInput.init();
    clockOutput.init();
    resetOutput.init();

    rcc_periph_clock_enable(RCC_SYSCFG);

    nvic_set_priority(NVIC_EXTI15_10_IRQ, CONFIG_DIO_IRQ_PRIORITY);
    nvic_enable_irq(NVIC_EXTI15_10_IRQ);

    exti_select_source(EXTI10 | EXTI11, GPIOB);
    exti_set_trigger(EXTI10 | EXTI11, EXTI_TRIGGER_BOTH);
    exti_enable_request(EXTI10 | EXTI11);
}

void exti15_10_isr(void) {
    if (exti_get_flag_status(EXTI10)) {
        g_dio->resetInput.interrupt();
        exti_reset_request(EXTI10);
    }
    if (exti_get_flag_status(EXTI11)) {
        g_dio->clockInput.interrupt();
        exti_reset_request(EXTI11);
    }
}
