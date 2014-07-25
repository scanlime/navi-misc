/*
 * Surface.cpp - A class implementing dynamic real-time procedural
 *               surfaces including continuous level of detail using a generalization
 *               of the Diamond algorithm
 *
 *     References:
 *         The Diamond algorithm - http://www.cs.sun.ac.za/~henri/terrain.html
 *
 *     Optimization to-do list:
 *         - Fully merge the mesh as soon as the surface moves completely outside the frustum
 *         - Hierarchial culling tests (we have a quadtree, might as well use it...)
 *         - Triangle strips?
 *         - Profile, profile, profile...
 *
 *     Known bug list:
 *         - Geomorphing is broke. "spikes" appear at seemingly random places on the surface
 *           where a vertex gets stuck inbetween the actual surface and the seed mesh.
 *         - Morphing for splits happen in one step, whereas merging for morphs happen one
 *           tree level at a time. This may or may not be a problem, but it's icky and I'd like
 *           a way of doing merging that's closer to the nice simple way split is done.
 *         - Bogus pseudoculling test in getPriority can give false negatives
 *
 *     Feature wishlist:
 *         - Automatic tesselation error adjustment according to frame rate (should probably go
 *           in the DetailSettings class)
 *
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

/* Debugging for this file gives you several things:
 *   - Every second, the surface dumps queue lengths and a triangle count
 *   - Lots of assertions are enabled
 *   - Instead of the normal draw() function, a debug draw function is used.
 *     The debug draw separates the individual triangles, draws the neighbor pointers,
 *     and applies color coding.
 *     For edges and neighbor pointers:http://www.cs.washington.edu/homes/klee/misc/slashdot.html
 *        - red    = NULL
 *        - green  = bidirectional, correct side indices
 *        - gray   = bidirectional, incorrect side indices
 *        - yellow = unidirectional
 *     For the triangle's interior:
 *        - clear  = Not mergable
 *        - white  = Mergable and enqueued
 *        - red    = Mergable but not enqueued
 *     Additionally, the front and back extents of the bounding prism
 *     are indicated in blue and black, respectively.
 */

//#define DEBUG_FILE			/* Enable assertions and deguggative spew */
//#define DEBUG_RENDERING		/* Draw debug information instead of regular triangles */
//#define SURFACE_SPLIT_ONLY		/* Reset the mesh each frame */
//#define DEBUG_NORMALS      		/* Show vertex normals */
#define DISABLE_GEOMORPHING		/* Don't geomorph when splitting and merging */


#include "Engine.h"
#include "Util.h"
#include "Matrix.h"
#include "Surface.h"
#include "DetailSettings.h"
#include "ViewingFrustum.h"
#include "SurfaceGenerator.h"
#include <string.h>
#include <SDL.h>
#include <math.h>


/***************************************************************** Utility data types ***/

void SurfacePoint::init() {
  morphReset();
  
  /* Default to white */
  this->color[0] = 1;
  this->color[1] = 1;
  this->color[2] = 1;
}

SurfacePoint::SurfacePoint() {
  init();
}

SurfacePoint::SurfacePoint(Vector3 vertex, Vector3 normal, Vector3 parameter) {
  this->vertex = vertex;
  this->normal = normal;
  this->parameter = parameter;
  init();
}

bool SurfacePoint::operator == (const SurfacePoint &v) const {
  return vertex==v.vertex;
}

bool SurfacePoint::operator != (const SurfacePoint &v) const {
  return vertex!=v.vertex;
}

bool SurfacePoint::operator < (const SurfacePoint &v) const {
  if (vertex[0] < v.vertex[0])
    return true;
  if (vertex[0] != v.vertex[0])
    return false;
  if (vertex[1] < v.vertex[1])
    return true;
  if (vertex[1] != v.vertex[1])
    return false;
  if (vertex[2] < v.vertex[2])
    return true;
  return false;
}

void SurfacePoint::morphReset(void) {
  linearInterpolation = *this;
  surfaceInterpolation = *this;
  morphWeight = 1;
  morphBackwards = false;
}

bool SurfacePoint::morph(float dt) {
  float duration = Engine::getInstance()->detail.geomorphDuration;

  if (duration > 0) {
    morphWeight += dt / duration;
    if (morphWeight > 1)
      morphWeight = 1;    
  }
  else {
    morphWeight = 1;
  }

  if (morphBackwards)
    interpolate(surfaceInterpolation, linearInterpolation, morphWeight);
  else
    interpolate(linearInterpolation, surfaceInterpolation, morphWeight);

  return morphWeight >= 1;
}

SurfacePoint& SurfacePoint::operator = (const BufferedVertex& v) {
  *(BufferedVertex*)this = v;
}

SurfaceEdge::SurfaceEdge(SurfacePoint &a, SurfacePoint &b) {
  if (a<b) {
    this->a = a;
    this->b = b;
  }
  else {
    this->a = b;
    this->b = a;
  }
}

bool SurfaceEdge::operator == (const SurfaceEdge &v) const {
  return a==v.a && b==v.b;
}

bool SurfaceEdge::operator != (const SurfaceEdge &v) const {
  return !(*this == v);
}

bool SurfaceEdge::operator < (const SurfaceEdge &v) const {
  if (a < v.a)
    return true;
  if (a != v.a)
    return false;
  if (b < v.b)
    return true;
  return false;
}


/***************************************************************** Quadtree node ***/

SurfaceQuadtreeNode::SurfaceQuadtreeNode() {
  memset(&vertices, 0, sizeof(vertices));
  memset(&neighbors, 0, sizeof(neighbors));
  memset(&children, 0, sizeof(children));
  depth = 0;
}

