/*
 * setjmp.h - A simple setjmp/longjmp wrapper around IntrContext.
 */

#ifndef __SETJMP_H__
#define __SETJMP_H__

#include "intr.h"

typedef IntrContext jmp_buf;

#define setjmp(buf)       ((int)Intr_SaveContext(&(buf)))
#define longjmp(buf,val)  _longjmp(&(buf), (val))

static inline void _longjmp(jmp_buf *env, int val)
{
   env->eax = val;
   Intr_RestoreContext(env);
}

#endif /* __SETJMP_H__ */
