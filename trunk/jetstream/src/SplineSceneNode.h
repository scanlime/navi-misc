/*
 * SplineSceneNode.h - Spline display
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

#ifndef _H_SPLINESCENENODE
#define _H_SPLINESCENENODE

#include "Spline.h"
#include "Scene.h"
#include "Surface.h"

class SplineSceneNode : public SceneNode {
 public:
  		SplineSceneNode(JetCOW *cow, Sint32 id=0, const char *type="Spline");
  virtual	~SplineSceneNode();

  virtual void	draw();

  void		set1(unsigned int which, unsigned int index, float value);
  float		get1(unsigned int which, unsigned int index);
  void		setv1(unsigned int which, Vector3 v);

 private:
  void		update();
  Spline3	s;
  SceneNode	*children[4];
};

#endif
