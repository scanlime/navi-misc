/*
 * SceneLights.cpp - Scene nodes for lights
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

#include "SceneLights.h"

SceneLight::SceneLight(JetCOW *cow, Sint32 id, const char *type) :
  TransformedSceneNode(cow,id,type) {
  if (id<0) {
    setAttr("Red",  1.0f);
    setAttr("Green",1.0f);
    setAttr("Blue", 1.0f);
    setAttr("Intensity", 1.0f);
  }
}

void SceneLight::loadCachedValues(void) {
  TransformedSceneNode::loadCachedValues();
  color[0] = getAttrFloatProtected("Red");
  color[1] = getAttrFloatProtected("Green");
  color[2] = getAttrFloatProtected("Blue");
  intensity = getAttrFloatProtected("Intensity");
}

void SceneLight::saveCachedValues(void) {
  TransformedSceneNode::saveCachedValues();
  setAttrProtected("Red", color[0]);
  setAttrProtected("Green", color[1]);
  setAttrProtected("Blue", color[2]);
  setAttrProtected("Intensity", intensity);
}

DynamicPointLight::DynamicPointLight(JetCOW *cow, Sint32 id, const char *type) :
  SceneLight(cow,id,type) {}

void DynamicPointLight::push(void) {
  TransformedSceneNode::push();

  /* FIXME: This is just temporary code so we can use OpenGL's lights
   *        in scenes for testing. This will be replaced with something
   *        better later.
   */

  float zeros[] = {0,0,0,1};
  float diffuse[]  = {color[0]*intensity,
		      color[1]*intensity,
		      color[2]*intensity,0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, zeros);
  glLightfv(GL_LIGHT0, GL_SPECULAR, zeros);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  zeros);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zeros);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zeros);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zeros);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, zeros);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
}

void DynamicPointLight::pop(void) {
  TransformedSceneNode::pop();
}

/* The End */
