/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * math-util.c - Small math utilities shared by other modules
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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

#include "math-util.h"
#include <glib.h>
#include <math.h>


float uniform_variate() {
    /* A uniform random variate between 0 and 1 */
    return g_random_double();
}

float normal_variate() {
    /* A unit-normal random variate, implemented with the Box-Muller method */
    return sqrt(-2*log(g_random_double())) * cos(g_random_double() * (2*M_PI));
}

int find_upper_pow2(int x) {
    /* Find the smallest power of two greater than or equal to x */
    int p = 1;
    while (p < x)
	p <<= 1;
    return p;
}

/* The End */
