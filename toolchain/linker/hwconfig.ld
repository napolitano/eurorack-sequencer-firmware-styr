/*
 * Purpose: Defines the memory layout and linker sections for the embedded linker configuration.
 * Authors: Simon Kallweit — original PER|FORMER implementation lineage;
 *          Axel Napolitano — Styr modifications, integration and modernization
 * Copyright: 2017-2018 Simon Kallweit; 2026 Axel Napolitano
 * License: MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
/* Linker script for ST STM32F405RGTx (1024K flash, 192K RAM) */

/* Define memory regions. */
MEMORY
{
	ROM (rx) : ORIGIN = 0x08008000, LENGTH = 16K
}

/* Define sections. */
SECTIONS
{
	.hwconfig : {
        *(.hwconfig*)
        KEEP (*(.hwconfig))
	} >ROM

}
