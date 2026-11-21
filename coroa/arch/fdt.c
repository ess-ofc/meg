/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#include "arch.h"

#define swap(i) __builtin_bswap32(i)

fdt_tok *
fdt_struc(fdt_hdr *h) {
	u08 *p = (u08 *) h;
	p += swap(h->structoff);
	return (fdt_tok *) p;
}

str
fdt_str(fdt_hdr *h, u32 off) {
	str p = (str) h;
	p += swap(h->stroff);
	return p + off;
}

fdt_tok *
fdt_next(fdt_tok *t) {
	switch(swap(t->k)) {
	case FDT_BEGNODE:
		return (void *) &t->name[ALIGNUP(
			strlen(t->name) + 1,
			4
		)];
	case FDT_PROP:
		return (void *) &t->prop.val[ALIGNUP(
			swap(t->prop.len),
			4
		)];
	case FDT_ENDNODE:
	case FDT_NOP:
		return (void *) (&t->k + 1);
	case FDT_END:
	default:
		return nil;
	}
}

fdt_tok *
fdt_subtok(fdt_hdr *h, fdt_tok *pa, str nm) {
	fdt_tok *t;
	fdt_tokk k;
	size l;
	str tn;

	k = swap(pa->k);
	debug("pa = %u", k);
	if(k != FDT_BEGNODE)
		return nil;

	t = fdt_next(pa);
	for(;;) {
		k = swap(t->k);

		switch(k) {
		case FDT_BEGNODE:
			if(streq(t->name, nm))
				return t;

			l = 0;
			for(;;) {
				t = fdt_next(t);
				k = swap(t->k);

				switch(k) {
				case FDT_BEGNODE:
					l++;
					break;
				case FDT_ENDNODE:
					if(l == 0)
						goto end;
					l--;
					break;

				default:
				}
			}
		end:
			break;

		case FDT_PROP:
			tn = fdt_str(h, swap(t->prop.off));
			if(streq(tn, nm))
				return t;
			break;

		case FDT_NOP:
			break;

		default:
			return nil;
		}

		t = fdt_next(t);
	}
}

fdt_tok *
fdt_gettok(fdt_hdr *h, str p) {
	constexpr size MAX = 256;
	fdt_tok *t = nil;
	char buf[MAX];
	size n;

	if(*p == '/') {
		t = fdt_struc(h);
		p++;
	}

	if(*p == '\0')
		return t;

	for(;;) {
		str e;
		n = 0;

		for(e = p; *e != '\0'; e++, n++)
			if(*e == '/')
				break;

		memcpy(buf, p, e - p);
		buf[n] = '\0';
		p = e;

		t = fdt_subtok(h, t, buf);
		if(t == nil)
			return nil;

		if(*p == '/') {
			p++;
			continue;
		}
		return t;
	}
}

err
fdt_check(fdt_hdr *h) {
	u32 mag, ver, sz;

	mag = swap(h->mag);
	if(mag != 0xd00dfeed)
		return (err){"invalid device tree"};

	ver = swap(h->ver);
	if(ver < 17)
		return (err){"invalid version"};

	sz = swap(h->size);
	if(sz > MAXFDTSZ)
		return (err){"fdt too large"};

	return (err){};
}