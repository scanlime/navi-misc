/*
 * SimpleGame.cpp - Simple OpenGL and SDL game framework
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

GameObject::GameObject(class SimpleGame *game) {
  this->game = game;
}

void GameObject::draw(void) {}

void GameObject::animate(float dt) {}

/* Return true if the given line segment intersects this object */
bool GameObject::testCollision(Vector3 v1, Vector3 v2) {
  return false;
}

bool GameObject::collision(GameObject *object) {
  return true;
}

SimpleCamera::SimpleCamera(void) {
  position.set(-0.5, -0.3, -1.1);
  angles.set(-45,0,0);
}

void SimpleCamera::setMatrix(void) {
  glLoadIdentity();
  glTranslatef(position[0], position[1], position[2]);
  glRotatef(angles[2], 0,0,1);
  glRotatef(angles[0], 1,0,0);
  glRotatef(angles[1], 0,1,0);
}

SimpleGame::SimpleGame(void) {
  running = false;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *surface = SDL_SetVideoMode(1024,768,0,SDL_OPENGL);

  glEnable(GL_BLEND);
  glEnable(GL_LINE_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);
  glDepthFunc(GL_LEQUAL);

  glViewport(0, 0, surface->w, surface->h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1.0*surface->w/surface->h, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void SimpleGame::run(void) {
  Uint32 oldTicks = SDL_GetTicks();
  running = true;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      switch (event.type) {
	
      case SDL_QUIT:
	running = false;
	break;
	
      }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam.setMatrix();

    Uint32 newTicks = SDL_GetTicks();
    float dt = (newTicks - oldTicks) / 1000.0;
    oldTicks = newTicks;

    sensorInput = sensor.getPosition();

    for (vector<GameObject*>::iterator i=objects.begin(); i!=objects.end(); i++)
      (*i)->animate(dt);

    for (vector<GameObject*>::iterator i=objects.begin(); i!=objects.end(); i++)
      (*i)->draw();

    SDL_GL_SwapBuffers();
  }

  return;
}

/* The End */

  