SurfaceQuadtreeNode::SurfaceQuadtreeNode(VertexBuffer<SurfacePoint> &vbuffer,
					 SurfacePoint a_, SurfacePoint b_, SurfacePoint c_) {
  vertices[a] = vbuffer.alloc();
  vertices[b] = vbuffer.alloc();
  vertices[c] = vbuffer.alloc();

  /* Note: This will obliterate the reference count from the vertices in the vbuffer
   *       and replace them with the ones from our args, but that's alright since they
   *       should both have been just allocated, they should both be 1.
   */
  assert(vbuffer[vertices[a]].refcount == 1);
  assert(vbuffer[vertices[b]].refcount == 1);
  assert(vbuffer[vertices[c]].refcount == 1);
  assert(a_.refcount == 1);
  assert(b_.refcount == 1);
  assert(c_.refcount == 1);
  vbuffer[vertices[a]] = a_;
  vbuffer[vertices[b]] = b_;
  vbuffer[vertices[c]] = c_;

  memset(&neighbors, 0, sizeof(neighbors));
  memset(&children, 0, sizeof(children));
  list = NULL;
  unsplittable = false;
  culled = false;
  parent = NULL;
  depth = 0;
}

SurfaceQuadtreeNode::SurfaceQuadtreeNode(SurfaceQuadtreeNode *parent_,
					 unsigned int a_, unsigned int b_, unsigned int c_) {
  vertices[a] = a_;
  vertices[b] = b_;
  vertices[c] = c_;
  memset(&neighbors, 0, sizeof(neighbors));
  memset(&children, 0, sizeof(children));
  list = NULL;
  parent = parent_;
  culled = false;
  unsplittable = false;
  bounds.front = 0;
  bounds.back = 0;
  depth = parent->depth+1;
}

/* The two vertices, in order, for each side */
const SurfaceQuadtreeNode::sideInfoStruct SurfaceQuadtreeNode::sideInfo[3] = {
  { a,b },
  { b,c },
  { c,a },
};

void SurfaceQuadtreeNode::draw(Surface *s) {
  unsigned int t[6];

  /* Fix T-vertices on the following sides: */

  if (neighbors[ab].n && !neighbors[ab].n->isLeaf()) {
    if (neighbors[bc].n && !neighbors[bc].n->isLeaf()) {
      if (neighbors[ca].n && !neighbors[ca].n->isLeaf()) {
	/* ab, bc, ca */
	t[0] = neighbors[ca].n->children[sideInfo[neighbors[ca].side].v1]->vertices[sideInfo[neighbors[ca].side].v2];
	t[1] = vertices[a];
	t[2] = neighbors[ab].n->children[sideInfo[neighbors[ab].side].v1]->vertices[sideInfo[neighbors[ab].side].v2];
	t[3] = vertices[b];
	t[4] = neighbors[bc].n->children[sideInfo[neighbors[bc].side].v1]->vertices[sideInfo[neighbors[bc].side].v2];
	t[5] = vertices[c];
	s->vbuffer.draw(GL_TRIANGLE_FAN, t, 6);
      }
      else {
	/* ab, bc */
	t[0] = neighbors[ab].n->children[sideInfo[neighbors[ab].side].v1]->vertices[sideInfo[neighbors[ab].side].v2];
	t[1] = vertices[b];
	t[2] = neighbors[bc].n->children[sideInfo[neighbors[bc].side].v1]->vertices[sideInfo[neighbors[bc].side].v2];
	t[3] = vertices[c];
	t[4] = vertices[a];
	s->vbuffer.draw(GL_TRIANGLE_FAN, t, 5);
      }
    }
    else {
      if (neighbors[ca].n && !neighbors[ca].n->isLeaf()) {
	/* ab, ca */
	t[0] = neighbors[ab].n->children[sideInfo[neighbors[ab].side].v1]->vertices[sideInfo[neighbors[ab].side].v2];
	t[1] = vertices[b];
	t[2] = vertices[c];
	t[3] = neighbors[ca].n->children[sideInfo[neighbors[ca].side].v1]->vertices[sideInfo[neighbors[ca].side].v2];
	t[4] = vertices[a];
	s->vbuffer.draw(GL_TRIANGLE_FAN, t, 5);
      }
      else {
	/* ab */
	t[0] = vertices[c];
	t[1] = vertices[a];
	t[2] = neighbors[ab].n->children[sideInfo[neighbors[ab].side].v1]->vertices[sideInfo[neighbors[ab].side].v2];
	t[3] = vertices[b];
	s->vbuffer.draw(GL_TRIANGLE_FAN, t, 4);
      }
    }
  }
  else {
    if (neighbors[bc].n && !neighbors[bc].n->isLeaf()) {
      if (neighbors[ca].n && !neighbors[ca].n->isLeaf()) {
	/* bc, ca */
	t[0] = neighbors[bc].n->children[sideInfo[neighbors[bc].side].v1]->vertices[sideInfo[neighbors[bc].side].v2];
	t[1] = vertices[c];
	t[2] = neighbors[ca].n->children[sideInfo[neighbors[ca].side].v1]->vertices[sideInfo[neighbors[ca].side].v2];
	t[3] = vertices[a];
	t[4] = vertices[b];
	s->vbuffer.draw(GL_TRIANGLE_FAN, t, 5);
      }
      else {
	/* bc */
	t[0] = vertices[a];
	t[1] = vertices[b];
	t[2] = neighbors[bc].n->children[sideInfo[neighbors[bc].side].v1]->vertices[sideInfo[neighbors[bc].side].v2];
	t[3] = vertices[c];
	s->vbuffer.draw(GL_TRIANGLE_FAN, t, 4);
      }
    }
    else {
      if (neighbors[ca].n && !neighbors[ca].n->isLeaf()) {
	/* ca */
	t[0] = vertices[b];
	t[1] = vertices[c];
	t[2] = neighbors[ca].n->children[sideInfo[neighbors[ca].side].v1]->vertices[sideInfo[neighbors[ca].side].v2];
	t[3] = vertices[a];
	s->vbuffer.draw(GL_TRIANGLE_FAN, t, 4);
      }
      else {
	/* (none) */
	s->vbuffer.draw(GL_TRIANGLES, vertices, 3);
      }
    }
  }

#ifdef DEBUG_NORMALS
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  for (int i=0; i<3; i++) {
    SurfacePoint &sp = s->vbuffer[vertices[i]];
    Vector3 p = sp.vertex;
    Vector3 q = p + sp.normal * 4;
    glColor3f(0,0,1);
    glVertex3f(p[0],p[1],p[2]);
    glVertex3f(q[0],q[1],q[2]);
  }
  glEnd();
  glEnable(GL_LIGHTING);
#endif /* DEBUG_NORMALS */
}

