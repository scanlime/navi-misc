/*
 * SurfaceGenerator.h - Base classes for the models used to build Surfaces
 *
 * Copyright (C) 2002-2003 Micah Dowty and David Trowbridge
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

#ifndef _H_SURFACEGENERATOR
#define _H_SURFACEGENERATOR

#include "JetCOW.h"
#include "VertexBuffer.h"
#include <vector>

class Surface;
class SurfaceQuadtreeNode;
class SurfacePoint;


/* An abstract base class for a method of generating a surface.
 * This consists of two fundamental operations:
 *   - generating a 'seed mesh' of triangles that will be refined
 *   - a function for generating a new vertex at the midpoint between two supplied vertices
 */
class SurfaceGenerator : public JetCOWDictionary {
 public:
  
  /* Push and pop opengl settings for drawing */
  virtual void push(void);
  virtual void pop(void);

  /* Note:
   *   The seed mesh must contain vertices and normals,
   *   but it shouldn't have neighbors- those will be generated by the Surface.
   */
  virtual void generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
				VertexBuffer<SurfacePoint> &vbuffer) = 0;

  /* Generate a midpoint for refining the mesh.
   * Default is a linear interpolation. The qn parameter isn't usually necessary to use.
   */
  virtual void generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint, 
				SurfaceQuadtreeNode *qn);
  
  /* generate a bounding sphere for the surface */
  virtual void generateBoundingSphere(Vector3 &center, float &radius) = 0;

  /* Update a quadtree node's bounding prism.
   * Default in an infinitely thin bounding prism.
   * The quadtree node's vertices, vertex normals, and plane normal are all
   * guaranteed to be valid at this point.
   */
  virtual void updateBounds(SurfaceQuadtreeNode *n, Surface *s);

  /* The generator can optionally be animated */
  virtual void animate(float dt);

  /* Test the dirty flag, and unset it if it's set */
  virtual bool testDirty(void);

  SurfaceGenerator(JetCOW *cow, Sint32 id,  const char *type="SurfaceGenerator");

  bool dirty;
};


/* Abstract base class for a surface generator that composes an arbitrary transformation
 * with another SurfaceGenerator. This could perform a per-vertex modification, add
 * texture layers, or anything else that could be represented in the SurfacePoint.
 */
class SurfaceModifier : public SurfaceGenerator {
 public:

  /* Push and pop opengl settings for drawing */
  virtual void push(void);
  virtual void pop(void);

  /* This function is applied to all points after being produced by the parent in
   * this class' default implementation of generateSeedMesh and generateMidpoint.
   * If the subclass overrides those functions, there's no need to provide this one.
   */
  virtual void f(SurfacePoint &p, SurfaceQuadtreeNode *qn);

  /* The implementations of these functions call the parent, then pass the result through f() */
  virtual void generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
				VertexBuffer<SurfacePoint> &vbuffer);
  virtual void generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint, 
				SurfaceQuadtreeNode *qn);

  /* This just calls the parent. If you are modifying any 
   * geometry this should probably be overridden by the subclass.
   */
  virtual void generateBoundingSphere(Vector3 &center, float &radius);
  virtual void updateBounds(SurfaceQuadtreeNode *n, Surface *s);

  /* Dictionary keys: 
   *    Parent - JetCOW object ID of the parent
   */

  /* JetCOW hooks - never call directly */
  SurfaceModifier(JetCOW *cow, Sint32 id,  const char *type="SurfaceModifier");
  virtual ~SurfaceModifier();
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  

  /* Test the dirty flag of this and the parent, and unset it if it's set */
  virtual bool testDirty(void);

 protected:
  SurfaceGenerator *parent;
};

#endif /* _H_SURFACEGENERATOR */

/* The End */
