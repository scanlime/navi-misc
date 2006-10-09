/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * math-util.h - Small math utilities shared by other modules
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#ifndef __MATH_UTIL_H__
#define __MATH_UTIL_H__

void math_init();

int int_variate(int minimum, int maximum);
double uniform_variate();
void normal_variate_pair(double *a, double *b);

int find_upper_pow2(int x);

#endif /* __MATH_UTIL_H__ */

/* The End */
