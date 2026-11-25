/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#include "arch.h"

static volatile char *serial = INITSER;

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
	if(c == '\n')
		*serial = '\r';
	*serial = c;
}

void
arch_backtrace() {
	void **fp =
	   __builtin_frame_address(0);
	cont("--Backtrace--");
	for(;;) {
		cont(
		   "%xl, ret = %xl",
		   fp[0],
		   fp[1]
		);
		fp = fp[0];
		if(fp == nil) {
			cont("--end--");
			break;
		}
	}
}

static void
clearbss() {
	extern u08 __bss[], __bss_end[];
	memset(__bss, 0, __bss_end - __bss);
}

static void
initser(u64 adr, u64) {
	if(serial == nil)
		serial = (void *) adr;
}

static void
getcells(
   fdt_tok *t,
   str nm,
   size *ac,
   size *sc
) {
	if(streq(nm, "#address-cells")) {
		*ac = fdt_numval(t);
		return;
	}
	if(streq(nm, "#size-cells")) {
		*sc = fdt_numval(t);
		return;
	}
}

static void
essential(
   fdt_tok *t,
   size ac,
   size sc
) {
	fdt_tok *st;
	str dev;

	ASSERT(
	   fdt_k(t) == FDT_BEGNODE,
	   "not a node"
	);

	st = fdt_subtok(t, "device_type");
	if(st == nil)
		return;

	dev = st->prop.val;
	if(streq(dev, "memory")) {
		size ad, sz;

		if(fdt_regval(
			  t, ac, sc, 0, &ad, &sz
		   )) {
			return;
		}

		/* Init memblock here. */
		return;
	}

	/* TODO: if streq(dev, "cpu") */
	return;
}

static i32
analynod(
   fdt_hdr *h,
   fdt_tok *t,
   size ac,
   size sc
) {
	fdt_tok *ty;

	if(t == nil) {
		t = fdt_struc();
		ac = 2, sc = 1;
	}

	ASSERT(
	   fdt_k(t) == FDT_BEGNODE,
	   "not a node"
	);

	ty = t;
	while((ty = fdt_next(ty))) {
		switch(fdt_k(ty)) {
		case FDT_BEGNODE:
			analynod(h, ty, ac, sc);
			fdt_skip(t);
			break;
		case FDT_PROP:
			str nm = fdt_name(ty);
			if(*nm == '#') {
				getcells(
				   ty, nm, &ac, &sc
				);
				break;
			}
			if(streq(
				  nm, "device_type"
			   )) {
				essential(t, ac, sc);
				break;
			}
		case FDT_NOP:
			break;
		case FDT_ENDNODE:
			return 0;
		case FDT_END:
			return 1;
		}
	}
	return 0;
}

static void
analysefdt(fdt_hdr *h) {
	fdt_tok *t;

	if(fdt_set(h))
		panic("invalid fdt");

	analynod(h, nil, 0, 0);

	t = fdt_gettok("/aliases");
	if(t != nil) {
		size
	}
}

[[noreturn]]
void
arch_init(void *fdt) {
	clearbss();
	analysefdt(fdt);

	puts(COROA);  // very important

	for(;;) {}
}
