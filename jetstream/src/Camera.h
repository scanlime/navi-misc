/*
 * Camera.h - Header for scriptable camera control
 *
 *      Note: this class controls camera motion but does not include 
 *            frustum culling or projection, since there may be multiple
 *            cameras but there is only one current OpenGL context.
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

#ifndef _H_CAMERA
#define _H_CAMERA

#include "AnimatedVector.h"

class Camera {
 public:
  Camera();

  virtual void setMatrix(void);
  virtual void animate(float seconds);

  /* If nonzero, thrust automatically applied along the forward vector */
  float forwardThrust;

  Vector3 getForwardVector(void);
  Vector3 getUpVector(void);

  AnimatedVector position;
  AnimatedVector rotation;
  AnimatedVector displacement;

  /* Python helper functions */
  AnimatedVector *getPosition() {return &position;}
  AnimatedVector *getRotation() {return &rotation;}
  AnimatedVector *getDisplacement() {return &displacement;}
};

#endif /* _H_CAMERA */

/* The End */
