/*
 * Matrix.h - Matrix classes
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

#ifndef _H_MATRIX
#define _H_MATRIX

#include "Util.h"
#include <GL/gl.h>
#include <string.h>
#include "Vector.h"

/******************************************* Declarations ***/

class Matrix4x4 {
  public:    
    inline Matrix4x4();
    inline Matrix4x4(const GLfloat *m);

    inline Matrix4x4	&operator = (const Matrix4x4 &m);
    inline Matrix4x4	&operator += (const Matrix4x4 &m);
    inline Matrix4x4	&operator -= (const Matrix4x4 &m);
    inline Matrix4x4	&operator *= (const float &f);
    inline Matrix4x4	&operator *= (const Matrix4x4 &m);
    inline Matrix4x4	&operator /= (const float &f);

    inline Vector3	operator * (const Vector3 &v) const;
    inline Matrix4x4	operator * (const float &f) const;
    inline Matrix4x4	operator / (const float &f) const;
    inline Matrix4x4	operator * (const Matrix4x4 &m) const;
    inline Matrix4x4	operator + (const Matrix4x4 &m) const;
    inline Matrix4x4	operator - (const Matrix4x4 &m) const;

    inline Matrix4x4	&negate();
    inline Matrix4x4	&operator - ();
    inline Matrix4x4	&zero();
    inline float&	operator[] (unsigned int index);

    inline float	operator[] (unsigned int index) const;

    inline  bool	operator == (const Matrix4x4 &v) const;
    inline bool	operator != (const Matrix4x4 &v) const;

    /* Multiply the 3-vector by this matrix then divide by the
     * w to project the resulting vector into 2D space.
     * Returns the answer in OpenGL normalized coordinates.
     */
    inline Vector2     project(const Vector3 &v) const;

    /* A multiply by Vector3 that assumes the missing value is 0 rather than 1 */
    inline Vector3     multiply3(const Vector3 &v) const;

    GLfloat	vec[16];
};

class Matrix3x3 {
  public:
    inline Matrix3x3();
    inline Matrix3x3(const GLfloat *m);

    inline Matrix3x3	&operator = (const Matrix3x3 &m);
    inline Matrix3x3	&operator += (const Matrix3x3 &m);
    inline Matrix3x3	&operator -= (const Matrix3x3 &m);
    inline Matrix3x3	&operator *= (const float &f);
    inline Matrix3x3	&operator *= (const Matrix3x3 &m);
    inline Matrix3x3	&operator /= (const float &f);

    inline Matrix3x3	operator * (const float &f) const;
    inline  Matrix3x3	operator / (const float &f) const;
    inline Matrix3x3	operator * (const Matrix3x3 &m) const;
    inline Matrix3x3	operator + (const Matrix3x3 &m) const;
    inline Matrix3x3	operator - (const Matrix3x3 &m) const;

    inline Matrix3x3	&negate();
    inline Matrix3x3	&operator - ();
    inline Matrix3x3	&zero();
    inline float	&operator[] (unsigned int index);
    inline float	operator[] (unsigned int index) const;

    inline bool	operator == (const Matrix3x3 &m) const;
    inline bool	operator != (const Matrix3x3 &m) const;

    inline Vector3	operator * (const Vector3 &v) const;

    GLfloat	vec[9];
};


/******************************************* Matrix4x4 Class ***/

inline Matrix4x4::Matrix4x4() {
  zero();
}

inline Matrix4x4::Matrix4x4(const GLfloat *m) {
  memcpy(vec,m,sizeof(vec));
}

inline Matrix4x4 &Matrix4x4::operator = (const Matrix4x4 &m) {
  memcpy(vec,m.vec,sizeof(vec));
}

inline Matrix4x4 &Matrix4x4::operator += (const Matrix4x4 &m) {
  int i;
  for (i=0;i<16;i++)
    vec[i] += m.vec[i];
  return *this;
}

inline Matrix4x4 &Matrix4x4::operator -= (const Matrix4x4 &m) {
  int i;
  for (i=0;i<16;i++)
    vec[i] -= m.vec[i];
  return *this;
}

inline Matrix4x4 &Matrix4x4::operator *= (const float &f) {
  int i;
  for (i=0;i<16;i++)
    vec[i] *= f;
  return *this;
}

inline Matrix4x4 &Matrix4x4::operator *= (const Matrix4x4 &m) {
  *this = (*this) * m;
  return *this;
}

inline Matrix4x4 &Matrix4x4::operator /= (const float &f) {
  int i;
  for (i=0;i<16;i++)
    vec[i] /= f;
  return *this;
}

inline Matrix4x4 Matrix4x4::operator * (const float &f) const {
  Matrix4x4 n = *this;
  n *= f;
  return n;
}

inline Matrix4x4 Matrix4x4::operator / (const float &f) const {
  Matrix4x4 n = *this;
  n /= f;
  return n;
}

