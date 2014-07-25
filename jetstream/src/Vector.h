/*
 * Vector.h - Vector classes, all inlined.
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

#ifndef _H_VECTOR
#define _H_VECTOR

#include "Util.h"
#include <GL/gl.h>
#include <math.h>


/******************************************* Declarations ***/

class Vector4 {
 public:
  typedef const float* iterator;

  inline Vector4();
  inline Vector4(float x, float y, float z, float w);
  inline Vector4(const float *v);
  inline Vector4(const double *v);

  inline Vector4 &operator = (const Vector4 &v);
  inline Vector4 &operator += (const Vector4 &v);
  inline Vector4 &operator -= (const Vector4 &v);
  inline Vector4 &operator *= (const float &f);
  inline Vector4 &operator /= (const float &f);

  inline Vector4 operator * (const float &f) const;
  inline Vector4 operator / (const float &f) const;
  inline float   operator * (const Vector4 &f) const;
  inline Vector4 operator + (const Vector4 &v) const;
  inline Vector4 operator - (const Vector4 &v) const;

  inline Vector4 &normalize();
  inline Vector4 &negate();
  inline Vector4 &operator - ();
  inline Vector4 &zero();
  inline void    set(float x, float y, float z, float w);
  inline float&	 operator[] (unsigned int index);

  /* Helper functions for the python wrapper */
  inline void    set1(unsigned int index, float value);
  inline float   get1(unsigned int index) const;
  inline void    setv(Vector4 v);

  inline float	  operator[] (unsigned int index) const;
  inline iterator begin();
  inline const    float	*get() const;
  inline float    length() const;
  inline float    length2() const;

  inline bool operator == (const Vector4 &v) const;
  inline bool operator != (const Vector4 &v) const;

  inline void fillGL(GLfloat v[4]) const;

  inline void fromBuffer(Uint8 *v);
  inline void toBuffer(Uint8 *v) const;    

 private:
  float	vec[4];
};

class Vector3 {
 public:
  typedef const float* iterator;

  inline Vector3();
  inline Vector3(float x, float y, float z);
  inline Vector3(const float *v);
  inline Vector3(const double *v);

  inline Vector3 &operator = (const Vector3 &v);
  inline Vector3 &operator += (const Vector3 &v);
  inline Vector3 &operator -= (const Vector3 &v);
  inline Vector3 &operator %= (const Vector3 &v);
  inline Vector3 &operator *= (const float &f);
  inline Vector3 &operator /= (const float &f);

  inline Vector3 operator * (const float &f) const;
  inline Vector3 operator / (const float &f) const;
  inline float   operator * (const Vector3 &f) const;    /* Dot product */
  inline Vector3 operator % (const Vector3 &f) const;    /* Cross product */
  inline Vector3 operator + (const Vector3 &v) const;
  inline Vector3 operator - (const Vector3 &v) const;

  inline Vector3 &normalize();
  inline Vector3 &negate();
  inline Vector3 &operator - ();
  inline Vector3 &zero();
  inline Vector3 &rotate(float angle, float x, float y, float z);
  inline void    set(float x, float y, float z);
  inline float&	 operator[] (unsigned int index);

  /* Helper functions for the python wrapper */
  inline void  set1(unsigned int index, float value);
  inline float get1(unsigned int index) const;
  inline void  setv(Vector3 v);

  inline Vector3 perpendicular() const;

  inline float       operator[] (unsigned int index) const;
  inline iterator    begin();
  inline const float *get() const;
  inline float       length() const;
  inline float       length2() const;

  inline bool operator == (const Vector3 &v) const;
  inline bool operator != (const Vector3 &v) const;

  inline void fillGL(GLfloat v[3]) const;

  inline void fromBuffer(Uint8 *v);
  inline void toBuffer(Uint8 *v) const;    

 private:
  float	vec[3];
};

class Vector2 {
 public:
  typedef const float* iterator;

  inline Vector2();
  inline Vector2(float x, float y);
  inline Vector2(const float *v);
  inline Vector2(const double *v);

  inline Vector2 &operator = (const Vector2 &v);
  inline Vector2 &operator += (const Vector2 &v);
  inline Vector2 &operator -= (const Vector2 &v);
  inline Vector2 &operator *= (const float &f);
  inline Vector2 &operator /= (const float &f);

  inline Vector2 operator * (const float &f) const;
  inline Vector2 operator / (const float &f) const;
  inline float   operator * (const Vector2 &f) const;    /* Dot product */
  inline Vector2 operator + (const Vector2 &v) const;
  inline Vector2 operator - (const Vector2 &v) const;

