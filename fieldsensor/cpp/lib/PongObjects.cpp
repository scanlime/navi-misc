/*
 * PongObjects.cpp - Simple game objects used in the pong-like games
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

#include <PongObjects.h>

Paddle::Paddle(class SimpleGame *game, Vector3 origin, Vector3 direction, 
	       float width, float thickness, Vector4 color) : GameObject(game) {
  this->origin = origin;
  this->direction = direction;
  this->width = width;
  this->thickness = thickness;
  this->color = color;
  position = 0;
}

void Paddle::draw(void) {
  Vector3 v;
  Vector3 up = Vector3(0,0,1);
  up %= direction;
  glColor4f(color[0], color[1], color[2], color[3]);
  glBegin(GL_POLYGON);
  v = origin + direction*(position-width/2) + up*(thickness/2);
  glVertex3f(v[0], v[1], v[2]);
  v = origin + direction*(position+width/2) + up*(thickness/2);
  glVertex3f(v[0], v[1], v[2]);
  v = origin + direction*(position+width/2) - up*(thickness/2);
  glVertex3f(v[0], v[1], v[2]);
  v = origin + direction*(position-width/2) - up*(thickness/2);
  glVertex3f(v[0], v[1], v[2]);
  glEnd();
}

bool Paddle::testCollision(Vector3 v1, Vector3 v2) {
}

HumanPaddle::HumanPaddle(class SimpleGame *game, Vector3 origin, Vector3 direction, 
			 float width, float thickness, Vector4 color) 
  : Paddle(game,origin,direction,width,thickness,color) {}

void HumanPaddle::animate(float dt) {
  position = game->sensorInput[0];
}

PlayfieldSegment::PlayfieldSegment(class SimpleGame *game, Vector3 v1, Vector3 v2, Vector4 color) : GameObject(game) {
  this->v1 = v1;
  this->color = color;
  direction = v2-v1;
  length = direction.length();
  direction.normalize();
}

void PlayfieldSegment::draw(void) {
  glColor4f(color[0], color[1], color[2], color[3]);
  for (float i=0;i<length;i+=spacing)
    dot(v1+direction*i);
}

void PlayfieldSegment::dot(Vector3 v) {
  glBegin(GL_QUADS);
  glVertex3f(v[0]-radius/2, v[1]-radius/2, v[2]);
  glVertex3f(v[0]+radius/2, v[1]-radius/2, v[2]);
  glVertex3f(v[0]+radius/2, v[1]+radius/2, v[2]);
  glVertex3f(v[0]-radius/2, v[1]+radius/2, v[2]);
  glEnd();
}

bool PlayfieldSegment::testCollision(Vector3 v1, Vector3 v2) {
}

Ball::Ball(class SimpleGame *game, Vector4 color) : GameObject(game) {
  this->color = color;
  reset();
}

void Ball::reset(void) {
  position.set(0.5,0.5,0.0);
}

void Ball::draw(void) {
  glColor4f(color[0], color[1], color[2], color[3]);
  glBegin(GL_QUADS);
  glVertex3f(position[0]-radius/2, position[1]-radius/2, position[2]);
  glVertex3f(position[0]+radius/2, position[1]-radius/2, position[2]);
  glVertex3f(position[0]+radius/2, position[1]+radius/2, position[2]);
  glVertex3f(position[0]-radius/2, position[1]+radius/2, position[2]);
  glEnd();
}

void Ball::animate(float dt) {
  position += velocity;
}

/* The End */

  