inline Matrix4x4 Matrix4x4::operator * (const Matrix4x4 &m) const {
  Matrix4x4 n;
  n.vec[ 0] = vec[ 0]*m.vec[ 0] + vec[ 4]*m.vec[ 1] + vec[ 8]*m.vec[ 2] + vec[12]*m.vec[ 3];
  n.vec[ 1] = vec[ 1]*m.vec[ 0] + vec[ 5]*m.vec[ 1] + vec[ 9]*m.vec[ 2] + vec[13]*m.vec[ 3];
  n.vec[ 2] = vec[ 2]*m.vec[ 0] + vec[ 6]*m.vec[ 1] + vec[10]*m.vec[ 2] + vec[14]*m.vec[ 3];
  n.vec[ 3] = vec[ 3]*m.vec[ 0] + vec[ 7]*m.vec[ 1] + vec[11]*m.vec[ 2] + vec[15]*m.vec[ 3];
  n.vec[ 4] = vec[ 0]*m.vec[ 4] + vec[ 4]*m.vec[ 5] + vec[ 8]*m.vec[ 6] + vec[12]*m.vec[ 7];
  n.vec[ 5] = vec[ 1]*m.vec[ 4] + vec[ 5]*m.vec[ 5] + vec[ 9]*m.vec[ 6] + vec[13]*m.vec[ 7];
  n.vec[ 6] = vec[ 2]*m.vec[ 4] + vec[ 6]*m.vec[ 5] + vec[10]*m.vec[ 6] + vec[14]*m.vec[ 7];
  n.vec[ 7] = vec[ 3]*m.vec[ 4] + vec[ 7]*m.vec[ 5] + vec[11]*m.vec[ 6] + vec[15]*m.vec[ 7];
  n.vec[ 8] = vec[ 0]*m.vec[ 8] + vec[ 4]*m.vec[ 9] + vec[ 8]*m.vec[10] + vec[12]*m.vec[11];
  n.vec[ 9] = vec[ 1]*m.vec[ 8] + vec[ 5]*m.vec[ 9] + vec[ 9]*m.vec[10] + vec[13]*m.vec[11];
  n.vec[10] = vec[ 2]*m.vec[ 8] + vec[ 6]*m.vec[ 9] + vec[10]*m.vec[10] + vec[14]*m.vec[11];
  n.vec[11] = vec[ 3]*m.vec[ 8] + vec[ 7]*m.vec[ 9] + vec[11]*m.vec[10] + vec[15]*m.vec[11];
  n.vec[12] = vec[ 0]*m.vec[12] + vec[ 4]*m.vec[13] + vec[ 8]*m.vec[14] + vec[12]*m.vec[15];
  n.vec[13] = vec[ 1]*m.vec[12] + vec[ 5]*m.vec[13] + vec[ 9]*m.vec[14] + vec[13]*m.vec[15];
  n.vec[14] = vec[ 2]*m.vec[12] + vec[ 6]*m.vec[13] + vec[10]*m.vec[14] + vec[14]*m.vec[15];
  n.vec[15] = vec[ 3]*m.vec[12] + vec[ 7]*m.vec[13] + vec[11]*m.vec[14] + vec[15]*m.vec[15];
  return n;
}

inline Vector3	Matrix4x4::operator * (const Vector3 &v) const {
  Vector3 n;
  n[0] = v[0]*vec[ 0] + v[1]*vec[ 4] + v[2]*vec[ 8] + vec[12];
  n[1] = v[0]*vec[ 1] + v[1]*vec[ 5] + v[2]*vec[ 9] + vec[13];
  n[2] = v[0]*vec[ 2] + v[1]*vec[ 6] + v[2]*vec[10] + vec[14];
  return n;
}

/* A multiply by Vector3 that assumes the missing value is 0 rather than 1 */
inline Vector3 Matrix4x4::multiply3(const Vector3 &v) const {
  Vector3 n;
  n[0] = v[0]*vec[ 0] + v[1]*vec[ 4] + v[2]*vec[ 8];
  n[1] = v[0]*vec[ 1] + v[1]*vec[ 5] + v[2]*vec[ 9];
  n[2] = v[0]*vec[ 2] + v[1]*vec[ 6] + v[2]*vec[10];
  return n;
}

inline Vector2 Matrix4x4::project(const Vector3 &v) const {
  Vector2 n;
  float w;
 
  n[0] = v[0]*vec[ 0] + v[1]*vec[ 4] + v[2]*vec[ 8] + vec[12];
  n[1] = v[0]*vec[ 1] + v[1]*vec[ 5] + v[2]*vec[ 9] + vec[13];
  w    = v[0]*vec[ 3] + v[1]*vec[ 7] + v[2]*vec[11] + vec[14];
  n   /= w;

  return n;
}

inline Matrix4x4 Matrix4x4::operator + (const Matrix4x4 &m) const {
  Matrix4x4 n = *this;
  n += m;
  return n;
}

inline Matrix4x4 Matrix4x4::operator - (const Matrix4x4 &m) const {
  Matrix4x4 n = *this;
  n -= m;
  return n;
}

inline Matrix4x4 &Matrix4x4::negate() {
  int i;
  for (i=0;i<16;i++)
    vec[i] = -vec[i];
  return *this;
}

inline Matrix4x4 &Matrix4x4::operator - () {
  negate();
  return *this;
}