void SurfaceQuadtreeNode::drawDebug(Surface *s) {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);

  if (isMergable()) {
    glBegin(GL_TRIANGLES);
    /* White filling if it's mergable, red filling if it's mergable but not enqueued */
    if (isEnqueued())
      glColor4f(1,1,1,0.5);
    else
      glColor4f(1,0,0,0.5);
    glNormal3f(s->vbuffer[vertices[a]].normal[0],
	       s->vbuffer[vertices[a]].normal[1],
	       s->vbuffer[vertices[a]].normal[2]);
    glVertex3f(s->vbuffer[vertices[a]].vertex[0],
	       s->vbuffer[vertices[a]].vertex[1],
	       s->vbuffer[vertices[a]].vertex[2]);
    glNormal3f(s->vbuffer[vertices[b]].normal[0],
	       s->vbuffer[vertices[b]].normal[1],
	       s->vbuffer[vertices[b]].normal[2]);
    glVertex3f(s->vbuffer[vertices[b]].vertex[0],
	       s->vbuffer[vertices[b]].vertex[1],
	       s->vbuffer[vertices[b]].vertex[2]);
    glNormal3f(s->vbuffer[vertices[c]].normal[0],
	       s->vbuffer[vertices[c]].normal[1],
	       s->vbuffer[vertices[c]].normal[2]);
    glVertex3f(s->vbuffer[vertices[c]].vertex[0],
	       s->vbuffer[vertices[c]].vertex[1],
	       s->vbuffer[vertices[c]].vertex[2]);
    glEnd();
  }


  if (isLeaf()) {
    glBegin(GL_LINES);

    /* Draw bounding prism front and back extents in blue and black, respectively */
    Vector3 extent;
    glColor3f(0,0,1);
    extent = centroid + planeNormal * bounds.front;
    glVertex3f(centroid[0],centroid[1],centroid[2]);
    glVertex3f(extent[0],extent[1],extent[2]);
    glColor3f(0,0,0);
    extent = centroid - planeNormal * bounds.back;
    glVertex3f(centroid[0],centroid[1],centroid[2]);
    glVertex3f(extent[0],extent[1],extent[2]);    

    for (int i=ab; i<=ca; i++) {
      if (neighbors[i].n) {
	if (neighbors[i].n->neighbors[neighbors[i].side].n == this) {
	  if (neighbors[i].n->neighbors[neighbors[i].side].side == i) {
	    /* Green for correct bidirectional pointers */
	    glColor3f(0,1,0);
	  }
	  else {
	    /* Gray for bidirectional pointers with an incorrect 'side' */
	    glColor3f(0.8, 0.8, 0.8);
	  }
	}
	else {
	  /* yellow for unidirectional pointers */
	  glColor3f(1,1,0);
	}
      }
      else {
	/* red for NULL pointers */
	glColor3f(1,0,0);
      }

      Vector3 va = s->vbuffer[vertices[sideInfo[i].v1]].vertex; 
      Vector3 vb = s->vbuffer[vertices[sideInfo[i].v2]].vertex; 
      va = (va-centroid) * 0.9 + centroid;
      vb = (vb-centroid) * 0.9 + centroid;
      
      /* The triangle side itself, offset inward a bit so
       * we can distinguish neighboring triangles from each other.
       */
      glVertex3f(va[0],va[1],va[2]);
      glVertex3f(vb[0],vb[1],vb[2]);

      if (neighbors[i].n) {
	Vector3 abmidpoint = (vb-va) * 0.5 + va;
	
	/* Graphically show the pointer from this triangle to the neighbor */
	glVertex3f(abmidpoint[0],abmidpoint[1],abmidpoint[2]);
	glVertex3f(neighbors[i].n->centroid[0],
		   neighbors[i].n->centroid[1],
		   neighbors[i].n->centroid[2]);
      }
    }			
    glEnd();
  }
  else {
    children[a]->drawDebug(s);
    children[b]->drawDebug(s);
    children[c]->drawDebug(s);
    children[center]->drawDebug(s);
  }
}

