/*
 * ViewingFrustum.cpp - Culling and projection based on the current OpenGL context
 *
 *     References:
 *        http://www.markmorley.com/opengl/frustumculling.html
 *        http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf
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

#include <GL/gl.h>
#include <math.h>
#include "ViewingFrustum.h"

ViewingFrustum *ViewingFrustum::instance = NULL;

ViewingFrustum::ViewingFrustum(void) {
  invalidate();
}

void ViewingFrustum::invalidate(void) {
  dirty = true;
}

void ViewingFrustum::calculate(void) {
  if (!dirty)
    return;

  /* Snag the OpenGL matrices we're interested in */
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview.vec);
  glGetFloatv(GL_PROJECTION_MATRIX, projection.vec);
  projectionTimesModelview = projection * modelview;

  /* Might as well save the viewport too */
  glGetIntegerv(GL_VIEWPORT, viewport);

  /* Calculate the frustum planes */

  /* Right */
  frustum[0].set(projectionTimesModelview[ 3] - projectionTimesModelview[ 0],
		 projectionTimesModelview[ 7] - projectionTimesModelview[ 4],
		 projectionTimesModelview[11] - projectionTimesModelview[ 8],
		 projectionTimesModelview[15] - projectionTimesModelview[12]);

  /* Left */
  frustum[1].set(projectionTimesModelview[ 3] + projectionTimesModelview[ 0],
		 projectionTimesModelview[ 7] + projectionTimesModelview[ 4],
		 projectionTimesModelview[11] + projectionTimesModelview[ 8],
		 projectionTimesModelview[15] + projectionTimesModelview[12]);
  
  /* Bottom */
  frustum[2].set(projectionTimesModelview[ 3] + projectionTimesModelview[ 1],
		  projectionTimesModelview[ 7] + projectionTimesModelview[ 5],
		  projectionTimesModelview[11] + projectionTimesModelview[ 9],
		  projectionTimesModelview[15] + projectionTimesModelview[13]);

  /* Top */
  frustum[3].set(projectionTimesModelview[ 3] - projectionTimesModelview[ 1],
		 projectionTimesModelview[ 7] - projectionTimesModelview[ 5],
		 projectionTimesModelview[11] - projectionTimesModelview[ 9],
		 projectionTimesModelview[15] - projectionTimesModelview[13]);

  /* Far */
  frustum[4].set(projectionTimesModelview[ 3] - projectionTimesModelview[ 2],
		 projectionTimesModelview[ 7] - projectionTimesModelview[ 6],
		 projectionTimesModelview[11] - projectionTimesModelview[10],
		 projectionTimesModelview[15] - projectionTimesModelview[14]);

  /* Near */
  frustum[5].set(projectionTimesModelview[ 3] + projectionTimesModelview[ 2],
		 projectionTimesModelview[ 7] + projectionTimesModelview[ 6],
		 projectionTimesModelview[11] + projectionTimesModelview[10],
		 projectionTimesModelview[15] + projectionTimesModelview[14]);

  /* Normalize planes */
  for (int i=0; i<6; i++) {
    float w = sqrt(frustum[i][0] * frustum[i][0] + 
		   frustum[i][1] * frustum[i][1] + 
		   frustum[i][2] * frustum[i][2]);
    frustum[i][0] /= w;
    frustum[i][1] /= w;
    frustum[i][2] /= w;
    frustum[i][3] /= w;
  }

  dirty = false;
}

ViewingFrustum *ViewingFrustum::getInstance(void) {
  if (!instance)
    instance = new ViewingFrustum();
  return instance;
}

const Matrix4x4 &ViewingFrustum::getModelview(void) {
  calculate();
  return modelview;
}

const Matrix4x4 &ViewingFrustum::getProjection(void) {
  calculate();
  return projection;
}

const Matrix4x4 &ViewingFrustum::getProjectionTimesModelview(void) {
  calculate();
  return projectionTimesModelview;
}

const int* ViewingFrustum::getViewport(void) {
  calculate();
  return viewport;
}

Vector2 ViewingFrustum::pixelProject(const Vector3 &v) {
  calculate();
  Vector2 n = projection.project(v);
  n[0] = ( n[0]*0.5+0.5) * viewport[2];
  n[1] = (-n[1]*0.5+0.5) * viewport[3];  
  return n;
}

ViewingFrustum::frustumBitfield ViewingFrustum::testPoint(const Vector3 &a) {
  return testSphere(a,0);
}

float ViewingFrustum::pointFrustumDistance(int plane, const Vector3 &v) {
  calculate();
  return frustum[plane][0] * v[0] + frustum[plane][1] * v[1] + frustum[plane][2] * v[2] + frustum[plane][3];
}

ViewingFrustum::frustumBitfield ViewingFrustum::testSphere(const Vector3 &a, const float radius) {
  int bits = 0;

  /* Test the point against each frustum plane, and encode it in a bitfield */
  for (int i=5;i>=0;i--) {
    bits <<= 1;
    bits |= pointFrustumDistance(i,a) < -radius;
  }
    
  return (frustumBitfield) bits;
}

ViewingFrustum::intersectionCode ViewingFrustum::cullPoint(const Vector3 &a) {
  return cullSphere(a,0);
}

ViewingFrustum::intersectionCode ViewingFrustum::cullSphere(const Vector3 &a, const float radius) {
  bool isFullyInside = true;
  
  for (int p=0;p<6;p++) {
    float d = pointFrustumDistance(p, a);
    if (d < -radius)
      return fullyOutside;
    if (d <= radius)
      isFullyInside = false;
  }
    
  if (isFullyInside)
    return fullyInside;
  return overlappingBorder;
}

/* The End */
