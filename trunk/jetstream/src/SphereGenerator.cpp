/*
 * SphereGenerator.cpp - SurfaceGenerator implementations for making spheres
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

#include "SphereGenerator.h"

SphereGenerator::SphereGenerator(JetCOW *cow, Sint32 id, const char *type) : SurfaceGenerator(cow,id,type) {
  if (id<0) {
    /* Defaults */
    setAttr("Radius", 1.0f);
  }
}

void SphereGenerator::loadCachedValues(void) {
  radius = getAttrFloatProtected("Radius");
  dirty = true;
}

void SphereGenerator::saveCachedValues(void) {
  setAttrProtected("Radius", radius);
}

void SphereGenerator::generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint,
				       SurfaceQuadtreeNode *qn) {
  /* Parametric function for a sphere, where the parameter is a point on a unit sphere.
   * To do a spherical interpolation, we can do a linear midpoint then
   * normalize the resulting vector so it's on the surface of our sphere.
   * The surface normal always equals the normalized vertex position.
   */
  Vector3 d = b.parameter - a.parameter;
  midpoint.parameter = a.parameter + d/2;
  midpoint.parameter.normalize();
  midpoint.normal = midpoint.parameter;
  midpoint.vertex = midpoint.parameter * radius;
}

void SphereGenerator::generateBoundingSphere(Vector3 &center, float &radius) {
  center.set(0,0,0);
  radius = this->radius + EPSILON;
}

void SphereGenerator::generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
				       VertexBuffer<SurfacePoint> &vbuffer) {
  /* Icosahedron seed mesh */
  
  float t = 0.8506508084;
  float n = 0.5257311121;
  float tr = t * radius;
  float nr = n * radius;
  
  SurfacePoint za(Vector3( tr, nr,  0),Vector3( t, n, 0),Vector3( t, n, 0));
  SurfacePoint zb(Vector3(-tr, nr,  0),Vector3(-t, n, 0),Vector3(-t, n, 0));
  SurfacePoint zc(Vector3(-tr,-nr,  0),Vector3(-t,-n, 0),Vector3(-t,-n, 0));
  SurfacePoint zd(Vector3( tr,-nr,  0),Vector3( t,-n, 0),Vector3( t,-n, 0));
  SurfacePoint ya(Vector3( nr,  0, tr),Vector3( n, 0, t),Vector3( n, 0, t));
  SurfacePoint yb(Vector3( nr,  0,-tr),Vector3( n, 0,-t),Vector3( n, 0,-t));
  SurfacePoint yc(Vector3(-nr,  0,-tr),Vector3(-n, 0,-t),Vector3(-n, 0,-t));
  SurfacePoint yd(Vector3(-nr,  0, tr),Vector3(-n, 0, t),Vector3(-n, 0, t));
  SurfacePoint xa(Vector3(  0, tr, nr),Vector3( 0, t, n),Vector3( 0, t, n));
  SurfacePoint xb(Vector3(  0,-tr, nr),Vector3( 0,-t, n),Vector3( 0,-t, n));
  SurfacePoint xc(Vector3(  0,-tr,-nr),Vector3( 0,-t,-n),Vector3( 0,-t,-n));
  SurfacePoint xd(Vector3(  0, tr,-nr),Vector3( 0, t,-n),Vector3( 0, t,-n));

  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yd, xa, ya ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xb, yd, ya ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xd, yc, yb ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yc, xc, yb ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zd, ya, za ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yb, zd, za ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zb, yd, zc ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yc, zb, zc ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xd, za, xa ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zb, xd, xa ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zd, xc, xb ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xc, zc, xb ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, za, ya, xa ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yb, za, xd ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zd, xb, ya ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xc, zd, yb ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zb, xa, yd ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xd, zb, yc ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xb, zc, yd ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zc, xc, yc ));
}

/* Compute the bounding prism around a triangle stretched between
 * three points on the surface of our sphere.
 */
void SphereGenerator::updateBounds(SurfaceQuadtreeNode *n, Surface *s) {
  /* Since the triangle will always be contained within the sphere,
   * the surface will never be behind the triangle.
   */
  n->bounds.back = 0.0;

  /* The maximum distance between our triangle and the sphere surface
   * is obtained at the point on the triangle that's closest to the
   * center of the sphere. The front bound is the sphere radius minus
   * the point-plane distance between the center of the sphere and the triangle.
   * Note that the center of the sphere is the origin in this coordinate system.
   */

  Vector3 &va = s->vbuffer[n->vertices[SurfaceQuadtreeNode::a]].parameter;
  Vector3 &vb = s->vbuffer[n->vertices[SurfaceQuadtreeNode::b]].parameter;
  Vector3 &vc = s->vbuffer[n->vertices[SurfaceQuadtreeNode::c]].parameter;
  Vector3 parameterPlaneNormal = (vc - va) % (vb - va);
  parameterPlaneNormal.normalize();
  n->bounds.front = radius * (1 - va * parameterPlaneNormal);
}

/* The End */