void SurfaceQuadtreeNode::split(Surface *s) {
  unsigned int midpoints[3];

  DBG("this=%p, surface=%p\n",this,s);
  assert(isLeaf() == true);

  if (isEnqueued())
    dequeue();

  if (parent && parent->isEnqueued())
    parent->dequeue();

  /* Fixup neighbor pointers... */
  for (int i=ab; i<=ca; i++) {
    if (neighbors[i].n) {
      /* Split neighbors if we need to */
      if (neighbors[i].n->neighbors[neighbors[i].side].n != this)
	neighbors[i].n->split(s);
      
      /* Remove major neighbors from the merge queue */
      if (neighbors[i].n->parent && neighbors[i].n->parent->isEnqueued()) {
	assert(neighbors[i].n->parent->isLeaf() == false);
	neighbors[i].n->parent->dequeue();
      }
    }
  }

  /* Acquire midpoint vertices for each side */
  for (int i=ab; i<=ca; i++) {
    if (neighbors[i].n && neighbors[i].n->children[sideInfo[neighbors[i].side].v1]) {
      /* This midpoint already exists in our neighbor's children, create a reference to it */
      midpoints[i] = neighbors[i].n->children[sideInfo[neighbors[i].side].v1]->vertices[sideInfo[neighbors[i].side].v2];
      s->vbuffer.ref(midpoints[i]);
    }
    else {
      /* Generate a new midpoint */
      midpoints[i] = s->vbuffer.alloc();
      SurfacePoint &p = s->vbuffer[midpoints[i]];

      SurfacePoint current1 = s->vbuffer[vertices[sideInfo[i].v1]];
      SurfacePoint current2 = s->vbuffer[vertices[sideInfo[i].v2]];

      /* This should be using 'parameter' rather than the vertex's position, so we
       * don't want to give it surfaceInterpolation.
       */
      s->generator->generateMidpoint(current1, current2, p, this);

      if (s->enableGeomorphing && !culled) {
	/* We use geomorphing to cover up the appearance of new vertices.
	 * Move the vertex we just generated into a geomorphing target we will gradually
	 * interpolate to, and set the vertex's current position to an approximation of
	 * what it's being rendered as right now- a linear interpolation between the two vertices
	 * we just calculated a midpoint for.
	 */
	p.surfaceInterpolation = p;
	p.interpolate(current1, current2, 0.5);
	p.linearInterpolation = p;
	p.morphWeight = 0.0;
	p.morphBackwards = false;
      }
      else {
	p.morphReset();
      }
	
      /* This reference is owned by the splitMorphVertices list */
      s->vbuffer.ref(midpoints[i]);
      s->splitMorphVertices.push_back(midpoints[i]);
    }

    /* Each midpoint will need a total of three new references to cover the triangles we generate
     * below. We just got one reference above, add two more.
     */
    s->vbuffer.ref(midpoints[i]);
    s->vbuffer.ref(midpoints[i]);
  }      

  /* Add one new references for each of our original vertices */
  s->vbuffer.ref(vertices[a]);
  s->vbuffer.ref(vertices[b]);
  s->vbuffer.ref(vertices[c]);
  
  /* Generate four child triangles */
  children[a]      = new SurfaceQuadtreeNode(this,   vertices[a], midpoints[ab], midpoints[ca]);
  children[b]      = new SurfaceQuadtreeNode(this, midpoints[ab],   vertices[b], midpoints[bc]);
  children[c]      = new SurfaceQuadtreeNode(this, midpoints[ca], midpoints[bc],   vertices[c]);
  children[center] = new SurfaceQuadtreeNode(this, midpoints[ab], midpoints[bc], midpoints[ca]);

  /* Assign all neighbor pointers.
   * See extra_media/quadtree-naming.png and
   * extra_media/triangle-mesh.png to make this a little more clear.
   *
   * External neighbor links on each side:
   */
  for (int side=ab;side<=ca;side++) {
    if (!neighbors[side].n)
      continue;

    if (neighbors[side].n->isLeaf()) {
      children[sideInfo[side].v1]->neighbors[side].n = neighbors[side].n;
      children[sideInfo[side].v2]->neighbors[side].n = neighbors[side].n;
    }
    else {
      children[sideInfo[side].v1]->neighbors[side].n = neighbors[side].n->children[sideInfo[neighbors[side].side].v2];
      children[sideInfo[side].v2]->neighbors[side].n = neighbors[side].n->children[sideInfo[neighbors[side].side].v1];
      neighbors[side].n->children[sideInfo[neighbors[side].side].v2]->neighbors[neighbors[side].side].n = children[sideInfo[side].v1];
      neighbors[side].n->children[sideInfo[neighbors[side].side].v1]->neighbors[neighbors[side].side].n = children[sideInfo[side].v2];
      neighbors[side].n->children[sideInfo[neighbors[side].side].v2]->neighbors[neighbors[side].side].side = (sideNames) side;
      neighbors[side].n->children[sideInfo[neighbors[side].side].v1]->neighbors[neighbors[side].side].side = (sideNames) side;
    }
    children[sideInfo[side].v1]->neighbors[side].side = neighbors[side].side;
    children[sideInfo[side].v2]->neighbors[side].side = neighbors[side].side;
  }
  
  /* Internal neighbor links: */
  children[a]->neighbors[bc].n = children[center];
  children[b]->neighbors[ca].n = children[center];
  children[c]->neighbors[ab].n = children[center];
  children[a]->neighbors[bc].side = ca;
  children[b]->neighbors[ca].side = ab;
  children[c]->neighbors[ab].side = bc;
  children[center]->neighbors[ab].n = children[b];
  children[center]->neighbors[bc].n = children[c];
  children[center]->neighbors[ca].n = children[a];
  children[center]->neighbors[ab].side = ca;
  children[center]->neighbors[bc].side = ab;
  children[center]->neighbors[ca].side = bc;

  /* This triangle no longer renders, but we have four new ones */
  s->numTriangles += 3;

  for (int i=a; i<=center; i++)
    children[i]->updateGeometry(s);

  /* Prioritize and enqueue */
  children[a]->splitEnqueue(s);
  children[b]->splitEnqueue(s);
  children[c]->splitEnqueue(s);
  children[center]->splitEnqueue(s);

  if (isMergable())
    enqueue(s->MergeAbove);
}
 
