/*
 * PongObjects.h - Simple game objects used in the pong-like games
 *
 * Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
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
 * Contributors:
 * 
 * 
 * 
 */

#include <SimpleGame.h>

#ifndef __H_PONGOBJECTS
#define __H_PONGOBJECTS


class Paddle : public GameObject {
 public:
  Paddle(class SimpleGame *game, Vector3 origin, Vector3 direction, 
	 float width, float thickness, Vector4 color);

  virtual void draw(void);
  virtual bool testCollision(Vector3 v1, Vector3 v2);

 protected:
  Vector3 origin, direction;
  Vector4 color;
  float width, thickness, position;
};


class HumanPaddle : public Paddle {
 public:
  HumanPaddle(class SimpleGame *game, Vector3 origin, Vector3 direction, 
	      float width, float thickness, Vector4 color);

  virtual void animate(float dt);
};


class PlayfieldSegment : public GameObject {
 public:
  PlayfieldSegment(class SimpleGame *game, Vector3 v1, Vector3 v2, Vector4 color);

  virtual void draw(void);
  virtual bool testCollision(Vector3 v1, Vector3 v2);

 private:
  Vector3 v1, direction;
  Vector4 color;
  float length;
  void dot(Vector3 v);
  const static float radius = 0.01;
  const static float spacing = 0.03;
};


class Ball : public GameObject {
 public:
  Ball(class SimpleGame *game, Vector4 color);

  void reset(void);
  virtual void draw(void);
  virtual void animate(float dt);

  Vector3 position, velocity;
  Vector4 color;
  const static float radius = 0.01;
};


#endif /* __H_PONGOBJECTS */

/* The End */

  
