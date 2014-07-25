/*
 * AnimatedVector.cpp - A scriptable vector supporting animation
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

#include "AnimatedVector.h"

AnimatedVector::AnimatedVector() {
  thrust = 400;
  friction = 7.3;
  hysteresis = 2.4;
}

void AnimatedVector::animate(float frameTime) {
  /* Integrate velocity and acceleration */
  *this += velocity * frameTime;
  velocity += acceleration * frameTime;

  /* Fluid friction */
  velocity += velocity * (-friction * frameTime);

  /* Get the distance and direction to our target. To simulate
   * the motion of a real object, we only directly set the acceleration.
   *
   * This algorithm accelerates the vector toward its target until it reaches
   * the point where it could complete the journey by coasting, then it coasts.
   */
  Vector3 trajectory = target;
  trajectory -= *this;
  float targetDistance = trajectory.length();
  float speed = velocity.length();
  if (targetDistance > 0 && thrust > 0) {
    float stoppingDistanceDenominator = acceleration.length() - speed*friction;
    float stoppingDistance = stoppingDistanceDenominator ? (speed*speed / stoppingDistanceDenominator) : 0;
    if (targetDistance > hysteresis && targetDistance > stoppingDistance)
      acceleration = trajectory.normalize() * thrust;
    else
      acceleration.zero();
  }
}

/* The End */
