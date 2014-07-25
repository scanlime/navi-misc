/*
 * PerlinDisplacement.h - Surface modifier for midpoint displacement
 *                        using perlin noise.
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

#ifndef _H_PERLINDISPLACEMENT
#define _H_PERLINDISPLACEMENT

#include "Surface.h"
#include "SurfaceGenerator.h"
#include "Noise.h"


class PerlinDisplacement : public SurfaceModifier {
 public:
  virtual void generateBoundingSphere(Vector3 &center, float &radius);
  virtual void updateBounds(SurfaceQuadtreeNode *n, Surface *s);
  virtual void f(SurfacePoint &p, SurfaceQuadtreeNode *qn);

  /* Dictionary keys: 
   *    Fundamental  - Fundamental frequency of the noise
   *    Amplitude    - Maximum amplitude of the noise
   *    Octaves      - Number of octaves of noise to calculate
   *    Persistence  - Amount of amplitude decay between octaves
   *    Seed         - PRNG seed number
   */

  /* JetCOW hooks - never call directly */
  PerlinDisplacement(JetCOW *cow, Sint32 id,  const char *type="PerlinDisplacement");
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  

 protected:
  PerlinNoise3 noise;
  float amplitude, fundamental;
};


#endif /* _H_PERLINDISPLACEMENT */

/* The End */