/* Update the node's normal and bounding prism.
 * This should be called any time the surfaceInterpolation vertices change.
 */
void SurfaceQuadtreeNode::updateGeometry(Surface *s) {
  assert(vertices[a] <= s->vbuffer.allocatedSize());
  assert(vertices[b] <= s->vbuffer.allocatedSize());
  assert(vertices[c] <= s->vbuffer.allocatedSize());

  /* Use the interpolated vertices that actually lie on the surface */
  Vector3 &ca = s->vbuffer[vertices[a]].surfaceInterpolation.vertex;
  Vector3 &cb = s->vbuffer[vertices[b]].surfaceInterpolation.vertex;
  Vector3 &cc = s->vbuffer[vertices[c]].surfaceInterpolation.vertex;

  /* Update plane normal */
  Vector3 u = cc - ca;
  Vector3 v = cb - ca;
  planeNormal = u % v;
  assert(planeNormal.length2() > 0);
  planeNormal.normalize();
  
  /* Calculate centroid */
  centroid = (ca + cb + cc) / 3; 

  /* Update bounding prism */
  s->generator->updateBounds(this, s);

  /* If we're approaching the limit of our floating point precision, stop splitting! */
  if (u.length2() <= EPSILON*EPSILON*2 ||
      v.length2() <= EPSILON*EPSILON*2)
    unsplittable = true;

  /* Calculate the radius of the bounding sphere centered at the centroid */
  Vector3 frontNormal = planeNormal *   bounds.front;
  Vector3 backNormal =  planeNormal * (-bounds.back);
  float r1 = (ca + frontNormal - centroid).length();
  float r2 = (cb + frontNormal - centroid).length();
  float r3 = (cc + frontNormal - centroid).length();
  float r4 = (ca + backNormal  - centroid).length();
  float r5 = (cb + backNormal  - centroid).length();
  float r6 = (cc + backNormal  - centroid).length();
  cullRadius = 0;
  if (r1 > cullRadius) cullRadius = r1;
  if (r2 > cullRadius) cullRadius = r2;
  if (r3 > cullRadius) cullRadius = r3;
  if (r4 > cullRadius) cullRadius = r4;
  if (r5 > cullRadius) cullRadius = r5;
  if (r6 > cullRadius) cullRadius = r6;
}

void SurfaceQuadtreeNode::mergeTopHalf(Surface *s) {
  DBG("this=%p, surface=%p\n",this,s);

  assert(isLeaf() == false);
  assert(isMergable() == true);

  /* Set up geomorphing for the three midpoint vertices */
  for (int i=ab; i<=ca; i++) {
    SurfacePoint &v = s->vbuffer[children[sideInfo[i].v1]->vertices[sideInfo[i].v2]];

    if (s->enableGeomorphing && !culled) {
      if (!v.morphBackwards) {
	v.morphWeight = 0;
	v.morphBackwards = true;
      }
    }
    else {
      v.morphReset();
    }
  }
}

void SurfaceQuadtreeNode::mergeBottomHalf(Surface *s) {
  DBG("this=%p, surface=%p\n",this,s);

  /* Dequeue children if they're enqueued 
   * NOTE: Since the split queue determines triangle rendering,
   *       if we do this in the TopHalf the triangles will be invisible during merge!
   */
  for (int i=a; i<=center; i++)
    if (children[i]->isEnqueued())
      children[i]->dequeue();

  if (isEnqueued())
    dequeue();
  assert(isEnqueued() == false);

  /* Fixup neighbor pointers */
  for (int side=ab;side<=ca;side++) {
    if (neighbors[side].n) {
      if (neighbors[side].n->children[center]) {
	neighbors[side].n->children[sideInfo[neighbors[side].side].v1]->neighbors[neighbors[side].side].n = this;
	neighbors[side].n->children[sideInfo[neighbors[side].side].v2]->neighbors[neighbors[side].side].n = this;
      }

      /* Enqueue neighbor parent for merging if it can merge */
      if (neighbors[side].n->parent && 
	  neighbors[side].n->parent != parent &&
	  neighbors[side].n->parent->isMergable())
	neighbors[side].n->parent->mergeEnqueue(s);

      /* Enqueue neighbor for merging if it can merge 
       * Note: This wasn't present in the original Diamond implementation, but
       *       seems to be necessary to keep mergable nodes from getting 'stuck'
       *       outside the merge queue (red-tinted triangles in the debug mode).
       */
      if (neighbors[side].n->isMergable() && !neighbors[side].n->isEnqueued())
	neighbors[side].n->mergeEnqueue(s);
    }
  }

  /* Delete children */
  for (int i=a;i<=center;i++) {
    s->delTree(*children[i]);
    delete children[i];
    children[i] = NULL;
  }

  s->numTriangles -= 3;

  if (parent && parent->isMergable())
    parent->mergeEnqueue(s);

  enqueue(s->SplitBelow);
}

