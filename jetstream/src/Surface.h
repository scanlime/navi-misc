/*
 * Surface.h - Interface for a class implementing dynamic real-time procedural
 *             surfaces including continuous level of detail using a generalization
 *             of the Diamond algorithm
 *
 *     References:
 *        The Diamond algorithm:
 *            http://www.cs.sun.ac.za/~henri/terrain.html
 *        Some useful general landscape generation info:
 *            http://www.wizardnet.com/musgrave/dissertation.pdf
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

#ifndef _H_SURFACE
#define _H_SURFACE

#include "JetCOW.h"
#include "Vector.h"
#include "VertexBuffer.h"
#include "Scene.h"
#include "ViewingFrustum.h"
#include "SurfaceGenerator.h"
#include <vector>
#include <map>
#include <list>

class Surface;


/***************************************************************** Utility data types ***/


/* Information about one point on a surface, both used
 * for inter-SurfaceGenerator communication and for rendering.
 */
class SurfacePoint : public BufferedVertex {
 public:
  SurfacePoint();
  SurfacePoint(Vector3 vertex, Vector3 normal, Vector3 parameter);
  bool operator == (const SurfacePoint &v) const;
  bool operator != (const SurfacePoint &v) const;
  bool operator < (const SurfacePoint &v) const;
  SurfacePoint& operator = (const BufferedVertex& v);

  /* For geomorphing:
   * Note that the current contents of the SurfacePoint's BufferedVertex is what's rendered
   * onscreen at the moment, which probably isn't what you want to use for calculations.
   * For calculations, two versions of the vertex are provided: the vertex that lies on the surface,
   * and the vertex that's a linear interpolation between the two points it was calculated from.
   * Depending on whether we're splitting or merging, we may be going either direction between the
   * two. 
   *
   * We define the morphing interpolation as:
   *   for morphBackwards == false:
   *     0.0 = linearInterpolation
   *     1.0 = surfaceInterpolation
   *   for morphBackwards == true:
   *     0.0 = surfaceInterpolation
   *     1.0 = linearInterpolation
   */
  BufferedVertex linearInterpolation;
  BufferedVertex surfaceInterpolation;
  float morphWeight;
  bool morphBackwards;

  /* Reset the two interpolations to the vertex's current value. This must
   * be used when a vertex is modified by a method other than geomorphing.
   */
  void morphReset(void);

  /* Perform geomorphing for one frame, returns true when done */
  bool morph(float dt);

  /* The SurfaceGenerator should store the parameter it uses to generate the
   * surface here, so it won't get confused when Surfacemodifiers change
   * the SurfacePoints. SurfaceModifiers should not touch this.
   */
  Vector3 parameter;

 private:
  void init();
};

/* Two adjacent points, used in triangle sorting */
class SurfaceEdge {
 public:
  SurfaceEdge(SurfacePoint &a, SurfacePoint &b);
  
  /* Equality is independent of point order,
   * the < comparison is arbitrary.
   */
  bool operator == (const SurfaceEdge &v) const;
  bool operator != (const SurfaceEdge &v) const;
  bool operator <  (const SurfaceEdge &v) const;

  /* The constructor ensures that a<b */
  SurfacePoint a,b;
};

/* The variance of a quadtree node is expressed as a bounding prism
 * indicating how far the children may protrude from the triangle's front and back.
 */
struct BoundingPrism {
  float front, back;
};


/***************************************************************** Quadtree node ***/

/* One triangle in a Quadtree.
 * All vertices are in counterclockwise order.
 */
class SurfaceQuadtreeNode {
 public:
  /****** Algorizhems *****/

  SurfaceQuadtreeNode();
  SurfaceQuadtreeNode(VertexBuffer<SurfacePoint> &vbuffer, 
		      SurfacePoint a, SurfacePoint b, SurfacePoint c);
  SurfaceQuadtreeNode(SurfaceQuadtreeNode *parent,
		      unsigned int a, unsigned int b, unsigned int c);

  void enqueue(std::list<SurfaceQuadtreeNode*> &l);
  void dequeue(void);

  /* Enqueue in MergeBelow and initialize geomorphing */
  void enqueueMergeBelow(Surface *s);

  void splitEnqueue(Surface *s);
  void mergeEnqueue(Surface *s);

  /* The split operation uses the given surface's generator,
   * both operations update the surface's triangle counts.
   */
  void split(Surface *s);   /* Generate children */

  /* Merge is split into two parts:
   *   - The top half dequeues children, and is performed
   *     before geomorphing
   *   - The bottom half deletes children and requeues, and is
   *     performed after geomorphing.
   */
  void mergeTopHalf(Surface *s);
  void mergeBottomHalf(Surface *s);

  bool isLeaf();
  bool isEnqueued();
  bool isMergable();