inline Matrix4x4 &Matrix4x4::zero() {
  memset(vec,0,sizeof(vec));
}

inline float& Matrix4x4::operator[] (unsigned int index) {
  return vec[index];
}

inline float Matrix4x4::operator[] (unsigned int index) const {
  return vec[index];
}

inline bool Matrix4x4::operator == (const Matrix4x4 &v) const {
  int i;
  for (i=0;i<16;i++)
    if (vec[i] != v.vec[i])
      return false;
  return true;
}

inline bool Matrix4x4::operator != (const Matrix4x4 &v) const {
  int i;
  for (i=0;i<16;i++)
    if (vec[i] == v.vec[i])
      return false;
  return true;
}


/******************************************* Matrix3x3 Class ***/

inline Matrix3x3::Matrix3x3() {
  zero();
}

inline Matrix3x3::Matrix3x3(const GLfloat *m) {
  memcpy(vec, m, sizeof(vec));
}

inline Matrix3x3 &Matrix3x3::operator = (const Matrix3x3 &m) {
  memcpy(vec, m.vec, sizeof(vec));
}

inline Matrix3x3 &Matrix3x3::operator += (const Matrix3x3 &m) {
  for(int i = 0; i < 9; i++)
    vec[i] += m.vec[i];
  return *this;
}

inline Matrix3x3 &Matrix3x3::operator -= (const Matrix3x3 &m) {
  for(int i = 0; i < 9; i++)
    vec[i] += m.vec[i];
  return *this;
}

inline Matrix3x3 &Matrix3x3::operator *= (const float &f) {
  for(int i = 0; i < 9; i++)
    vec[i] *= f;
  return *this;
}

inline Matrix3x3 &Matrix3x3::operator *= (const Matrix3x3 &m) {
  *this = (*this) * m;
  return *this;
}

inline Matrix3x3 &Matrix3x3::operator /= (const float &f) {
  for(int i = 0; i < 9; i++)
    vec[i] /= f;
  return *this;
}

inline Matrix3x3 Matrix3x3::operator * (const float &f) const {
  Matrix3x3 n = *this;
  n *= f;
  return n;
}

inline Matrix3x3 Matrix3x3::operator / (const float &f) const {
  Matrix3x3 n = *this;
  n /= f;
  return n;
}

inline Matrix3x3 Matrix3x3::operator * (const Matrix3x3 &m) const {
  Matrix3x3 n;
  n.vec[0] = vec[0] * m.vec[0] + vec[3] * m.vec[1] + vec[6] * m.vec[2];
  n.vec[1] = vec[1] * m.vec[0] + vec[4] * m.vec[1] + vec[7] * m.vec[2];
  n.vec[2] = vec[2] * m.vec[0] + vec[5] * m.vec[1] + vec[8] * m.vec[2];
  n.vec[3] = vec[0] * m.vec[3] + vec[3] * m.vec[4] + vec[6] * m.vec[5];
  n.vec[4] = vec[1] * m.vec[3] + vec[4] * m.vec[4] + vec[7] * m.vec[5];
  n.vec[5] = vec[2] * m.vec[3] + vec[5] * m.vec[4] + vec[8] * m.vec[5];
  n.vec[6] = vec[0] * m.vec[6] + vec[3] * m.vec[7] + vec[6] * m.vec[8];
  n.vec[7] = vec[1] * m.vec[6] + vec[4] * m.vec[7] + vec[7] * m.vec[8];
  n.vec[8] = vec[2] * m.vec[6] + vec[5] * m.vec[7] + vec[8] * m.vec[8];
  return n;
}

inline Matrix3x3 Matrix3x3::operator + (const Matrix3x3 &m) const {
  Matrix3x3 n = *this;
  n += m;
  return n;
}

inline Matrix3x3 Matrix3x3::operator - (const Matrix3x3 &m) const {
  Matrix3x3 n = *this;
  n -= m;
  return n;
}

inline Matrix3x3 &Matrix3x3::negate() {
  for(int i = 0; i < 9; i++)
    vec[i] = -vec[i];
  return *this;
}

inline Matrix3x3 &Matrix3x3::operator - () {
  negate();
  return *this;
}

inline Matrix3x3 &Matrix3x3::zero() {
  memset(vec, 0, sizeof(vec));
}

inline float &Matrix3x3::operator[] (unsigned int index) {
  return vec[index];
}

inline float Matrix3x3::operator[] (unsigned int index) const {
  return vec[index];
}

inline bool Matrix3x3::operator == (const Matrix3x3 &m) const {
  for(int i = 0; i < 9; i++)
    if(vec[i] != m.vec[i])
      return false;
  return true;
}

inline bool Matrix3x3::operator != (const Matrix3x3 &m) const {
  for(int i = 0; i < 9; i++)
    if(vec[i] == m.vec[i])
      return false;
  return true;
}

inline Vector3	Matrix3x3::operator * (const Vector3 &v) const {
  Vector3 result;
  for(int i = 0; i < 3; i++)
    result[i] = vec[i * 3] * v[0] + vec[i * 3 + 1] * v[1] + vec[i * 3 + 2] * v[2];
  return result;
}

#endif

/* The End */
