/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * types.h - Low-level type, macro, and inline definitions.
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2008-2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __TYPES_H__
#define __TYPES_H__

typedef long long int64;
typedef unsigned long long uint64;

typedef int int32;
typedef unsigned int uint32;

typedef short int16;
typedef unsigned short uint16;

typedef char int8;
typedef unsigned char uint8;

typedef uint8 Bool;

typedef struct {
   int x, y;
} IVec2;

#define NULL   ((void*)0)
#define TRUE   1
#define FALSE  0

#define offsetof(type, member)  ((uint32)(&((type*)NULL)->member))
#define arraysize(var)          (sizeof(var) / sizeof((var)[0]))
#define roundup(x, y)           (((x) + ((y) - 1)) / (y))

#define PACKED       __attribute__ ((__packed__))
#define ALIGNED(n)   __attribute__ ((aligned(n)))
#define fastcall     __attribute__ ((fastcall))

#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#define MAX(a, b)   ((a) > (b) ? (a) : (b))

static inline void
memcpy(void *dest, const void *src, uint32 size)
{
   asm volatile ("cld; rep movsb" : "+c" (size), "+S" (src), "+D" (dest) :: "memory");
}

static inline void
memset(void *dest, uint8 value, uint32 size)
{
   asm volatile ("cld; rep stosb" : "+c" (size), "+D" (dest) : "a" (value) : "memory");
}

static inline void
memcpy16(void *dest, const void *src, uint32 size)
{
   asm volatile ("cld; rep movsw" : "+c" (size), "+S" (src), "+D" (dest) :: "memory");
}

static inline void
memset16(void *dest, uint16 value, uint32 size)
{
   asm volatile ("cld; rep stosw" : "+c" (size), "+D" (dest) : "a" (value) : "memory");
}

static inline void
memcpy32(void *dest, const void *src, uint32 size)
{
   asm volatile ("cld; rep movsl" : "+c" (size), "+S" (src), "+D" (dest) :: "memory");
}

static inline void
memset32(void *dest, uint32 value, uint32 size)
{
   asm volatile ("cld; rep stosl" : "+c" (size), "+D" (dest) : "a" (value) : "memory");
}

#define Atomic_Exchange(mem, reg) \
   asm volatile ("xchgl %0, %1" : "+r" (reg), "+m" (mem) :)

#define Atomic_Or(mem, reg) \
   asm volatile ("lock orl %1, %0" :"+m" (mem) :"r" (reg))

#endif /* __TYPES_H__ */

