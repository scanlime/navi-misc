/*
 * TriangleGenerator.cpp - SurfaceGenerator implementations for making single triangles
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

#include "TriangleGenerator.h"

TriangleGenerator::TriangleGenerator(JetCOW *cow, Sint32 id,  const char *type) : SurfaceGenerator(cow,id,type) {
  if (id<0) {
    /* Defaults */
    setAttr("ax",0.0f);
    setAttr("ay",0.0f);
    setAttr("az",0.0f);
    setAttr("bx",0.0f);
    setAttr("by",0.0f);
    setAttr("bz",0.0f);
    setAttr("cx",0.0f);
    setAttr("cy",0.0f);
    setAttr("cz",0.0f);
  }
}

void TriangleGenerator::loadCachedValues(void) {
  a.set(getAttrFloatProtected("ax"),
	getAttrFloatProtected("ay"),
	getAttrFloatProtected("az"));
  b.set(getAttrFloatProtected("bx"),
	getAttrFloatProtected("by"),
	getAttrFloatProtected("bz"));
  c.set(getAttrFloatProtected("cx"),
	getAttrFloatProtected("cy"),
	getAttrFloatProtected("cz"));

  normal = (c-a) % (b-a);
  normal.normalize();
  dirty = true;
}

void TriangleGenerator::saveCachedValues(void) {
  setAttrProtected("ax", a[0]);
  setAttrProtected("ay", a[1]);
  setAttrProtected("az", a[2]);
  setAttrProtected("bx", b[0]);
  setAttrProtected("by", b[1]);
  setAttrProtected("bz", b[2]);
  setAttrProtected("cx", c[0]);
  setAttrProtected("cy", c[1]);
  setAttrProtected("cz", c[2]);
}

void TriangleGenerator::generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint,
					 SurfaceQuadtreeNode *qn) {
  Vector3 d = b.parameter - a.parameter;
  midpoint.parameter = a.parameter + d/2;
  midpoint.vertex = midpoint.parameter;
  midpoint.normal = normal;
}

void TriangleGenerator::generateBoundingSphere(Vector3 &center, float &radius) {
  /* Set the center at the triangle's centroid, and set
   * the radius large enough to enclose the triangle's vertices.
   */
  center = (a+b+c) / 3;
  float r1 = (a - center).length();
  float r2 = (b - center).length();
  float r3 = (c - center).length();
  radius = 0;
  if (r1 > radius) radius = r1;
  if (r2 > radius) radius = r2;
  if (r3 > radius) radius = r3;
  radius += EPSILON;
}

void TriangleGenerator::generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
					 VertexBuffer<SurfacePoint> &vbuffer) {
  SurfacePoint sa(a, normal, a);
  SurfacePoint sb(b, normal, b);
  SurfacePoint sc(c, normal, c);
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, sa, sb, sc ));
}

/* The End */