  inline Vector2 &normalize();
  inline Vector2 &negate();
  inline Vector2 &operator - ();
  inline Vector2 &zero();
  inline void    set(float x, float y);
  inline float&  operator[] (unsigned int index);

  /* Helper functions for the python wrapper */
  inline void  set1(unsigned int index, float value);
  inline float get1(unsigned int index) const;
  inline void  setv(Vector2 v);

  inline float       operator[] (unsigned int index) const;
  inline iterator    begin();
  inline const float *get() const;
  inline float       length() const;
  inline float       length2() const;

  inline bool operator == (const Vector2 &v) const;
  inline bool operator != (const Vector2 &v) const;

  inline void fillGL(GLfloat v[2]) const;

  inline void fromBuffer(Uint8 *v);
  inline void toBuffer(Uint8 *v) const;    

 private:
  float	vec[2];
};



/******************************************* Vector4 Class ***/

inline Vector4::Vector4() {
  vec[0] = vec[1] = vec[2] = vec[3] = 0;
}

inline Vector4::Vector4(float x, float y, float z, float w) {
  vec[0] = x;
  vec[1] = y;
  vec[2] = z;
  vec[3] = w;
}

inline Vector4::Vector4(const float *v) {
  vec[0] = v[0];
  vec[1] = v[1];
  vec[2] = v[2];
  vec[3] = v[3];
}

inline Vector4::Vector4(const double *v) {
  vec[0] = v[0];
  vec[1] = v[1];
  vec[2] = v[2];
  vec[3] = v[3];
}

inline Vector4	&Vector4::operator = (const Vector4 &v) {
  vec[0] = v.vec[0];
  vec[1] = v.vec[1];
  vec[2] = v.vec[2];
  vec[3] = v.vec[3];
  return *this;
}

inline Vector4	&Vector4::operator += (const Vector4 &v) {
  vec[0] = vec[0] + v.vec[0];
  vec[1] = vec[1] + v.vec[1];
  vec[2] = vec[2] + v.vec[2];
  vec[3] = vec[3] + v.vec[3];
  return *this;
}

inline Vector4	&Vector4::operator -= (const Vector4 &v) {
  vec[0] = vec[0] - v.vec[0];
  vec[1] = vec[1] - v.vec[1];
  vec[2] = vec[2] - v.vec[2];
  vec[3] = vec[3] - v.vec[3];
  return *this;
}

inline float Vector4::operator * (const Vector4 &v) const {
  return vec[0]*v.vec[0] + vec[1]*v.vec[1] + vec[2]*v.vec[2] + vec[3]*v.vec[3];
}

inline Vector4	&Vector4::operator *= (const float &f) {
  vec[0] *= f;
  vec[1] *= f;
  vec[2] *= f;
  vec[3] *= f;
  return *this;
}

inline Vector4	&Vector4::operator /= (const float &f) {
  vec[0] /= f;
  vec[1] /= f;
  vec[2] /= f;
  vec[3] /= f;
  return *this;
}

inline Vector4	Vector4::operator * (const float &f) const {
  Vector4 result;
  result.vec[0] = vec[0] * f;
  result.vec[1] = vec[1] * f;
  result.vec[2] = vec[2] * f;
  result.vec[3] = vec[3] * f;
  return result;
}

inline Vector4	Vector4::operator / (const float &f) const {
  Vector4 result;
  result.vec[0] = vec[0] / f;
  result.vec[1] = vec[1] / f;
  result.vec[2] = vec[2] / f;
  result.vec[3] = vec[3] / f;
  return result;
}

inline Vector4	Vector4::operator + (const Vector4 &v) const {
  Vector4 result;
  result.vec[0] = vec[0] + v.vec[0];
  result.vec[1] = vec[1] + v.vec[1];
  result.vec[2] = vec[2] + v.vec[2];
  result.vec[3] = vec[3] + v.vec[3];
  return result;
}

inline Vector4	Vector4::operator - (const Vector4 &v) const {
  Vector4 result;
  result.vec[0] = vec[0] - v.vec[0];
  result.vec[1] = vec[1] - v.vec[1];
  result.vec[2] = vec[2] - v.vec[2];
  result.vec[3] = vec[3] - v.vec[3];
  return result;
}

inline Vector4	&Vector4::normalize() {
  return ((*this) /= length());
}

