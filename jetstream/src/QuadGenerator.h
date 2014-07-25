/*
 * QuadGenerator.h - SurfaceGenerator implementation for making quadrilaterals
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

#ifndef _H_QUADGENERATOR
#define _H_QUADGENERATOR

#include "Surface.h"

class QuadGenerator : public SurfaceGenerator {
 public:

  virtual void generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint,
				SurfaceQuadtreeNode *qn);
  virtual void generateBoundingSphere(Vector3 &center, float &radius);
  virtual void generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
				VertexBuffer<SurfacePoint> &vbuffer);

  /* Dictionary keys: 
   *
   *   ax,ay,az \
   *   bx,by,bz  | - Four points in counterclockwise order
   *   cx,cy,cz  |
   *   dx,dy,dz /
   */

  /* JetCOW hooks - never call directly */
  QuadGenerator(JetCOW *cow, Sint32 id,  const char *type="QuadGenerator");
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  

 protected:
  Vector3 a,b,c,d,normal;
};

#endif /* _H_TRIANGLEGENERATOR */

/* The End */
