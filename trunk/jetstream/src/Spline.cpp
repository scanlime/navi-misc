/*
 * Spline.cpp - Spline classes
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

#include "Spline.h"

Spline3::Spline3() {
}

Vector3	Spline3::compute(float distance) {
  float b[4];
  bernstein(distance, b);
  return (p[0] * b[0] + p[1] * b[1] + p[2] * b[2] + p[3] * b[3]);
}

void Spline3::bernstein(float u, float b[4]) {
  /* return the Bernstein cubic polynomials. Since there's so much state
   * shared between all the polynomials, do it in one fell swoop. */
  /* B0(u) = (1-u)^3
   * B1(u) = 3u(1-u)^2
   * B2(u) = 3u^2(1-u)
   * B3(u) = u^3
   */
   float onemu = 1.0f - u;
   b[3] = u * u * u;
   b[2] = ((3.0f * u) * (3.0f * u)) * onemu;
   b[1] = (3.0f * u) * (onemu * onemu);
   b[0] = onemu * onemu * onemu;
}

void Spline3::set1(unsigned int which, unsigned int index, float value) {
  (p[which])[index] = value;
}

float Spline3::get1(unsigned int which, unsigned int index) {
  return (p[which])[index];
}

void Spline3::setv1(unsigned int which, Vector3 v) {
  p[which] = v;
}
