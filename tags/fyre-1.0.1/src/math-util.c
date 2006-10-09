/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * math-util.c - Small math utilities shared by other modules
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

#include "math-util.h"
#include <glib.h>
#include <math.h>

/* It's much faster to use our own global g_rand, rather than
 * relying on the g_random_* family of functions. Those functions
 * are thread-safe, and the locking around that shared g_rand can
 * take a very significant amount of CPU. We don't need thread-safe
 * random variates yet.
 */
static GRand* global_random = NULL;

void math_init() {
    global_random = g_rand_new_with_seed(time(NULL));
}

double uniform_variate() {
    /* A uniform random variate between 0 and 1 */
    return g_rand_double(global_random);
}

void normal_variate_pair(double *a, double *b) {
    /* Produce a pair of values with a standard normal distribution,
     * using the Polar Box-Mueller method.
     */
    double x, y, r2, m;

    do {
	x = uniform_variate();
	x += x - 1;
	y = uniform_variate();
	y += y - 1;

	/* Squared radius. The vector must be nonzero,
	 * and it must lie within the unit circle.
	 */
	r2 = x*x + y*y;
    } while (r2 > 1 || r2 == 0);
    
    /* Perform the Box-Mueller transform on each component */
    m = sqrt(-2.0 * log(r2) / r2);
    *a = x * m;
    *b = y * m;
}

int int_variate(int minimum, int maximum) {
    return g_rand_int_range(global_random, minimum, maximum);
}

int find_upper_pow2(int x) {
    /* Find the smallest power of two greater than or equal to x */
    int p = 1;
    while (p < x)
	p <<= 1;
    return p;
}

/* The End */
