/*
 * SceneLights.h - Scene nodes for lights
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

#ifndef _H_SCENELIGHTS
#define _H_SCENELIGHTS

#include "Scene.h"

/* Base class for all scene lights */
class SceneLight : public TransformedSceneNode {
 public:

  /* Attributes:
   *   Red, Green, Blue - light color (float)
   *   Intensity        - Multiplied by light color (float)
   */

  /* JetCOW hooks - Never use these directly! */
  SceneLight(JetCOW *cow, Sint32 id, const char *type="SceneLight");
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  

 protected:
  float color[3];
  float intensity;
};


class DynamicPointLight : public SceneLight {
 public:
  virtual void push(void);
  virtual void pop(void);

  /* JetCOW hooks - Never use these directly! */
  DynamicPointLight(JetCOW *cow, Sint32 id, const char *type="DynamicPointLight");
};

#endif /* _H_SCENELIGHTS */

/* The End */
