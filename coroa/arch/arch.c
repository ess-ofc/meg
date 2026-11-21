/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#include "arch.h"

#define swap(i) __builtin_bswap32(i)
#define swapl(i) __builtin_bswap64(i)

// Provisory.
#define UART0 ((volatile char *) 0x9000000)

static volatile char *serial0;

void
arch_handle(u64 num, u64 esr) {
	panic(
		"exception: %il,"
		" esr_el1 = %xl",
		num,
		esr
	);
}

void
arch_putc(char c) {
	if(serial0 == nil)
		return;

	if(c == '\n')
		*serial0 = '\r';
	*serial0 = c;
}

static void
init_serial(fdt_hdr *fdt) {
	fdt_tok *t, *ser;
	u32 *p;

	t = fdt_gettok(fdt, "/aliases/serial0");
	if(t == nil)
		return;

	ser = fdt_gettok(fdt, t->prop.val);
	if(ser == nil)
		return;

	t = fdt_subtok(fdt, ser, "reg");
	p = (u32 *) t->prop.val;
	serial0 = (void *) (u64) swap(p[1]);

	debug("serial = %xl", serial0);
}

[[noreturn]]
void
arch_init(void *fdt) {
	/* First, pre analyse dtb. */
	if(fdt_check(fdt).s)
		goto bad;

	init_serial(fdt);
	debug("arm64 initialising");

bad:
	for(;;)
		continue;
}