/*
 * Quat.cpp - Quaternion classes
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

#include "Quat.h"
#include "Util.h"
#include "MathT.h"
#include <math.h>

Quat::Quat() : r(0), v(0,0,0) {}
Quat::Quat(float r_) : r(r_), v(0,0,0) {}
Quat::Quat(Vector3 v_) : r(0), v(v_) {}
Quat::Quat(float r_, Vector3 v_) : r(r_), v(v_) {}
Quat::Quat(float r_, float i, float j, float k) : r(r_), v(i,j,k) {}

Quat &Quat::operator = (const Quat &q) {
  r = q.r;
  v = q.v;
}

Quat &Quat::operator += (const Quat &q) {
  r += q.r;
  v += q.v;
}

Quat &Quat::operator -= (const Quat &q) {
  r -= q.r;
  v -= q.v;
}

Quat &Quat::operator *= (const Quat &q) {
  *this = *this * q;
  return *this;
}

Quat &Quat::operator /= (const Quat &q) {
  *this = *this / q;
  return *this;
}

Quat &Quat::operator *= (const float &f) {
  r *= f;
  v *= f;
}

Quat &Quat::operator /= (const float &f) {
  r /= f;
  v /= f;
}

Quat Quat::operator * (const float &f) const {
  return Quat(r*f,v*f);
}

Quat Quat::operator * (const Quat &q) const {
  Quat u;
  u.r = r*q.r - v*q.v;
  u.v = q.v*r + v*q.r + v%q.v;
  return u;
}

Quat Quat::operator / (const Quat &q) const {
  Quat q2 = q;
  q2.inverse();
  return *this * q2;
}

Quat Quat::operator + (const Quat &q) const {
  return Quat(r+q.r,v+q.v);
}

Quat &Quat::negate() {
  r = -r;
  v.negate();
  return *this;
}

Quat &Quat::conjugate() {
  v.negate();
  return *this;
}

Quat &Quat::operator - () {
  return negate();
}

Quat &Quat::zero() {
  r = 0;
  v.zero();
  return *this;
}

Quat &Quat::inverse() {
  float l = magnitude();
  conjugate();
  *this /= l*l;
  return *this;
}

Quat &Quat::normalize() {
  return ((*this) /= magnitude());
}

float Quat::magnitude() const {
  return sqrt(norm());
}

float Quat::norm() const {
  return r*r + v.length();
}

bool Quat::operator == (const Quat &v) const {
  return r==v.r && v==v.v;
}

bool Quat::operator != (const Quat &v) const {
  return r!=v.r || v!=v.v;  
}

Quat Quat::slerp(const Quat &to, float h) {
  float cosine = this->dot(to);
  float angle = acosf(cosine);

  if(absf(angle) < EPSILON)
    return (*this);

  float sine = Math::sin(angle);
  float invsine = 1.0 / sine;
  float coeff0 = Math::sin((1.0 - h) * angle) * invsine;
  float coeff1 = Math::sin(h * angle) * invsine;
  return (((*this) * coeff0) + (to * coeff1));
}

Quat Quat::pow(float p) {
  float theta = p * acos(r);
  // return Quat(cos(theta), v.normal() * sin(theta));
}

float Quat::length() {
  return sqrt((r * r) + v.length2());
}

float Quat::length2() {
  return ((r * r) + v.length2());
}

void Quat::fromAxis(const float &angle, const Vector3 &axis) {
  float halfangle = angle * 0.5f;
  float sine = Math::sin(halfangle);
  r = Math::cos(halfangle);
  v = axis * sine;
}

void Quat::toAxis(float &angle, Vector3 &axis) {
  float lengths = length2();
  if(lengths > 0.0f) {
    angle = 2.0f * acosf(r);
    float invl = 1.0f / (sqrt(lengths));
    axis = v * invl;
  } else {
    angle = 0.0f;
    axis[0] = 1.0f;
    axis[1] = axis[2] = 0.0f;
  }
}

float Quat::dot(const Quat &q) const {
  return ((v[0] * q.v[0]) + (v[1] * q.v[1]) + (v[2] * q.v[2]) + (r * q.r));
}

Vector3 Quat::operator * (const Vector3 &v) const {
  return (toRotationMatrix() * v);
}

Matrix3x3 Quat::toRotationMatrix() const {
  float vec[9];
  float tx = 2.0f * v[0];
  float ty = 2.0f * v[1];
  float tz = 2.0f * v[2];
  float twx = tx * r;
  float twy = ty * r;
  float twz = tz * r;
  float txx = tx * v[0];
  float txy = tx * v[1];
  float txz = tx * v[2];
  float tyy = ty * v[1];
  float tyz = ty * v[2];
  float tzz = tz * v[2];

  vec[0] = 1.0f - (tyy + tzz);
  vec[1] = txy - twz;
  vec[2] = txz + twy;
  vec[3] = txy + twz;
  vec[4] = 1.0f - (txx + tzz);
  vec[5] = tyz - twx;
  vec[6] = txz - twy;
  vec[7] = tyz + twx;
  vec[8] = 1.0f - (txx + tyy);
  return Matrix3x3(vec);
}

/* The End */
