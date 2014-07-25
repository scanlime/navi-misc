/* 
 * Camera.cpp - Scriptable class for camera control
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

#include <GL/glu.h>
#include "Camera.h"
#include "ViewingFrustum.h"

Camera::Camera() {
  forwardThrust = 0;
}

void Camera::setMatrix() {
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50, ((float)viewport[2])/viewport[3], 0.01, 10000);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  glTranslatef(displacement[0], displacement[1], displacement[2]);
  glRotatef(rotation[2], 0,0,1);
  glRotatef(rotation[0], 1,0,0);
  glRotatef(rotation[1], 0,1,0);
  glTranslatef(position[0], position[1], position[2]);

  ViewingFrustum::getInstance()->invalidate();
}

void Camera::animate(float seconds) {
  if (forwardThrust != 0)
    position.acceleration = getForwardVector() * forwardThrust;

  position.animate(seconds);
  rotation.animate(seconds);
  displacement.animate(seconds);
}

Vector3 Camera::getForwardVector(void) {
  Vector3 v;

  /* Start out pointing toward +z */
  v.set(0,0,1);

  v.rotate(rotation[2], 0,0,1);
  v.rotate(rotation[0], 1,0,0);
  v.rotate(rotation[1], 0,1,0);
  return v;
}

Vector3 Camera::getUpVector(void) {
  Vector3 v;

  /* Start out pointing toward +y */
  v.set(0,1,0);

  v.rotate(rotation[2], 0,0,1);
  v.rotate(rotation[0], 1,0,0);
  v.rotate(rotation[1], 0,1,0);
  return v;
}


/* The End */