void SurfaceQuadtreeNode::splitEnqueue(Surface *s) {
  assert(isLeaf() == true);
  assert(isMergable() == false);
  if (getPriority(s) > 0)
    enqueue(s->SplitAbove);
  else
    enqueue(s->SplitBelow);
}

void SurfaceQuadtreeNode::mergeEnqueue(Surface *s) {
  assert(isLeaf() == false);
  assert(isMergable() == true);
  if (getPriority(s) < 0)
    enqueueMergeBelow(s);
  else
    enqueue(s->MergeAbove);
}

void SurfaceQuadtreeNode::enqueue(std::list<SurfaceQuadtreeNode*> &l) {
  //  DBG("this=%p in list %p\n",this,&l);

  assert(isEnqueued() == false);
  l.push_front(this);
  iter = l.begin();
  list = &l;
}

void SurfaceQuadtreeNode::dequeue(void) {
  //  DBG("this=%p from list %p\n",this,list);
  assert(isEnqueued() == true);
  list->erase(iter);
  list = NULL;
}

void SurfaceQuadtreeNode::enqueueMergeBelow(Surface *s) {
  enqueue(s->MergeBelow);
  mergeTopHalf(s);
}

bool SurfaceQuadtreeNode::isLeaf() {
  bool l = (children[center] == NULL);
  if (l) {
    assert(children[a] == NULL);
    assert(children[b] == NULL);
    assert(children[c] == NULL);
  }
  else {
    assert(children[a] != NULL);
    assert(children[b] != NULL);
    assert(children[c] != NULL);
  }
  return l;
}

bool SurfaceQuadtreeNode::isEnqueued() {
  if (list) {
    assert(*iter == this);
  }
  return list != NULL;
}

bool SurfaceQuadtreeNode::isMergable() {
  /* If we don't have exactly one layer of children, can't merge */
  if (!children[center] || 
       children[center]->children[center] ||
       children[a]->children[center] ||
       children[b]->children[center] ||
       children[c]->children[center] )
    return false;

  /* If our immediate neighbors have more than one layer of children, don't merge */
  for (int i=ab; i<=ca; i++) {
    if (neighbors[i].n &&
	neighbors[i].n->children[center] &&
	(neighbors[i].n->children[sideInfo[neighbors[i].side].v1]->children[center] ||
	 neighbors[i].n->children[sideInfo[neighbors[i].side].v2]->children[center]))
      return false;
  }

  return true;
}

void SurfaceQuadtreeNode::cull(Surface *s) {
  ViewingFrustum *frustum = ViewingFrustum::getInstance();

  switch (s->boundingSphere.frustumCode) {

    /* Entire surface is outside the frustum */
  case ViewingFrustum::fullyOutside:
    culled = true;
    break;

    /* Entire surface is inside the frustum */
  case ViewingFrustum::fullyInside:
    culled = false;
    break;

    /* Surface is partially inside the frustum, do a bounding sphere test on this triangle */
  default:
    switch (frustum->cullSphere(centroid, cullRadius)) {

      /* Triangle is fully outside */
    case ViewingFrustum::fullyOutside:
      culled = true;
      break;
      
      /* Triangle is fully inside */
    case ViewingFrustum::fullyInside:
      culled = false;
      break;

      /* Bounding sphere is partially inside, check the bounding prism points.
       * This test still gives false positives, but far fewer of them
       * than the bounding sphere test.
       */
    default: {
      Vector3 &va = s->vbuffer[vertices[a]].surfaceInterpolation.vertex;
      Vector3 &vb = s->vbuffer[vertices[b]].surfaceInterpolation.vertex;
      Vector3 &vc = s->vbuffer[vertices[c]].surfaceInterpolation.vertex;

      Vector3 front = planeNormal * bounds.front;
      Vector3 back  = planeNormal * (-bounds.back);

      culled = false;

      ViewingFrustum::frustumBitfield fa = frustum->testPoint(va + front);
      if (fa == ViewingFrustum::behindNone) break;
      ViewingFrustum::frustumBitfield fb = frustum->testPoint(vb + front);
      if (fa != fb) break;
      ViewingFrustum::frustumBitfield fc = frustum->testPoint(vc + front);
      if (fa != fc) break;
      ViewingFrustum::frustumBitfield ba = frustum->testPoint(va + back);
      if (fa != ba) break;
      ViewingFrustum::frustumBitfield bb = frustum->testPoint(vb + back);
      if (fa != bb) break;
      ViewingFrustum::frustumBitfield bc = frustum->testPoint(vc + back);
      if (fa != bc) break;
							    
      culled = true;
    }
    }
  }
}

float SurfaceQuadtreeNode::getPriority(Surface *s) {
  Vector3 *s1, *s2;
  float d;

  /* Update culling, culled triangles should always merge if they can */
  cull(s);
  if (culled || unsplittable)
    return -1;

  /* The goal of this priority function is to limit the apparent error in the
   * final rendering. This method computes the thickness of the triangle's
   * bounding prism after projecting it onto the screen, and compares that to
   * error margins set in pixels.
   *
   * Note: right now we do all this with the current frustum, but we could
   *       optimize this surface for multiple simultaneous views by doing this
   *       calculation for several viewing frustums then taking the largest
   *       resulting errorSquared value.
   */

  ViewingFrustum *frustum = ViewingFrustum::getInstance();
  const int *viewport = frustum->getViewport();
  Vector3 front = frustum->getModelview() * (centroid + planeNormal * bounds.front);
  Vector3 back  = frustum->getModelview() * (centroid - planeNormal * bounds.back);

  Vector2 pfront = frustum->pixelProject(front);
  Vector2 pback  = frustum->pixelProject(back);

  /* FIXME: kludge to work around infinite subdivision bug */
  if (depth > 6)
    return -1;

  float errorSquared = (pfront - pback).length2();
  if (errorSquared < Engine::getInstance()->detail.minTesselationErrorSquared)
    return -1;
  if (errorSquared > Engine::getInstance()->detail.maxTesselationErrorSquared)
    return 1;
  return 0;
}


