/*
 * SimpleGame.h - Simple OpenGL and SDL game framework
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

#include <FieldSensor.h>
#include <Vector.h>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
using namespace std;

#ifndef __H_SIMPLEGAME
#define __H_SIMPLEGAME

class SimpleGame;

struct Rect {
  Vector2 topLeft, size;
};


class GameObject {
 public:
  GameObject(class SimpleGame *game);

  void draw(void);
  void animate(float dt);
  Rect getBoundingBox(void);
  void collision(GameObject *object);

  SimpleGame* game;
};


class SimpleCamera {
 public:
  void setMatrix(void);

  Vector3 position;
  Vector3 angles;
};


class SimpleGame {
 public:
  SimpleGame(void);
  void run(void);

  SimpleCamera cam;
  vector<GameObject> objects;
  FieldSensor sensor;
  SDL_Surface *surface;
  bool running;
};

#endif /* __H_SIMPLEGAME */

/* The End */

  
