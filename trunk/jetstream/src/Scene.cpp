/*
 * Scene.cpp - Implementation for the global scene manager
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

#include "Scene.h"
#include "Skybox.h"
#include "ViewingFrustum.h"
#include "JetCOWRegistry.h"

void SceneNode::attachChild(SceneNode *child) {
  child->ref();
  children->vec.push_back(child);
  children->dirty = true;
  child->childListLocation = children->vec.end();
  child->childListLocation--;
}

void SceneNode::removeChild(SceneNode *child) {
  children->vec.erase(child->childListLocation);
  children->dirty = true;
  child->unref();
}

/* Set up the matrix and draw/animate this node and all children.
 * No animation is performed if dt==0. Note that some scene nodes rely
 * on the matrix being set correctly during animate().
 */
void SceneNode::visit(float dt, bool drawNode) {
  push();
  if (dt>0.0)
    animate(dt);
  if (drawNode)
    draw();
  /* We've already checked to make sure everything here is castable to SceneNode* */
  for (std::vector<JetCOWObject *>::iterator i=children->vec.begin(); i!=children->vec.end(); i++)
    ((SceneNode*)(*i))->visit(dt,drawNode);
  pop();
}

void SceneNode::push(void) {}
void SceneNode::pop(void) {}
void SceneNode::draw(void) {}
void SceneNode::animate(float dt) {}

SceneNode::SceneNode(JetCOW *cow, Sint32 id, const char *type) : JetCOWDictionary(cow,id,type) {
  /* Defaults for new objects */
  if (id<0) {
    children = (JetCOWVector*) cow->checkout("JetCOWVector");
    setAttr("Children", children->id);
  }
  else {
    int childId = getAttrInt("Children");
    children = (JetCOWVector*) cow->checkout("JetCOWVector", childId);

    /* Make sure everything in the vector is castable to SceneNode */
    for (std::vector<JetCOWObject *>::iterator i=children->vec.begin(); i!=children->vec.end(); i++)
      JetCast<SceneNode>("SceneNode",*i);
  }
}

SceneNode::~SceneNode() {
  children->unref();
}

Scene::Scene(JetCOW *cow, Sint32 id, const char *type) : 
  SceneNode(cow,id,type),
  mainHandler(this,Engine::priority_drawScene, &Scene::mainloopIteration) {
  wireframe = false;
  disableBackfaceCull = false;
  bgShade = 0.3f;
}

Scene::~Scene() {}

void Scene::push(void) {
  camera.setMatrix();
  
  glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
  if (wireframe)
    glClearColor(bgShade,bgShade,bgShade,0.0f);
  glClear((wireframe ? GL_COLOR_BUFFER_BIT : 0) | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);
  if (disableBackfaceCull)
    glDisable(GL_CULL_FACE);
  else 
    glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

void Scene::pop(void) {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
}

void Scene::mainloopIteration(void) {
  visit(Engine::getInstance()->frameTime);
}

void Scene::animate(float dt) {
  camera.animate(dt);
}

void TransformedSceneNode::push(void) {
  glPushMatrix();
  glTranslatef(translation[0], translation[1], translation[2]);
  ViewingFrustum::getInstance()->invalidate();
}

void TransformedSceneNode::pop(void) {
  glPopMatrix();
  ViewingFrustum::getInstance()->invalidate();
}

TransformedSceneNode::TransformedSceneNode(JetCOW *cow, Sint32 id,  const char *type)
  : SceneNode(cow,id,type) {
  if (id<0) {
    /* Defaults */
    setAttr("tx", 0.0f);
    setAttr("ty", 0.0f);
    setAttr("tz", 0.0f);
  }
}

void TransformedSceneNode::loadCachedValues(void) {
  translation[0] = getAttrFloatProtected("tx");
  translation[1] = getAttrFloatProtected("ty");
  translation[2] = getAttrFloatProtected("tz");
}

void TransformedSceneNode::saveCachedValues(void) {
  setAttrProtected("tx", translation[0]);
  setAttrProtected("ty", translation[1]);
  setAttrProtected("tz", translation[2]);
}

/* The End */
