/*
 * SurfaceGenerator.h - Base classes for the models used to build Surfaces
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

/* Push and pop opengl settings for drawing */
void SurfaceGenerator::push(void) {}
void SurfaceGenerator::pop(void) {}

void SurfaceGenerator::animate(float dt) {}

SurfaceGenerator::SurfaceGenerator(JetCOW *cow, Sint32 id,  const char *type) 
  : JetCOWDictionary(cow,id,type) {
  dirty = false;
}

/* Linear interpolation by default */
void SurfaceGenerator::generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint,
					SurfaceQuadtreeNode *qn) {
  midpoint.interpolate(a,b,0.5);
}

void SurfaceGenerator::updateBounds(SurfaceQuadtreeNode *n, Surface *s) {
  n->bounds.front = 0.0;
  n->bounds.back  = 0.0;
}

/* Test the dirty flag of this and the parent, and unset it if it's set */
bool SurfaceGenerator::testDirty(void) {
  bool d = dirty;
  dirty = false;
  return d;
}

/* Push and pop opengl settings for drawing */
void SurfaceModifier::push(void) {
  parent->push();
}

void SurfaceModifier::pop(void) {
  parent->pop();
}

void SurfaceModifier::generateBoundingSphere(Vector3 &center, float &radius) {
  parent->generateBoundingSphere(center,radius);
}

void SurfaceModifier::generateSeedMesh(std::vector<SurfaceQuadtreeNode> &mesh,
				       VertexBuffer<SurfacePoint> &vbuffer) {
  std::vector<SurfaceQuadtreeNode>::iterator i;
  std::vector<SurfaceQuadtreeNode> newNodes;

  /* We have to add the new nodes to a separate list so we can apply f() only to
   * the new nodes. This will become important if multiple modified seed meshes
   * are used in the same surface.
   */
  parent->generateSeedMesh(newNodes, vbuffer);
  
  for (i=newNodes.begin();i!=newNodes.end();i++) {
    for (int v=SurfaceQuadtreeNode::a;v<=SurfaceQuadtreeNode::c;v++) {
      SurfacePoint &p = vbuffer[i->vertices[v]];
      f(p, &*i);
      p.morphReset();
    }
    mesh.push_back(*i);
  }
}

void SurfaceModifier::generateMidpoint(SurfacePoint &a, SurfacePoint &b, SurfacePoint &midpoint,
				       SurfaceQuadtreeNode *qn) {
  parent->generateMidpoint(a,b,midpoint,qn);
  f(midpoint, qn);
  midpoint.morphReset();
}

SurfaceModifier::SurfaceModifier(JetCOW *cow, Sint32 id,  const char *type)
  : SurfaceGenerator(cow,id,type) {
  parent = NULL;
  if (id<0) {
    /* Defaults */
    setAttr("Parent",0);
  }
}

void SurfaceModifier::loadCachedValues(void) {
  int parentId = getAttrIntProtected("Parent");
  if (!parent || parent->id != parentId) {
    if (parent)
      parent->unref();
    if (parentId)
      parent = (SurfaceGenerator*) cow->checkout("SurfaceGenerator",parentId);
    else
      parent = NULL;
  }
}

void SurfaceModifier::updateBounds(SurfaceQuadtreeNode *n, Surface *s) {
  parent->updateBounds(n,s);
}

void SurfaceModifier::saveCachedValues(void) {
  setAttrProtected("Parent", parent ? parent->id : 0);
}

SurfaceModifier::~SurfaceModifier() {
  if (parent)
    parent->unref();
}

void SurfaceModifier::f(SurfacePoint &p, SurfaceQuadtreeNode *qn) {}

/* Test the dirty flag of this and the parent, and unset it if it's set */
bool SurfaceModifier::testDirty(void) {
  /* Don't do this in one line, since we don't want short-circuit logic */
  bool a = parent->testDirty();
  bool b = SurfaceGenerator::testDirty();
  return a || b;
}

/* The End */
