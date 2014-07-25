/*
 * ColorModulator.cpp - Surface modifier for modulating the vertex coloring of a surface
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

#include "ColorModulator.h"

void ColorModulator::f(SurfacePoint &p, SurfaceQuadtreeNode *qn) {
  p.color[0] *= color[0];
  p.color[1] *= color[1];
  p.color[2] *= color[2];
}

ColorModulator::ColorModulator(JetCOW *cow, Sint32 id,  const char *type) :
  SurfaceModifier(cow,id,type) {
  if (id<0) {
    setAttr("Red", 1.0f);
    setAttr("Green", 1.0f);
    setAttr("Blue", 1.0f);
  }
}

void ColorModulator::loadCachedValues(void) {
  SurfaceModifier::loadCachedValues();
  color[0] = getAttrFloatProtected("Red");
  color[1] = getAttrFloatProtected("Green");
  color[2] = getAttrFloatProtected("Blue");
  dirty = true;
}

void ColorModulator::saveCachedValues(void) {
  SurfaceModifier::saveCachedValues();
  setAttrProtected("Red", color[0]);
  setAttrProtected("Green", color[1]);
  setAttrProtected("Blue", color[2]);
}

/* The End */
