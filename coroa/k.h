/*
 * ======================================
 * SPDX-License-Identifier: GPL-3.0-only
 * Copyright (c) 2026 Elizeu S. Souza
 * ======================================
 */

#pragma once

typedef char i08;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u08;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u32 rune;
typedef u64 size;	 // TODO: u32 for 32-bit targets.
typedef char *str;
typedef struct err err;
typedef typeof(nullptr) null;

#define ALIGNUP(n, al) \
	(((n) + (al) - 1) & ~((al) - 1))

typedef u64 adr;
typedef u64 padr;
typedef u64 pgd;
typedef u64 pud;
typedef u64 pmd;
typedef u64 pte;
typedef u08 mprot;
typedef __builtin_va_list valist;

constexpr null nil = nullptr;

#define VAINIT(va, ls) __builtin_va_start(va, ls)
#define VADNIT(va) __builtin_va_end(va)
#define VAGET(va, ty) __builtin_va_arg(va, ty)

#define ASSERT(cond, msg, ...)\
	assert( \
		cond, \
		__FILE__ ", line %u: " \
		"assert(" #cond ") failed. \"" msg "\"", \
		__LINE__ __VA_OPT__(,) __VA_ARGS__ \
	)

enum {
	PNONE = 0,
	PREAD = 2,
	PWRITE = 4,
	PEXE = 8
};

struct err {
	str s;
};

/* lib.c */

/*
 * Note that vprintf and printf
 * are different to the ISO C
 * ones.
 */

void putc(char c);
void puts(str s);
void vprintf(str fmt, valist va);
void printf(str fmt, ...);
void memset(void *b, u08 c, size n);
void memcpy(void *b, void *src, size n);
size strlen(str s);
bool streq(str s, str o);
[[noreturn]]
void panic(str s, ...);
[[noreturn]]
void vpanic(str s, valist va);
void assert(bool cond, str s, ...);
void error(str s, ...);
void debug(str s, ...);
void warn(str s, ...);
void cont(str s, ...);

/* arch */

void arch_putc(char c);
void arch_halt();

/* main.c */

void main(void);