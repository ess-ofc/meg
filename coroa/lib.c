/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#include "k.h"

void
putc(char c) {
	arch_putc(c);
}

void
puts(str s) {
	if(s == nil)
		s = "nil str";
	while(*s) {
		arch_putc(*s++);
	}
}

void
vprintf(str fmt, valist va) {
	constexpr size BMAX = 256;
	char buf[BMAX];
	size n = 0;
	str s = fmt;

	while(*s != '\0') {
		if(*s != '%') {
			putc(*s++);
			continue;
		}
		s++;

		switch(*s++) {
		case '%':
			putc('%');
			continue;
		case 's':
			puts(VAGET(va, str));
			continue;
		case 'i':
			i64 i;
			if(*s == 'l') {
				s++;
				i = VAGET(va, i64);
			} else {
				i = VAGET(va, i32);
			}

			if(i == 0) {
				putc('0');
				continue;
			}

			if(i < 0)
				putc('-');
			for(; i != 0;) {
				i64 v = i % 10;
				i -= v;
				i /= 10;
				if(n < BMAX)
					buf[n++] = v + '0';
			}
			goto flush;
		case 'u':
			u64 u;
			if(*s == 'l') {
				s++;
				u = VAGET(va, u64);
			} else {
				u = VAGET(va, u32);
			}

			if(u == 0) {
				putc('0');
				continue;
			}

			for(; u > 0;) {
				i64 v = u % 10;
				u -= v;
				u /= 10;
				if(n < BMAX) {
					buf[n++] = v + '0';
				}
			}
			goto flush;
		case 'x':
			u64 x;
			if(*s == 'l') {
				s++;
				x = VAGET(va, u64);
			} else {
				x = VAGET(va, u32);
			}

			puts("0x");
			if(x == 0) {
				putc('0');
				continue;
			}

			for(; x > 0;) {
				i64 v = x % 16;
				x -= v;
				x /= 16;
				if(n < BMAX) {
					buf[n++] = v < 10 ?
						v + '0' :
						v + 'a';
				}
			}
			goto flush;
		case 'b':
			bool b = VAGET(va, i32);
			if(b) {
				puts("true");
				continue;
			}
			puts("false");
			continue;
		default:
			puts("%(invalid '");
			putc(s[-1]);
			puts("')");
			continue;
		}

	flush:
		/* Back to front. */
		while(n > 0) {
			putc(buf[--n]);
		}
	}
}

void
printf(str fmt, ...) {
	valist va;
	VAINIT(va, fmt);
	vprintf(fmt, va);
	VADNIT(va);
}

void
memset(void *b, u08 c, size n) {
	u08 *p = b;
	while(n--) {
		p[n] = c;
	}
}

void
memcpy(void *b, void *s, size n) {
	u08 *p1 = b, *p2 = s;
	for(size i = 0; i < n; i++) {
		p1[i] = p2[i];
	}
}

size
strlen(str s) {
	size len = 0;

	while(*s++)
		len++;
	return len;
}

bool
streq(str s, str o) {
	while(*s && *o) {
		if(*s != *o)
			return false;
		s++, o++;
	}

	if(*s)
		return false;
	return true;
}

/*
 * k is the label. k = "error"
 * prints:
 *   [ error ] message
 * when k is nil, its a
 * continuation:
 *   [ error ] message
 *             continuation
 */
static void
diag(str k, str s, valist va) {
	constexpr size SP = 10;
	static size klen = 0;
	size sp = 0;

	if(k == nil) {
		if(klen < SP)
			sp = SP - klen;
		sp += klen + 5;
		while(sp--)
			putc(' ');
		vprintf(s, va);
		putc('\n');
		return;
	}

	klen = strlen(k);
	if(klen < SP)
		sp = SP - klen;
	while(sp--)
		putc(' ');

	printf("[ %s ] ", k);
	vprintf(s, va);
	putc('\n');
}

void
vpanic(str s, valist va) {
	diag("Panic!", s, va);
	VADNIT(va);
	for(;;) {
		arch_halt();
	}
}

void
panic(str s, ...) {
	valist va;
	VAINIT(va, s);
	vpanic(s, va);
}

void
assert(bool cond, str s, ...) {
	valist va;

	if(cond)
		return;
	VAINIT(va, s);
	vpanic(s, va);
}

void
error(str s, ...) {
	valist va;

	VAINIT(va, s);
	diag("error", s, va);
	VADNIT(va);
}

void
debug(str s, ...) {
	valist va;

	VAINIT(va, s);
	diag("debug", s, va);
	VADNIT(va);
}

void
warn(str s, ...) {
	valist va;

	VAINIT(va, s);
	diag("warning", s, va);
	VADNIT(va);
}

void
cont(str s, ...) {
	valist va;

	VAINIT(va, s);
	diag(nil, s, va);
	VADNIT(va);
}