/*
 * MathT.cpp - Math tables
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

#include "MathT.h"
#include <math.h>
#include <iostream>

Math* Math::singleton = 0;

Math::Math() : cost(0), sint(0) {
  resolution = 1048576;	// 100 steps per hemicircle
  scale = 524288.0f / M_PI;
  recompute();
  singleton = this;
}

Math::Math(int resolution) : cost(0), sint(0) {
  this->resolution = resolution;
  scale = ((float) resolution / 2.0) / M_PI;
  recompute();
  singleton = this;
}

float Math::cos(float n) {
  Math *m = Math::getInstance();
  n *= m->scale;
  return m->cost[(int) n];
}

float Math::sin(float n) {
  Math *m = Math::getInstance();
  n *= m->scale;
  return m->sint[(int) n];
}

Math *Math::getInstance() {
  if(singleton == 0) {
    singleton = new Math;
  }
  return singleton;
}

void Math::recompute() {
  if(cos != 0)
    delete [] cost;
  if(sin != 0)
    delete [] sint;
  cost = new float[resolution];
  sint = new float[resolution];
  memset(cost, 0, resolution * sizeof(float));
  memset(sint, 0, resolution * sizeof(float));
  for(int i = 0; i < resolution; i++) {
    float angle = ((float) i) / scale;
    cost[i] = cosf(angle);
    sint[i] = sinf(angle);
//    std::cout << "cos(" << angle << ") = " << cost[i] << std::endl;
//    std::cout << "sin(" << angle << ") = " << sint[i] << std::endl;
  }
}
