/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#pragma once

#include "../k.h"

#define MAXFDTSZ 1'048'576

typedef struct fdt_hdr fdt_hdr;
struct fdt_hdr {
	u32 mag;
	u32 size;
	u32 structoff;
	u32 stroff;
	u32 offrvmem;
	u32 ver;
	u32 lastver;
	u32 cpuid;
	u32 structsz;
	u32 strsz;
};

typedef enum fdt_tokk fdt_tokk;
enum fdt_tokk {
	FDT_BEGNODE = 0x1,
	FDT_ENDNODE = 0x2,
	FDT_PROP = 0x3,
	FDT_NOP = 0x4,
	FDT_END = 0x9
};

typedef struct fdt_tok fdt_tok;
struct fdt_tok {
	fdt_tokk k;
	union {
		char name[];
		struct {
			u32 len;
			u32 off;
			char val[];
		} prop;
	};
};

fdt_tok *fdt_next(fdt_tok *t);
fdt_tok *fdt_struc(fdt_hdr *h);
str fdt_str(fdt_hdr *h, u32 off);
fdt_tok *fdt_subtok(fdt_hdr *h, fdt_tok *pa, str nm);
fdt_tok *fdt_gettok(fdt_hdr *h, str nm);
err fdt_check(fdt_hdr *h);