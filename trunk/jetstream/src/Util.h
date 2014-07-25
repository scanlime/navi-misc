/*
 * Util.h - Random utility functions
 *
 * Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _H_UTIL
#define _H_UTIL

#include <stdlib.h>
extern "C" {
#include <SDL_endian.h>
#include <SDL_types.h>
}

#define rand01() (((float)rand())/RAND_MAX)
#define EPSILON 0.0001
#define absf(n) ((n < 0.0f)?(n):(-n))
#define PI 3.1415927

/* Macros for byte swapping- the same as the ones in netinet/in.h,
 * but non-UNIX-specific.
 */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define jet_ntohl(x) SDL_Swap32(x)
#define jet_ntohs(x) SDL_Swap16(x)
#define jet_htonl(x) SDL_Swap32(x)
#define jet_htons(x) SDL_Swap16(x)
#else
#define jet_ntohl(x) (x)
#define jet_ntohs(x) (x)
#define jet_htonl(x) (x)
#define jet_htons(x) (x)
#endif

/* Types in network byte order */
typedef Uint8  u8n;
typedef Sint8  s8n;
typedef Uint16 u16n;
typedef Sint16 s16n;
typedef Uint32 u32n;
typedef Sint32 s32n;

/* Debugging 
 * This supports both assert() and the DBG() macro, only enabled
 * if DEBUG_FILE is set. Each file should optionally set DEBUG_FILE
 * before including its headers.
 */
#ifdef DEBUG_FILE
#include <stdio.h>
#define DBG(fmt, args...) printf( "%s: " fmt, __FUNCTION__ , ## args); fflush(stdout)
#else
#define DBG(fmt, args...)
#define NDEBUG
#endif
#include <assert.h>

/* Compiler-dependent tag for specifying a packed structure */
#ifdef __GNUC__
#define STRUCT_PACK __attribute__ ((packed));
#else
#define STRUCT_PACK
#warning Structures may not be packed!
#endif

#endif

/* The End */
