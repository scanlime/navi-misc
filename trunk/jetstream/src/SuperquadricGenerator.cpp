/*
 * SuperquadricGenerator.cpp - SurfaceGenerator implementations for making superquadrics
 *
 *     References:
 *        http://www.gamedev.net/reference/articles/article1172.asp
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

#include "SuperquadricGenerator.h"
#include "Util.h"
#include <math.h>


SuperellipsoidGenerator::SuperellipsoidGenerator(JetCOW *cow, Sint32 id, const char *type) : 
  SurfaceGenerator(cow,id,type) {
  if (id<0) {
    setAttr("Radius", 1.0f);
    setAttr("xe", 1.0f);
    setAttr("ye", 1.0f);
    setAttr("ze", 1.0f);
  }
}

void SuperellipsoidGenerator::loadCachedValues(void) {
  radius = getAttrFloatProtected("Radius");
  xe = getAttrFloatProtected("xe");
  ye = getAttrFloatProtected("ye");
  ze = getAttrFloatProtected("ze");
}

void SuperellipsoidGenerator::saveCachedValues(void) {
  setAttrProtected("Radius", radius);
  setAttrProtected("xe", xe);
  setAttrProtected("ye", ye);
  setAttrProtected("ze", ze);
}

void SuperellipsoidGenerator::generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint,
					       SurfaceQuadtreeNode *qn) {
  /* Start out with a sphere */
  Vector3 d = b.parameter - a.parameter;
  midpoint.parameter = a.parameter + d/2;
  midpoint.parameter.normalize();

  /* Apply the power function */
  midpoint.vertex.set( abspow(midpoint.parameter[0],xe) * radius,
		       abspow(midpoint.parameter[1],ye) * radius,
		       abspow(midpoint.parameter[2],ze) * radius );
  midpoint.normal.set( abspow(midpoint.parameter[0],2-xe),
		       abspow(midpoint.parameter[1],2-ye),
		       abspow(midpoint.parameter[2],2-ze) );
  midpoint.normal.normalize();
}

void SuperellipsoidGenerator::generateBoundingSphere(Vector3 &center, float &radius) {
  center.set(0,0,0);
  radius = this->radius + EPSILON;
}

void SuperellipsoidGenerator::generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
					       VertexBuffer<SurfacePoint> &vbuffer) {
  /* Octahedron seed mesh */  
  SurfacePoint  xPlus(Vector3( radius,      0,      0),Vector3( 1, 0, 0),Vector3( 1, 0, 0));
  SurfacePoint xMinus(Vector3(-radius,      0,      0),Vector3(-1, 0, 0),Vector3(-1, 0, 0));
  SurfacePoint  yPlus(Vector3(      0, radius,      0),Vector3( 0, 1, 0),Vector3( 0, 1, 0));
  SurfacePoint yMinus(Vector3(      0,-radius,      0),Vector3( 0,-1, 0),Vector3( 0,-1, 0));
  SurfacePoint  zPlus(Vector3(      0,      0, radius),Vector3( 0, 0, 1),Vector3( 0, 0, 1));
  SurfacePoint zMinus(Vector3(      0,      0,-radius),Vector3( 0, 0,-1),Vector3( 0, 0,-1));

  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xMinus, zMinus, yPlus  ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xMinus, yPlus , zPlus  ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zMinus, xPlus , yPlus  ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, xPlus , zPlus , yPlus  ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yMinus, zMinus, xMinus ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, zPlus , yMinus, xMinus ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yMinus, xPlus , zMinus ));
  mesh.push_back(SurfaceQuadtreeNode( vbuffer, yMinus, zPlus , xPlus  ));
}

float SuperellipsoidGenerator::abspow(float x, float e) {
  if (x == 0)
    return 0;
  float y = pow(fabsf(x),e);
  if (x<0)
    y = -y;
  return y;
}

void SuperellipsoidGenerator::updateBounds(SurfaceQuadtreeNode *n, Surface *s) {
  Vector3 p(abspow(n->centroid[0], xe),
	    abspow(n->centroid[1], ye),
	    abspow(n->centroid[2], ze));
	    
  n->bounds.back = 0.0;
  n->bounds.front = radius - p.length();

  /* FIXME: The above formula is broken, this sets a fixed level of detail */
  if (n->depth > 3)
    n->bounds.front = 0;
}


/* The End */