inline Vector4	&Vector4::negate() {
  vec[0] = -vec[0];
  vec[1] = -vec[1];
  vec[2] = -vec[2];
  vec[3] = -vec[3];
  return *this;
}

inline Vector4	&Vector4::operator - () {
  return negate();
}

inline Vector4	&Vector4::zero() {
  vec[0] = vec[1] = vec[2] = vec[3] = 0;
  return *this;
}

inline void Vector4::set(float x, float y, float z, float w) {
  vec[0] = x;
  vec[1] = y;
  vec[2] = z;
  vec[3] = w;
}

inline void Vector4::setv(Vector4 v) {
  *this = v;
}

inline float& Vector4::operator[] (unsigned int index) {
  return vec[index];
}

inline float Vector4::operator[] (unsigned int index) const {
  return vec[index];
}

inline float Vector4::get1(unsigned int index) const {
  return vec[index];
}

inline void Vector4::set1(unsigned int index, float value) {
  vec[index] = value;
}

inline Vector4::iterator Vector4::begin() {
  return vec;
}

inline const float *Vector4::get() const {
  return vec;
}

inline float Vector4::length() const {
  return sqrt(length2());
}

inline float Vector4::length2() const {
  return ((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]) + (vec[3] * vec[3]));
}

inline bool Vector4::operator == (const Vector4 &v) const {
  return ((vec[0] == v.vec[0]) && (vec[1] == v.vec[1]) && (vec[2] == v.vec[2]) && (vec[3] == v.vec[3]));
}

inline bool Vector4::operator != (const Vector4 &v) const {
  return (vec[0] != v.vec[0]) || (vec[1] != v.vec[1]) || (vec[2] != v.vec[2]) || (vec[3] != v.vec[3]);
}

inline void Vector4::fillGL(GLfloat v[4]) const {
  v[0] = vec[0];
  v[1] = vec[1];
  v[2] = vec[2];
  v[3] = vec[3];
}

inline void Vector4::fromBuffer(Uint8 *v) {
  vec[0] = ((float*)v)[0];
  vec[1] = ((float*)v)[1];
  vec[2] = ((float*)v)[2];
  vec[3] = ((float*)v)[3];
}

inline void Vector4::toBuffer(Uint8 *v) const {
  ((float*)v)[0] = vec[0];
  ((float*)v)[1] = vec[1];
  ((float*)v)[2] = vec[2];
  ((float*)v)[3] = vec[3];
}


/******************************************* Vector3 Class ***/

inline Vector3::Vector3() {
  vec[0] = vec[1] = vec[2] = 0;
}

inline Vector3::Vector3(float x, float y, float z) {
  vec[0] = x;
  vec[1] = y;
  vec[2] = z;
}

inline Vector3::Vector3(const float *v) {
  vec[0] = v[0];
  vec[1] = v[1];
  vec[2] = v[2];
}

inline Vector3::Vector3(const double *v) {
  vec[0] = v[0];
  vec[1] = v[1];
  vec[2] = v[2];
}

inline Vector3	&Vector3::operator = (const Vector3 &v) {
  vec[0] = v.vec[0];
  vec[1] = v.vec[1];
  vec[2] = v.vec[2];
  return *this;
}

inline Vector3	&Vector3::operator += (const Vector3 &v) {
  vec[0] = vec[0] + v.vec[0];
  vec[1] = vec[1] + v.vec[1];
  vec[2] = vec[2] + v.vec[2];
  return *this;
}

inline Vector3	&Vector3::operator -= (const Vector3 &v) {
  vec[0] = vec[0] - v.vec[0];
  vec[1] = vec[1] - v.vec[1];
  vec[2] = vec[2] - v.vec[2];
  return *this;
}

inline Vector3	&Vector3::operator %= (const Vector3 &v) {
  *this = (*this) % v;
  return *this;
}

inline Vector3	Vector3::operator % (const Vector3 &v) const {
  Vector3 u;
  u.vec[0] = vec[1]*v.vec[2] - vec[2]*v.vec[1];
  u.vec[1] = vec[2]*v.vec[0] - vec[0]*v.vec[2];
  u.vec[2] = vec[0]*v.vec[1] - vec[1]*v.vec[0];
  return u;
}

inline float Vector3::operator * (const Vector3 &v) const {
  return vec[0]*v.vec[0] + vec[1]*v.vec[1] + vec[2]*v.vec[2];
}

inline Vector3	&Vector3::operator *= (const float &f) {
  vec[0] *= f;
  vec[1] *= f;
  vec[2] *= f;
  return *this;
}

