/*
 * SuperquadricGenerator.h - SurfaceGenerator implementations for making superquadrics
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

#ifndef _H_SUPERQUADRICGENERATOR
#define _H_SUPERQUADRICGENERATOR

#include "Surface.h"

class SuperellipsoidGenerator : public SurfaceGenerator {
 public:
  virtual void generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint,
				SurfaceQuadtreeNode *qn);
  virtual void generateBoundingSphere(Vector3 &center, float &radius);
  virtual void generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
				VertexBuffer<SurfacePoint> &vbuffer);
  virtual void updateBounds(SurfaceQuadtreeNode *n, Surface *s);

  /* Dictionary keys: 
   *
   *   Radius   - Note that the center is always at 0,0,0
   *   xe,ye,ze - x, y, and z exponents
   *
   */

  /* JetCOW hooks - never call directly */
  SuperellipsoidGenerator(JetCOW *cow, Sint32 id,  const char *type="SuperellipsoidGenerator");
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  

 protected:
  float radius, xe,ye,ze;

  /* A pow() function symmetric for positive and negative inputs */
  float abspow(float x, float e);
};

#endif /* _H_SUPERQUADRIC */

/* The End */
