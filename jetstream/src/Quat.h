/*
 * Quat.h - Quaternion classes
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

#ifndef _H_QUAT
#define _H_QUAT

#include "Vector.h"
#include "Matrix.h"

class Quat {
 public:
  Quat();
  Quat(float r);
  Quat(Vector3 v);
  Quat(float r, Vector3 v);
  Quat(float r, float i, float j, float k);

  Quat &operator = (const Quat &q);
  Quat &operator += (const Quat &q);
  Quat &operator -= (const Quat &q);
  Quat &operator *= (const Quat &q);
  Quat &operator *= (const float &f);
  Quat &operator /= (const float &f);
  Quat &operator /= (const Quat &q);
  Quat operator * (const float &f) const;
  Quat operator * (const Quat &q) const;
  Quat operator + (const Quat &q) const;
  Quat operator / (const Quat &q) const;

  Vector3 operator * (const Vector3 &v) const;
  
  Quat &normalize();
  Quat &inverse();
  Quat &negate();
  Quat &operator - ();
  Quat &zero();
  Quat &conjugate();
  Quat slerp(const Quat &to, float h);
  Quat pow(float p);
  float dot(const Quat &q) const;

  float length();
  float length2();

  void fromAxis(const float &angle, const Vector3 &axis);
  void toAxis(float &angle, Vector3 &axis);
  Matrix3x3 toRotationMatrix() const;

  float norm() const;
  float magnitude() const;
  
  bool operator == (const Quat &v) const;
  bool operator != (const Quat &v) const;
  
  float r;
  Vector3 v;
};

#endif /* _H_QUAT */

/* The End */
