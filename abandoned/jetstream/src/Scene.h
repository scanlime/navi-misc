/*
 * Scene.h - Interface for the global scene manager
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

#ifndef _H_SCENE
#define _H_SCENE

#include <vector>
#include "Engine.h"
#include "Camera.h"
#include "Mutex.h"
#include "JetCOW.h"

class Scene;

class SceneNode : public JetCOWDictionary {
 public:

  virtual void attachChild(SceneNode *child);
  virtual void removeChild(SceneNode *child);

  /* Set up the matrix and draw/animate this node and all children.
   * No animation is performed if dt==0. Note that some scene nodes rely
   * on the matrix being set correctly during animate().
   */
  void visit(float dt=0, bool drawNode=true);

  /* Functions called when entering and leaving this node */
  virtual void push(void);
  virtual void pop(void);

  /* Hooks for subclasses to implement */
  virtual void draw(void);
  virtual void animate(float dt);

  /* JetCOW hooks - Never use these directly! */
  SceneNode(JetCOW *cow, Sint32 id, const char *type="SceneNode");
  virtual ~SceneNode();  

 protected:
  JetCOWVector *children;     /* ID stored in "Children" attribute */
  std::vector<JetCOWObject *>::iterator childListLocation;
};


/* Root scene node, provides the camera and a main loop handler */
class Scene : public SceneNode {
 public:
  
  /* Set up camera and OpenGL context */
  virtual void push(void);
  virtual void pop(void);

  /* The camera this scene is drawn with */
  Camera camera;
  Camera *getCamera() {return &camera;}
  virtual void animate(float dt);

  /* Non-persistent rendering settings */
  bool wireframe;
  float bgShade;
  bool disableBackfaceCull;

  /* JetCOW hooks - Never use these directly! */
  Scene(JetCOW *cow, Sint32 id, const char *type="Scene");
  virtual ~Scene();

 private:
  MainLoopHandler<Scene> mainHandler;
  void mainloopIteration(void);
};


/* Add a rotation and translation to the basic SceneNode */
class TransformedSceneNode : public SceneNode {
 public:

  virtual void push(void);
  virtual void pop(void);

  /* The following JetCOW attributes are supported:
   *  tx,ty,tz - Translation vector
   */

  Vector3 translation;

  /* JetCOW hooks - never call directly */
  TransformedSceneNode(JetCOW *cow, Sint32 id,  const char *type="TransformedSceneNode");
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  
};


#endif /* _H_SCENE */

/* The End */