/***************************************************************** Surface class ***/

void Surface::reset() {
  /* Generate a fresh seed mesh */
  for (std::vector<SurfaceQuadtreeNode>::iterator i=trees.begin(); i!=trees.end(); i++)
    delTree(*i);
  trees.clear();
  generator->generateSeedMesh(trees, vbuffer);
  prepareMesh();
  generator->generateBoundingSphere(boundingSphere.center, boundingSphere.radius);
  numTriangles = trees.size();
  seedInvalidated = false;

  /* Clear the diamond stacks */
  SplitAbove.clear();
  MergeAbove.clear();
  SplitBelow.clear();
  MergeBelow.clear();

  /* Prioritize and enqueue the seed mesh */
  std::vector<SurfaceQuadtreeNode>::iterator i;
  for (i=trees.begin(); i!=trees.end(); i++) {
    if (i->getPriority(this) > 0)
      i->enqueue(SplitAbove);
    else
      i->enqueue(SplitBelow);
  }
}

void Surface::reprioritize() {
  std::list<SurfaceQuadtreeNode*>::iterator i;
  SurfaceQuadtreeNode *j;

  /* Pass through SplitBelow, requeue in SplitAbove if necessary */
  for (i=SplitBelow.begin(); i!=SplitBelow.end();) {
    if ((*i)->getPriority(this) > 0) {
      j = *i;
      i++;
      j->dequeue();
      j->enqueue(SplitAbove);
    }
    else
      i++;
  }

  /* Pass through MergeAbove, requeue in MergeBelow if necessary */
  for (i=MergeAbove.begin(); i!=MergeAbove.end();) {
    if ((*i)->getPriority(this) < 0) {
      j = *i;
      i++;
      j->dequeue();
      j->enqueueMergeBelow(this);
    }
    else
      i++;
  }
}

void Surface::draw() {
#ifdef DEBUG_RENDERING
  /* Draw each quadtree recursively, in debug mode */
  std::vector<SurfaceQuadtreeNode>::iterator i;
  for (i=trees.begin();i!=trees.end();i++)
    i->drawDebug(this);
#else
  /* Draw the nodes in SplitBelow */
  std::list<SurfaceQuadtreeNode*>::iterator i;
  vbuffer.bind();
  generator->push();
  for (i=SplitBelow.begin();i!=SplitBelow.end();i++)
    if (!(*i)->culled)
      (*i)->draw(this);
  generator->pop();
#endif

#ifdef DEBUG_FILE
  static long t = SDL_GetTicks();
  if (SDL_GetTicks() > t+1000) {
    t = SDL_GetTicks();
    DBG("-- Surface stats --\n"
	"  Triangles: %d\n"
	" SplitAbove: %d\n"
	" MergeAbove: %d\n"
	" SplitBelow: %d\n"
	" MergeBelow: %d\n"
	"    vbuffer: %.2f%% utilization (%d used / %d allocated)\n"
	"\n", numTriangles, 
	SplitAbove.size(), MergeAbove.size(), 
	SplitBelow.size(), MergeBelow.size(),
	vbuffer.utilization(), vbuffer.size(), vbuffer.allocatedSize());
  }
#endif
}

void Surface::animate(float dt) {
  if (!generator)
    return;

  generator->animate(dt);

  if (Engine::getInstance()->frustumLock)
    return;

  /* Sanity-check our minimum tesselation error- if it's zero, we'll get
   * into an almost-infinite loop splitting triangles.
   * (It will stop once it hits the floating point precision limit, but by
   *  then we'll probably be out of memory anyway)
   */
  if (Engine::getInstance()->detail.minTesselationErrorSquared < EPSILON)
    Engine::getInstance()->detail.minTesselationErrorSquared = EPSILON;

#ifdef SURFACE_SPLIT_ONLY
  seedInvalidated = true;
#endif

  /* Order here is important, we don't want the testDirty to short-circuit */
  seedInvalidated = generator->testDirty() || seedInvalidated;

  /* Regenerate our seed mesh (all top-level SurfaceQuadtreeNodes) if needed */
  if (seedInvalidated) {
    reset();
    enableGeomorphing = false;
  }
  else {
#ifdef DISABLE_GEOMORPHING
    enableGeomorphing = false;
#else
    enableGeomorphing = true;
#endif
  }

  /* Test our surface's bounding sphere, used for optimizing quadtree node cull tests */
  boundingSphere.frustumCode = ViewingFrustum::getInstance()->
    cullSphere(boundingSphere.center, boundingSphere.radius);
  
  reprioritize();

  /* split all splittable triangles */
  while (!SplitAbove.empty())
    SplitAbove.front()->split(this);
 
  /* morphing for vertices we generated in splitting */
  for (std::list<int>::iterator i=splitMorphVertices.begin(); i!=splitMorphVertices.end();) {
    std::list<int>::iterator j = i;
    SurfacePoint &v = vbuffer[*i];
    j = i;
    i++;

    if (v.morph(dt)) {
      /* Done */
      vbuffer.unref(*j);
      splitMorphVertices.erase(j);
    }
  }

  /* For all mergable triangles... */
  for (std::list<SurfaceQuadtreeNode*>::iterator i=MergeBelow.begin(); i!=MergeBelow.end();) {
    SurfaceQuadtreeNode *n = *i;
    i++;

    /* Morph the three midpoint vertices */
    if (vbuffer[n->children[SurfaceQuadtreeNode::a]->vertices[SurfaceQuadtreeNode::b]].morph(dt) &&
	vbuffer[n->children[SurfaceQuadtreeNode::b]->vertices[SurfaceQuadtreeNode::c]].morph(dt) &&
	vbuffer[n->children[SurfaceQuadtreeNode::c]->vertices[SurfaceQuadtreeNode::a]].morph(dt)) {

      /* Done morphing, merge it */
      n->mergeBottomHalf(this);
    }
  }
}

