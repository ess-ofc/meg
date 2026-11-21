/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#pragma once

// Definitions to set MAIR_EL1 Register.

/// Attributes.
#define MAIR_ATTR_NORMAL          0xff
#define MAIR_ATTR_NORMAL_NC       0x44
#define MAIR_ATTR_DEVICE_NG_NR_NE 0x00

/// Indexes
#define MAIR_ATTR_INDEX_NORMAL            0
#define MAIR_ATTR_INDEX_NORMAL_NC         1
#define MAIR_ATTR_INDEX_DEVICE_NG_NR_NE   2

#define MAIR_ATTR_INDEX(attr, indx) (attr << indx * 8)

#define MAIR_SETUP \
   (MAIR_ATTR_INDEX(MAIR_ATTR_NORMAL, MAIR_ATTR_INDEX_NORMAL)                  | \
    MAIR_ATTR_INDEX(MAIR_ATTR_NORMAL_NC, MAIR_ATTR_INDEX_NORMAL_NC)             | \
    MAIR_ATTR_INDEX(MAIR_ATTR_DEVICE_NG_NR_NE, MAIR_ATTR_INDEX_DEVICE_NG_NR_NE))

#define TCR_SETUP ( \
   (0b101    << 32) | /* IPS 48 bits */ \
   (0b01     << 30) | /* TG1 16 KB */ \
   (0b11     << 28) | /* SH1 Inner Sharable */ \
   (0b01     << 26) | /* ORGN1 Normal memory, Outer Write-Back Write-Allocate Cacheable */ \
   (0b01     << 24) | /* IRGN1 Normal memory, Innert Write-Back Write-Allocate Cacheable */ \
   (0b0      << 23) | /* EPD1 Disable table walking */ \
   (0b0      << 22) | /* A1 ttbr0_el1 defines the ASID */ \
   (0b011001 << 16) | /* T1SZ 25 */ \
   (0b10     << 14) | /* TG0 16 KB */ \
   (0b11     << 12) | /* SH0 Inner Sharable */ \
   (0b01     << 10) | /* ORGN0 Normal memory, Outer Write-Back Write-Allocate Cacheable */ \
   (0b01     <<  8) | /* IRGN0 Normal memory, Inner Write-Back Write-Allocate Cacheable */ \
   (0b0      <<  7) | /* EPD0 Disable table walking */ \
   (0b0      <<  6) | /* Reserved */ \
   (0b011001 <<  0)   /* T0SZ 25 */ \
)

// Auxiliary assembly macros.

/// Moves 64 bits literals in four mov* instructions...
.macro mov_q, reg, lit
   movz   \reg, #(\lit & 0xffff), lsl #0
   movk   \reg, #((\lit >> 16) & 0xffff), lsl #16
   movk   \reg, #((\lit >> 32) & 0xffff), lsl #32
   movk   \reg, #((\lit >> 48) & 0xffff), lsl #48
.endm
