/*
 * oconfig.h
 * 
 * Basic config header for FX2 pipe code. 
 * 
 * NOTE: This file must be included first by all other source code files. 
 * 
 * Copyright (c) 2006 by Wolfgang Wieser ] wwieser (a) gmx <*> de [ 
 * 
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation. (See COPYING.GPL for details.)
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 */

#ifndef _INCLUDE_FX2PIPE_OCONFIG_H_
#define _INCLUDE_FX2PIPE_OCONFIG_H_ 1

#define QT_THREAD_SUPPORT 1

// Include autoconf-generated config header: 
#include "config.h"

#include <stdio.h>
#include <sys/types.h>  /* for ssize_t... */

typedef u_int64_t uint64;
typedef u_int32_t uint32;
typedef u_int16_t uint16;
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;

// We need to define this for some linux flavours since sometimes 
// likely/unlikely are not part of system headers for non-kernel-compiles 
// although they are needed at certain other places...
#define likely(x)   (x)
#define unlikely(x) (x)

// Some basics...

/// Operator delete which will do nothing for NULL pointers and set the passed 
/// pointer to NULL after deletion. 
template<typename T> inline void DELETE(T* &ptr)
	{  if(ptr)  {  delete ptr;  ptr=NULL;  }  }

#if 1
# include <new>
#else
/// Operator new with placement. 
inline void *operator new(size_t,void *ptr)
	{  return(ptr);  }
#endif

#endif  /* _INCLUDE_FX2PIPE_OCONFIG_H_ */
