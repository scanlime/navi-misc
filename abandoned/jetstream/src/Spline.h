/*
 * Spline.h - Spline classes
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

#ifndef _H_SPLINE
#define _H_SPLINE

#include "Vector.h"

class Spline3 {
 public:
  Spline3();

  Vector3	compute(float distance);

  /* Python helper functions */
  void		set1(unsigned int which, unsigned int index, float value);
  float		get1(unsigned int which, unsigned int index);
  void		setv1(unsigned int which, Vector3 v);

  friend class SplineSceneNode;

 private:
  inline void		bernstein(float u, float b[4]);
  Vector3 p[4];
};

#endif
