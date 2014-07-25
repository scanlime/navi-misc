/*
 * Noise.h - Noise utility classes
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

#ifndef _H_NOISE
#define _H_NOISE

#include <vector>
#include "Vector.h"

/* Fast 3D perlin noise generator.
 * Amplitude of the first octave is 1, fundamental frequency is 1
 */
class PerlinNoise3 {
 public:
  void set(int seed=0, int octaves=1, float persistence=0.5, int logTableSize=10);

  float getValue(const Vector3 &point);
  Vector3 getGradient(const Vector3 &point);

  float persistence;
  int octaves;
  int seed;

 private:
  std::vector<Vector3> gradTable;
  int tableMask;

  inline float noise(int n);
  inline Vector3 &tableNoise3(int x, int y, int z);
  inline float smoothNoise3(const Vector3 &point);
  inline Vector3 smoothNoiseGradient3(const Vector3 &point);
};

#endif /* _H_NOISE */

/* The End */