inline Vector3	&Vector3::operator /= (const float &f) {
  vec[0] /= f;
  vec[1] /= f;
  vec[2] /= f;
  return *this;
}

inline Vector3	Vector3::operator * (const float &f) const {
  Vector3 result;
  result.vec[0] = vec[0] * f;
  result.vec[1] = vec[1] * f;
  result.vec[2] = vec[2] * f;
  return result;
}

inline Vector3	Vector3::operator / (const float &f) const {
  Vector3 result;
  result.vec[0] = vec[0] / f;
  result.vec[1] = vec[1] / f;
  result.vec[2] = vec[2] / f;
  return result;
}

inline Vector3	Vector3::operator + (const Vector3 &v) const {
  Vector3 result;
  result.vec[0] = vec[0] + v.vec[0];
  result.vec[1] = vec[1] + v.vec[1];
  result.vec[2] = vec[2] + v.vec[2];
  return result;
}

inline Vector3	Vector3::operator - (const Vector3 &v) const {
  Vector3 result;
  result.vec[0] = vec[0] - v.vec[0];
  result.vec[1] = vec[1] - v.vec[1];
  result.vec[2] = vec[2] - v.vec[2];
  return result;
}

inline Vector3	&Vector3::normalize() {
  return ((*this) /= length());
}

inline Vector3	&Vector3::negate() {
  vec[0] = -vec[0];
  vec[1] = -vec[1];
  vec[2] = -vec[2];
  return *this;
}

inline Vector3	&Vector3::operator - () {
  return negate();
}

inline Vector3	&Vector3::zero() {
  vec[0] = vec[1] = vec[2] = 0;
  return *this;
}

inline void Vector3::set(float x, float y, float z) {
  vec[0] = x;
  vec[1] = y;
  vec[2] = z;
}

inline void Vector3::setv(Vector3 v) {
  *this = v;
}

inline Vector3 Vector3::perpendicular() const {
  static float squarezero = 1e-06 * 1e-06;
  Vector3 perp = (*this) % Vector3(1, 0, 0);
  if(perp.length2() < squarezero)
    perp = (*this) % Vector3(0, 1, 0);
  return perp;
}

inline float& Vector3::operator[] (unsigned int index) {
  return vec[index];
}

inline float Vector3::operator[] (unsigned int index) const {
  return vec[index];
}

inline float Vector3::get1(unsigned int index) const {
  return vec[index];
}

inline void Vector3::set1(unsigned int index, float value) {
  vec[index] = value;
}

inline Vector3::iterator Vector3::begin() {
  return vec;
}

inline const float *Vector3::get() const {
  return vec;
}

inline float Vector3::length() const {
  return sqrt(length2());
}

inline float Vector3::length2() const {
  return ((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]));
}

inline bool Vector3::operator == (const Vector3 &v) const {
  return ((vec[0] == v.vec[0]) && (vec[1] == v.vec[1]) && (vec[2] == v.vec[2]));
}

inline bool Vector3::operator != (const Vector3 &v) const {
  return (vec[0] != v.vec[0]) || (vec[1] != v.vec[1]) || (vec[2] != v.vec[2]);
}

inline void Vector3::fillGL(GLfloat v[3]) const {
  v[0] = vec[0];
  v[1] = vec[1];
  v[2] = vec[2];
}

inline void Vector3::fromBuffer(Uint8 *v) {
  vec[0] = ((float*)v)[0];
  vec[1] = ((float*)v)[1];
  vec[2] = ((float*)v)[2];
}

inline void Vector3::toBuffer(Uint8 *v) const {
  ((float*)v)[0] = vec[0];
  ((float*)v)[1] = vec[1];
  ((float*)v)[2] = vec[2];
}

inline Vector3 &Vector3::rotate(float angle, float x, float y, float z) {
  float m[3][3];
  float c,s,t;

  /* Rotation matrix */
  angle *= -PI / 180;
  c = cos(angle);
  s = sin(angle);
  t = 1-c;
  m[0][0] = t*x*x + c;     m[1][0] = t*x*y - s*z;     m[2][0] = t*x*z + s*y;
  m[0][1] = t*x*y + s*z;   m[1][1] = t*y*y + c;       m[2][1] = t*y*z - s*x;
  m[0][2] = t*x*z - s*y;   m[1][2] = t*y*z + s*x;     m[2][2] = t*z*z + c;
  
  /* Multiply the vector by it */
  set( m[0][0] * vec[0]  + m[1][0] * vec[1]  + m[2][0] * vec[2],
       m[0][1] * vec[0]  + m[1][1] * vec[1]  + m[2][1] * vec[2],
       m[0][2] * vec[0]  + m[1][2] * vec[1]  + m[2][2] * vec[2] );
 
  return *this;
}


