/*
 * PerlinDisplacement.cpp - Surface modifier for midpoint displacement
 *                          using perlin noise.
 *
 *    Terrain references:
 *        http://www.vterrain.org/Elevation/artificial.html
 *        http://www.dgp.toronto.edu/~dh/screenshots.html
 *        http://www.wizardnet.com/musgrave/dissertation.pdf
 *        Game Programming Gems, section 4.18
 *        http://users.bestweb.net/~hogdog/fractal.htm
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

#include "Surface.h"
#include "SurfaceGenerator.h"
#include "PerlinDisplacement.h"
#include "Noise.h"
#include <math.h>
#include <SDL/SDL_timer.h>


PerlinDisplacement::PerlinDisplacement(JetCOW *cow, Sint32 id,  const char *type) : 
  SurfaceModifier(cow,id,type) {
  if (id<0) {
    /* Defaults */
    setAttr("Fundamental", 1.0f);
    setAttr("Amplitude", 1.0f);
    setAttr("Persistence", 1.0f);
    setAttr("Octaves", 1);
    setAttr("Seed", (int)SDL_GetTicks());
  }
}

void PerlinDisplacement::loadCachedValues(void) {
  float persistence;
  int seed, octaves;
  
  SurfaceModifier::loadCachedValues();
  fundamental = getAttrFloatProtected("Fundamental");
  amplitude = getAttrFloatProtected("Amplitude");
  persistence = getAttrFloatProtected("Persistence");
  seed = getAttrIntProtected("Seed");
  octaves = getAttrIntProtected("Octaves");
  noise.set(seed,octaves,persistence);
  dirty = true;
}

void PerlinDisplacement::saveCachedValues(void) {
  SurfaceModifier::saveCachedValues();
  setAttrProtected("Fundamental", fundamental);
  setAttrProtected("Amplitude", amplitude);
  setAttrProtected("Octaves", noise.octaves);
  setAttrProtected("Persistence", noise.persistence);
  setAttrProtected("Seed", noise.seed);
}

void PerlinDisplacement::f(SurfacePoint &p, SurfaceQuadtreeNode *qn) {
  Vector3 oldVertex = p.vertex;
  Vector3 param = oldVertex * fundamental;
  float noiseValue = noise.getValue(param);

  p.vertex += p.normal * (noiseValue * amplitude);

  /* The gradient of the noise function is perpendicular to an isosurface
   * in the noise field through the original vertex. We can use this to
   * calculate the final normal of our surface:
   */
  Vector3 grad = noise.getGradient(param);

  /* Get two vectors perpendicular to the original normal */
  Vector3 n1 = p.normal;
  Vector3 n2 = p.normal.perpendicular();
  Vector3 n3 = n1 % n2;

  /* In addition to the vertex we just calculated, create two new
   * vertices that approximately lie on the surface. They are measured
   * relative to oldVertex, to reduce floating point error.
   * (Approximately, because this assumes the original surface is a plane)
   */
  Vector3 p1 =      (n1 * (noiseValue              ));
  Vector3 p2 = n2 + (n1 * (noiseValue + (n2 * grad)));
  Vector3 p3 = n3 + (n1 * (noiseValue + (n3 * grad)));

  /* If we assume p1,p2,p3 are the vertices of a triangle, the plane normal
   * of this triangle would be our new normal.
   */
  p.normal = (p2-p1) % (p3-p1);
  //  p.normal.normalize();
}

void PerlinDisplacement::generateBoundingSphere(Vector3 &center, float &radius) {
  parent->generateBoundingSphere(center,radius);
  radius += amplitude;
}

void PerlinDisplacement::updateBounds(SurfaceQuadtreeNode *n, Surface *s) {
  parent->updateBounds(n,s);
}

/* The End */