  /* Returns a mesh split/merge priority for the given node, giving the node's
   * current detail relative to the desired detail. Positive numbers indicate
   * the node should be split, negative indicates it should merge.
   *
   * Note: This uses the current OpenGL projection and modelview matrix, since
   *       it is assumed it will be called at draw-time for the surface.
   */
  float getPriority(Surface *s);

  /* Update the node's normal and bounding prism.
   * This should be called any time the vertices change.
   */
  void updateGeometry(Surface *s);

  /* Draw this subtree in the current OpenGL context */
  void draw(Surface *s);

  /* Draw a graphical representation of the surface's quadtrees, suitable for debugging */
  void drawDebug(Surface *s);

  /* Cull this node against the current frustum, update the 'culled' flag.
   * The triangle should only be culled if it is backfacing or it's completely outside
   * the frustum. The entire bounding prism should be used, not just the triangle itself.
   */
  void cull(Surface *s);

  /****** Static Data *****/

  enum vertexAndChildNames {
    a,
    b,
    c,
    center
  };

  enum sideNames {
    ab,
    bc,
    ca,
  };

  /* The two vertices, in order, for each side */
  static const struct sideInfoStruct {
    enum vertexAndChildNames v1, v2;
  } sideInfo[3];

  /****** Data *****/

  /* The plane normal is the unit vector specifying the direction
   * the bounds are measured against.
   */
  Vector3 planeNormal;

  /* Triangle centroid (average of it's vertices)
   * Used to calculate the priority of a triangle.
   */
  Vector3 centroid;

  /* Bounding prism for all children, measured along the plane normal. */
  BoundingPrism bounds;

  /* Bounding sphere radius used for a first approximation in culling, measured from the centroid */
  float cullRadius;

  /* Indexed with vertexAndChildNames, a triplet of indices into the vertex buffer */
  unsigned int vertices[3];

  /* True if the bounding prism is completely outside the frustum
   * or it's completely backfacing.
   */
  bool culled;

  /* If this is set, getPriority will always return -1. This is used to
   * keep very small triangles from splitting past what the floating point
   * precision can support.
   */
  bool unsplittable;

  /* I think I've re-added and re-deleted this about five times already...
   * It's not used by Surface itself, but I keep adding it back in for
   * some test I happen to be running...
   * Head nodes are depth 0.
   */
  int depth;

  /* Indexed with sideNames
   * this includes a pointer to the neighbor, and the side enum
   * in that neighbor that should point back to us.
   */
  struct neighborDef {
    SurfaceQuadtreeNode *n;
    enum sideNames side;
  } neighbors[3];
  
  SurfaceQuadtreeNode *children[4];

  SurfaceQuadtreeNode *parent;
  
  /* If isEnqueued is true, this points to the triangle's enqueued location */
  std::list<SurfaceQuadtreeNode*>::iterator iter;
  std::list<SurfaceQuadtreeNode*> *list;
};


/***************************************************************** Surface class ***/

class Surface : public TransformedSceneNode {
  friend class SurfaceQuadtreeNode;

 public:
  virtual void draw();
  virtual void animate(float dt);

  /* Dictionary keys: 
   *   Generator   - JetCOW object ID of the surface generator
   */

  /* JetCOW hooks - never call directly */
  Surface(JetCOW *cow, Sint32 id=0, const char *type="Surface");
  virtual ~Surface();
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  

  /* Vertex buffer */
  VertexBuffer<SurfacePoint> vbuffer;

  /* Updated by SurfaceQuadtreeNode */
  int numTriangles;

 private:
  /* Seed mesh and surface generator */
  bool seedInvalidated;
  bool enableGeomorphing;
  SurfaceGenerator *generator;
  std::vector<SurfaceQuadtreeNode> trees;
  void reset(void);
  void reprioritize(void);

  /* Bounding sphere, and the clip code the bounding sphere has */
  struct {
    Vector3 center;
    float radius;
    ViewingFrustum::intersectionCode frustumCode;
  } boundingSphere;

  /* Generate neighbor pointers and geometry information for the root nodes */
  void prepareMesh(void);

  /* Delete a tree of quadtreenodes. This can't be done by the node itself
   * because it must unref its vertices, and it doesn't have a pointer to our vbuffer.
   * Important: This doesn't delete the head quadtree node, only prepares it for deletion.
   *            This is necessary so this can be used on sequences of bare quadtree nodes.
   */
  void delTree(SurfaceQuadtreeNode &tree);

  /* List of vertices being morphed due to splitting */
  std::list<int> splitMorphVertices;

  /* Diamond algorithm */
  std::list<SurfaceQuadtreeNode*> SplitAbove, MergeAbove;
  std::list<SurfaceQuadtreeNode*> SplitBelow, MergeBelow;
};

#endif /* _H_SURFACE */

/* The End */
