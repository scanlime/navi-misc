/*
 * VertexBuffer.h - Template for creating efficient dynamically allocated
 *                  buffers of vertices to be processed by OpenGL
 *
 *     References:
 *         http://www.cs.rit.edu/~ncs/Courses/570/UserGuide/OpenGLonWin-15.html
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

#ifndef _H_VERTEXBUFFER
#define _H_VERTEXBUFFER

#include "Vector.h"
#include <vector>
#include <list>
#include <GL/gl.h>


/* This may be used as the base class to an application-specific
 * class for vertex data, or used as-is for basic vertices.
 */
class BufferedVertex {
 public:
  /* GL_T2F_C4F_N3F_V3F packing */
  GLfloat texcoord[2];
  GLfloat color[4];
  Vector3 normal;
  Vector3 vertex;
  
  /* refcount must be the item immediately after the GL params above,
   * since it is used to calculate the array stride.
   */
  int refcount;

  BufferedVertex() {
    refcount = 1;
    memset(texcoord, 0, sizeof(texcoord) + sizeof(color) + sizeof(normal) + sizeof(vertex));
  }
  
  /* Linear interpolation used as a source position in geomorphing.
   * Sets the vertex data as an interpolation between the given vertices,
   * and leaves the non-BufferedVertex data unchanged.
   */
  void interpolate(const BufferedVertex &from, const BufferedVertex &to, float weight) {
    texcoord[0] = interpolate(from.texcoord[0], to.texcoord[0], weight);
    texcoord[1] = interpolate(from.texcoord[1], to.texcoord[1], weight);
    color[0]    = interpolate(from.color[0],    to.color[0],    weight);
    color[1]    = interpolate(from.color[1],    to.color[1],    weight);
    color[2]    = interpolate(from.color[2],    to.color[2],    weight);
    color[3]    = interpolate(from.color[3],    to.color[3],    weight);
    normal[0]   = interpolate(from.normal[0],   to.normal[0],   weight);
    normal[1]   = interpolate(from.normal[1],   to.normal[1],   weight);
    normal[2]   = interpolate(from.normal[2],   to.normal[2],   weight);
    vertex[0]   = interpolate(from.vertex[0],   to.vertex[0],   weight);
    vertex[1]   = interpolate(from.vertex[1],   to.vertex[1],   weight);
    vertex[2]   = interpolate(from.vertex[2],   to.vertex[2],   weight);
  }

 private:  
  float interpolate(float from, float to, float weight) {
    return (to-from)*weight + from;
  }

} STRUCT_PACK;


/* The template is parameterized by the vertex class */
template <class T>
class VertexBuffer {
 public:

  VertexBuffer() {
    numUsedSlots = 0;
  }

  /* Set this as the current vertex buffer */
  void bind(void) {
    glInterleavedArrays(GL_T2F_C4F_N3F_V3F, sizeof(T), &buffer[0]);
  }

  /* Draw several items from the buffer, by index.
   * 'mode' is any constant that glBegin() can accept.
   */
  void draw(int mode, unsigned int *indices, unsigned int numIndices) {
    glDrawElements(mode, numIndices, GL_UNSIGNED_INT, indices);
  }

  /* Convert an index to a pointer. The pointer is only guaranteed
   * to be valid until the next call to alloc() or free()
   */
  T &operator [] (int index) {
    return buffer[index];
  }

  /* Allocate a new vertex, return the index */
  int alloc(void) {
    int i;
    numUsedSlots++;

    while (!freeSlots.empty()) {
      /* We don't remove slots from freeSlots immediately when the buffer shrinks,
       * we do that lazily right here.
       */
      i = freeSlots.front();
      freeSlots.pop_front();
      
      if (i < buffer.size()) {
	/* Reinitialize this slot and return it */
	buffer[i] = T();
	return i;
      }
    }
    
    /* Stick a new node onto the buffer, rely on std::vector to manage buffer memory */
    buffer.push_back(T());
    i = &buffer.back() - &buffer[0];    
    buffer[i].refcount = 1;
    return i;
  }
  
  void ref(int v) {
    buffer[v].refcount++;
  }

  /* Free an allocated vertex */
  void unref(int v) {
    if (--buffer[v].refcount == 0) {
      numUsedSlots--;
      freeSlots.push_front(v);
      
      /* Shrink the buffer if we can easily */
      while (allocatedSize() > 0 && buffer.back().refcount == 0)
	buffer.pop_back();
    }
  }

  float utilization(void) {
    return numUsedSlots * 100.0 / buffer.size();
  }

  int allocatedSize(void) {
    return buffer.size();
  }

  int size(void) {
    return numUsedSlots;
  }

 private:
  std::vector<T> buffer;
  std::list<int> freeSlots;
  int numUsedSlots;
};


#endif /* _H_VERTEXBUFFER */

/* The End */