Surface::Surface(JetCOW *cow, Sint32 id, const char *type) : TransformedSceneNode(cow,id,type) {
  seedInvalidated = true;
  numTriangles = 0;
  generator = NULL;
  enableGeomorphing = false;
  DBG("In surface constructor\n");
  /* Set reasonable defaults if this is a new object */
  if (id<0) {
    setAttr("Generator", 0);
  }
}

Surface::~Surface() {
  for (std::vector<SurfaceQuadtreeNode>::iterator i=trees.begin(); i!=trees.end(); i++)
    delTree(*i);
  if (generator)
    generator->unref();
}

void Surface::loadCachedValues(void) {
  TransformedSceneNode::loadCachedValues();
  
  /* If our Generator has changed, reload it and mark it
   * as invalid so we'll regenerate our trees at the next draw()
   */
  int genId = getAttrIntProtected("Generator");
  DBG("Loading cached values, genId is %d\n", genId);
  if (!generator || generator->id != genId) {
    if (generator)
      generator->unref();
    if (genId)
      generator = (SurfaceGenerator*) cow->checkout("SurfaceGenerator",genId);
    else
      generator = NULL;
    seedInvalidated = true;
  }
}

void Surface::saveCachedValues(void) {
  TransformedSceneNode::saveCachedValues();
  setAttrProtected("Generator", generator ? generator->id : 0);
}

void Surface::prepareMesh(void) {
  typedef std::pair<SurfaceQuadtreeNode*,SurfaceQuadtreeNode::neighborDef*> EdgeContext;
  typedef std::multimap<SurfaceEdge, EdgeContext> EdgeMap;
  typedef std::pair<SurfaceEdge, EdgeContext> EdgePair;

  std::vector<SurfaceQuadtreeNode>::iterator ti;
  EdgeMap edges;
  EdgeMap::iterator ei, eiNext;

  /* Add all edges to a sorted pool -
   *  The multimap is sorted by triangle edge, and for each edge it gives a pair with:
   *    - The triangle it was found in
   *    - The neighbor pointer it would connect to
   */
  for (ti=trees.begin(); ti!=trees.end(); ti++) {
    edges.insert(EdgePair(SurfaceEdge(vbuffer[ti->vertices[SurfaceQuadtreeNode::a]], 
				      vbuffer[ti->vertices[SurfaceQuadtreeNode::b]]), 
			  EdgeContext(&(*ti), &(ti->neighbors[SurfaceQuadtreeNode::ab]))));
    edges.insert(EdgePair(SurfaceEdge(vbuffer[ti->vertices[SurfaceQuadtreeNode::b]], 
				      vbuffer[ti->vertices[SurfaceQuadtreeNode::c]]), 
			  EdgeContext(&(*ti), &(ti->neighbors[SurfaceQuadtreeNode::bc]))));
    edges.insert(EdgePair(SurfaceEdge(vbuffer[ti->vertices[SurfaceQuadtreeNode::c]],
				      vbuffer[ti->vertices[SurfaceQuadtreeNode::a]]),
			  EdgeContext(&(*ti), &(ti->neighbors[SurfaceQuadtreeNode::ca]))));

    memset(&ti->neighbors, 0, sizeof(ti->neighbors));

    /* Update geometry while we're here */
    ti->updateGeometry(this);
  }

  /* Pull each one out and assign neighbors */
  for (ei=edges.begin(); ei!=edges.end(); ei++) {
    eiNext = ei;
    eiNext++;
    if (eiNext==edges.end())
      break;

    if (ei->first == eiNext->first) {
      /* An edge is shared, link the two triangles together as neighbors */
      ei->second.second->n = eiNext->second.first;
      ei->second.second->side = (SurfaceQuadtreeNode::sideNames) 
	(eiNext->second.second - eiNext->second.first->neighbors);
      eiNext->second.second->n = ei->second.first;
      eiNext->second.second->side = (SurfaceQuadtreeNode::sideNames) 
	 (ei->second.second - ei->second.first->neighbors);
    }
  }
}

void Surface::delTree(SurfaceQuadtreeNode &tree) {
  for (int i=SurfaceQuadtreeNode::a;i<=SurfaceQuadtreeNode::c;i++)
    vbuffer.unref(tree.vertices[i]);
  
  if (tree.isEnqueued())
    tree.dequeue();
  
  if (!tree.isLeaf())
    for (int i=SurfaceQuadtreeNode::a;i<=SurfaceQuadtreeNode::center;i++) {
      delTree(*tree.children[i]);
      delete tree.children[i];
    }
}

/* The End */
