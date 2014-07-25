/*
 * SplineSceneNode.cpp - Spline display
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

#include "SplineSceneNode.h"

SplineSceneNode::SplineSceneNode(JetCOW *cow, Sint32 id, const char *type) : SceneNode(cow, id, type) {
  JetCOWDictionary *sphereGen = cow->newDict("SphereGenerator");
  JetCOWDictionary *colorModu = cow->newDict("ColorModulator");
  sphereGen->setAttrFloat("Radius", 0.25f);
  colorModu->setAttrFloat("Red", 1.0f);
  colorModu->setAttrFloat("Green", 0.0f);
  colorModu->setAttrFloat("Blue", 0.0f);
  colorModu->setAttrInt("Parent", sphereGen->id);
  for(int i = 0; i < 4; i++) {
    children[i] = cow->newSceneNode("Surface");
    children[i]->setAttrInt("Generator", colorModu->id);
    attachChild(children[i]);
  }
}

SplineSceneNode::~SplineSceneNode() {
}

void SplineSceneNode::draw() {
  /* spline in blue */
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);
  glColor3f(0.0f, 0.0f, 1.0f);
  glBegin(GL_LINE_STRIP); {
    for(float i = 0; i < 1.0f; i += 0.01f) {
      glVertex3fv(s.compute(i).get());
    }
  } glEnd();
  glEnable(GL_CULL_FACE);
  /* control points drawn as children */
}

void SplineSceneNode::set1(unsigned int which, unsigned int index, float value) {
  s.set1(which, index, value);
  update();
}

float SplineSceneNode::get1(unsigned int which, unsigned int index) {
  return s.get1(which, index);
  update();
}

void SplineSceneNode::setv1(unsigned int which, Vector3 v) {
  s.setv1(which, v);
  update();
}

void SplineSceneNode::update() {
  for(int i = 0; i < 4; i++) {
    children[i]->setAttrFloat("tx", s.p[i][0]);
    children[i]->setAttrFloat("ty", s.p[i][1]);
    children[i]->setAttrFloat("tz", s.p[i][2]);
  }
}
