/*
 * JetCOWRegistry.cpp - A registry of data types used by JetCOW
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

#include "JetCOWRegistry.h"
JetCOWRegistry *JetCOWRegistry::instance = NULL;

#include "JetCOW.h"
#include "Lightmap.h"
#include "Surface.h"
#include "SphereGenerator.h"
#include "SuperquadricGenerator.h"
#include "TriangleGenerator.h"
#include "QuadGenerator.h"
#include "Scene.h"
#include "ParticleSystem.h"
#include "Skybox.h"
#include "PerlinDisplacement.h"
#include "SceneLights.h"
#include "ColorModulator.h"
#include "SurfaceInverter.h"
#include "SplineSceneNode.h"

JetCOWRegistry::JetCOWRegistry() {
  /* All objects in the library subclassed from JetCOWObject must be added here.
   * The first parameter is mandatory, the object type as a string.
   * The second is optional, the class's parent. Currently JetCOW doesn't support multiple inheritance.
   * The template parameter is the class name used for instantiating the class. If the class
   * can't be instantiated directly (it's an abstract base class for example) you can substitute
   * JetCOWObject for this.
   */

  add<JetCOWObject>("JetCOWObject");
  add<JetCOWDictionary>("JetCOWDictionary" ,"JetCOWObject");
  add<JetCOWVector>("JetCOWVector" ,"JetCOWObject");

  add<Lightmap>("Lightmap", "JetCOWObject");

  add<SceneNode>("SceneNode", "JetCOWDictionary");
  add<Scene>("Scene", "SceneNode");
  add<TransformedSceneNode>("TransformedSceneNode", "SceneNode");

  add<SceneLight>("SceneLight", "TransformedSceneNode");
  add<DynamicPointLight>("DynamicPointLight", "SceneLight");

  add<Surface>("Surface", "TransformedSceneNode");
  add<JetCOWObject>("SurfaceGenerator", "JetCOWDictionary");
  add<SurfaceModifier>("SurfaceModifier", "SurfaceGenerator");

  add<PerlinDisplacement>("PerlinDisplacement", "SurfaceModifier");
  add<SurfaceInverter>("SurfaceInverter", "SurfaceModifier");
  add<ColorModulator>("ColorModulator", "SurfaceModifier");

  add<SphereGenerator>("SphereGenerator", "SurfaceGenerator");
  add<SuperellipsoidGenerator>("SuperellipsoidGenerator", "SurfaceGenerator");
  add<TriangleGenerator>("TriangleGenerator", "SurfaceGenerator");
  add<QuadGenerator>("QuadGenerator", "SurfaceGenerator");
  
  add<ParticleSystem>("ParticleSystem", "SceneNode");
  add<Skybox>("Skybox", "SceneNode");
  add<SplineSceneNode>("Spline", "SceneNode");
}
 
bool JetCOWRegistry::isSubclass(const std::string &a, const std::string &b) {
  std::vector<std::string> s = typeMap[b]->subclasses;
  bool result = find(s.begin(), s.end(), a) != s.end();
  return result;
}

JetCOWObject *JetCOWRegistry::createObject(JetCOW *cow, Sint32 id, const std::string &type) {
  JetCOWObjectTypeBase* tb = typeMap[type];
  if (!tb)
    throw std::runtime_error("Attempt to create a nonexistant JetCOW object type"); 
  return tb->instantiate(cow,id);
}

JetCOWRegistry *JetCOWRegistry::getInstance(void) {
  if (!instance)
    instance = new JetCOWRegistry;
  return instance;
}

/* The End */