/******************************************* Vector2 Class ***/

inline Vector2::Vector2() {
  vec[0] = vec[1] = 0;
}

inline Vector2::Vector2(float x, float y) {
  vec[0] = x;
  vec[1] = y;
}

inline Vector2::Vector2(const float *v) {
  vec[0] = v[0];
  vec[1] = v[1];
}

inline Vector2::Vector2(const double *v) {
  vec[0] = v[0];
  vec[1] = v[1];
}

inline Vector2	&Vector2::operator = (const Vector2 &v) {
  vec[0] = v.vec[0];
  vec[1] = v.vec[1];
  return *this;
}

inline Vector2	&Vector2::operator += (const Vector2 &v) {
  vec[0] = vec[0] + v.vec[0];
  vec[1] = vec[1] + v.vec[1];
  return *this;
}

inline Vector2	&Vector2::operator -= (const Vector2 &v) {
  vec[0] = vec[0] - v.vec[0];
  vec[1] = vec[1] - v.vec[1];
  return *this;
}

inline float Vector2::operator * (const Vector2 &v) const {
  return vec[0]*v.vec[0] + vec[1]*v.vec[1];
}

inline Vector2	&Vector2::operator *= (const float &f) {
  vec[0] *= f;
  vec[1] *= f;
  return *this;
}

inline Vector2	&Vector2::operator /= (const float &f) {
  vec[0] /= f;
  vec[1] /= f;
  return *this;
}

inline Vector2	Vector2::operator * (const float &f) const {
  Vector2 result;
  result.vec[0] = vec[0] * f;
  result.vec[1] = vec[1] * f;
  return result;
}

inline Vector2	Vector2::operator / (const float &f) const {
  Vector2 result;
  result.vec[0] = vec[0] / f;
  result.vec[1] = vec[1] / f;
  return result;
}

inline Vector2	Vector2::operator + (const Vector2 &v) const {
  Vector2 result;
  result.vec[0] = vec[0] + v.vec[0];
  result.vec[1] = vec[1] + v.vec[1];
  return result;
}

inline Vector2	Vector2::operator - (const Vector2 &v) const {
  Vector2 result;
  result.vec[0] = vec[0] - v.vec[0];
  result.vec[1] = vec[1] - v.vec[1];
  return result;
}

inline Vector2	&Vector2::normalize() {
  return ((*this) /= length());
}

inline Vector2	&Vector2::negate() {
  vec[0] = -vec[0];
  vec[1] = -vec[1];
  return *this;
}

inline Vector2	&Vector2::operator - () {
  return negate();
}

inline Vector2	&Vector2::zero() {
  vec[0] = vec[1] = 0;
  return *this;
}

inline void Vector2::set(float x, float y) {
  vec[0] = x;
  vec[1] = y;
}

inline float& Vector2::operator[] (unsigned int index) {
  return vec[index];
}

inline float Vector2::operator[] (unsigned int index) const {
  return vec[index];
}

inline Vector2::iterator Vector2::begin() {
  return vec;
}

inline const float *Vector2::get() const {
  return vec;
}

inline float Vector2::length() const {
  return sqrt(length2());
}

inline float Vector2::length2() const {
  return (vec[0] * vec[0]) + (vec[1] * vec[1]);
}

inline bool Vector2::operator == (const Vector2 &v) const {
  return (vec[0] == v.vec[0]) && (vec[1] == v.vec[1]);
}

inline bool Vector2::operator != (const Vector2 &v) const {
  return (vec[0] != v.vec[0]) || (vec[1] != v.vec[1]);
}

inline void Vector2::fillGL(GLfloat v[2]) const {
  v[0] = vec[0];
  v[1] = vec[1];
}

inline void Vector2::fromBuffer(Uint8 *v) {
  vec[0] = ((float*)v)[0];
  vec[1] = ((float*)v)[1];
}

inline void Vector2::toBuffer(Uint8 *v) const {
  ((float*)v)[0] = vec[0];
  ((float*)v)[1] = vec[1];
}

inline float Vector2::get1(unsigned int index) const {
  return vec[index];
}

inline void Vector2::set1(unsigned int index, float value) {
  vec[index] = value;
}

inline void Vector2::setv(Vector2 v) {
  *this = v;
}

#endif

/* The End */
