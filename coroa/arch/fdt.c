/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#include "arch.h"

#define swap(i) __builtin_bswap32(i)

static fdt_hdr *hdr;

fdt_tok *
fdt_struc() {
	u08 *p = (u08 *) hdr;
	p += swap(hdr->structoff);
	return (fdt_tok *) p;
}

str
fdt_str(u32 off) {
	str p = (str) hdr;
	p += swap(hdr->stroff);
	return p + off;
}

fdt_tok *
fdt_skip(fdt_tok *t) {
	size depth = 0;

	ASSERT(
	   fdt_k(t) == FDT_BEGNODE,
	   "not a nod"
	);

	while((t = fdt_next(t))) {
		switch(fdt_k(t)) {
		case FDT_BEGNODE:
			depth++;
			break;
		case FDT_ENDNODE:
			if(depth == 0)
				return fdt_next(t);
			depth--;
			break;
		case FDT_PROP:
		case FDT_NOP:
			break;
		case FDT_END:
			panic(
			   "fdt_skip():"
			   " early fdt end"
			);
		}
	}

	panic("fdt_skip(): nil node");
}

fdt_tok *
fdt_next(fdt_tok *t) {
	/* Humm, this code smells. */

	switch(fdt_k(t)) {
	case FDT_BEGNODE:
		return (void *) &t
		   ->name[ALIGNUP(
			  strlen(t->name) + 1, 4
		   )];
	case FDT_PROP:
		return (void *) &t->prop
		   .val[ALIGNUP(
			  swap(t->prop.len), 4
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
fdt_subtok(fdt_tok *pa, str nm) {
	fdt_tok *t;
	str tn;

	ASSERT(
	   fdt_k(pa) == FDT_BEGNODE,
	   "not a node"
	);

	t = fdt_next(pa);
	for(;;) {
		switch(fdt_k(t)) {
		case FDT_BEGNODE:
			if(streq(t->name, nm))
				return t;
			fdt_skip(t);
			break;

		case FDT_PROP:
			tn = fdt_name(t);
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
fdt_gettok(str p) {
	constexpr size MAX = 256;
	fdt_tok *t = nil;
	char buf[MAX];
	size n;

	if(*p == '/') {
		t = fdt_struc();
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

		memcpy(buf, (void *) p, e - p);
		buf[n] = '\0';
		p = e;

		ASSERT(
		   fdt_k(t) == FDT_BEGNODE,
		   "not a node"
		);
		t = fdt_subtok(t, buf);
		if(t == nil)
			return nil;

		if(*p == '/')
			p++;
		if(*p == '0') {
			return t;
		}
	}
}

bool
fdt_match(fdt_tok *t, str compat) {
	size len;
	str p, end;

	ASSERT(
	   fdt_k(t) == FDT_BEGNODE,
	   "not a node"
	);

	t = fdt_subtok(t, "compatible");
	if(t == nil)
		return false;
	len = swap(t->prop.len);
	p = t->prop.val;
	end = &p[len];

	/* Iterate the string list. */
	while(p < end) {
		if(streq(p, compat))
			return true;
		while(*p++ != '\0') {}
	}

	return false;
}

u64
fdt_readnum(u32 *c, size n) {
	u64 r = 0;
	while(n--)
		r = (r << 32) | swap(*c++);
	return r;
}

i32
fdt_regval(
   fdt_tok *t,
   size ac,
   size sc,
   size idx,
   u64 *bs,
   u64 *sz
) {
	size elen = ac + sc;
	u32 *c;

	ASSERT(
	   fdt_k(t) == FDT_BEGNODE,
	   "not a node"
	);

	t = fdt_subtok(t, "reg");
	if(t == nil) {
		*bs = 0;
		*sz = 0;
		return 1;
	}

	c = (u32 *) t->prop.val;
	c += elen * idx;
	*bs = fdt_readnum(c, ac);
	*sz = fdt_readnum(c + ac, sc);
	return 0;
}

u32
fdt_numval(fdt_tok *t) {
	ASSERT(
	   fdt_k(t) == FDT_PROP,
	   "not a property"
	);
	return swap(*(u32 *) t->prop.val);
}

str
fdt_name(fdt_tok *t) {
	switch(fdt_k(t)) {
	case FDT_BEGNODE:
		return t->name;
	case FDT_PROP:
		return fdt_str(
		   swap(t->prop.off)
		);
	default:
		ASSERT(
		   false,
		   "expected node or property,"
		   "k = %x",
		   fdt_k(t)
		);
		return nil;
	}
}

fdt_tokk
fdt_k(fdt_tok *t) {
	return swap(t->k);
}

i32
fdt_set(fdt_hdr *h) {
	u32 mag, ver, sz;

	if(h == nil)
		return 1;

	mag = swap(h->mag);
	if(mag != 0xd00dfeed)
		return 1;

	ver = swap(h->ver);
	if(ver < 17)
		return 1;

	sz = swap(h->size);
	if(sz > MAXFDTSZ)
		return 1;

	hdr = h;
	return 0;
}
