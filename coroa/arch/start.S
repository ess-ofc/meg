/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#include "cpu.inc"

.section .head.text, "ax"
.align 11
    b _start
    .long 0
    .quad 0
    .quad __image_size
    .quad 0
    .quad 0
    .quad 0
    .quad 0
    .ascii "ARM\x64"
    .long 0

.align 16
_stack_top:
    .skip 16384
_stack_bottom:

.text
.global _start
.type _start, @function
_start:
    adrp x1, _stack_bottom
    add  x1, x1, :lo12:_stack_bottom
    mov  sp, x1

    adrp x1, vectors
    add  x1, x1, :lo12:vectors
    msr  vbar_el1, x1
    msr  daif, xzr
    
    b    arch_init // It cannot return.
idle:
    bl   arch_halt
    b    idle

.global arch_halt
.type arch_halt, @function
arch_halt:
    dsb sy
    wfi
    ret

.macro handle num
.p2align 7
    mov  x0, \num
    mrs  x1, esr_el1
    bl   arch_handle
    b    .
.endm

.p2align 11
vectors:
    handle #0
    handle #1
    handle #2
    handle #3

    handle #4
    handle #5
    handle #6
    handle #7

    handle #8
    handle #9
    handle #10
    handle #11

    handle #12
    handle #13
    handle #14
    handle #15
