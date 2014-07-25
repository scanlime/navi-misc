/*
 * ParticleSystem.cpp - Implementation for a generic particle system
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

#include <string.h>
#include <string>
#include <iostream>
#include "ParticleSystem.h"
#include "Util.h"
#include "Vector.h"
#include "Engine.h"
#include "Quat.h"

ParticleAffector::ParticleAffector() {
}

ParticleAffector::~ParticleAffector() {
}

ParticleEmitter::ParticleEmitter() {
  position[0] = position[1] = position[2] = 0;
  rate = 0;
  direction[0] = direction[1] = direction[2] = 0;
  up[0] = 0; up[1] = 0; up[2] = 1;
  angle = 0;
  minSpeed = 0;
  maxSpeed = 0;
  minTTL = 0;
  maxTTL = 0;
  colorStart[0] = colorStart[1] = colorStart[2] = colorStart[3] = 0;
  colorEnd[0] = colorEnd[1] = colorEnd[2] = colorEnd[3] = 0;
}

ParticleEmitter::~ParticleEmitter() {
}

void ParticleEmitter::create(ParticleSystem *system, float time) {
  int numParticles = createConstantEmissionCount(time);
  Particle *p;
  for(int i = 0; (i < numParticles) && (system->findEmptySlot(p)); i++) {
    createParticle(p);
  }
}

void ParticleEmitter::createPosition(GLfloat position[3]) {
  position[0] = this->position[0];
  position[1] = this->position[1];
  position[2] = this->position[2];
}

void ParticleEmitter::createVelocity(GLfloat velocity[3]) {
  float speed;
  if(minSpeed != maxSpeed)
    speed = minSpeed + (rand01() * (maxSpeed - minSpeed));
  else
    speed = minSpeed;
  Vector3 vel;
  if(angle == 0) {
    vel = Vector3(direction); vel *= speed;
  } else {
    Vector3 dir(direction);
    vel = randomDeviant(direction, rand01() * angle);
    vel *= speed;
  }
  vel.fillGL(velocity);
}

void ParticleEmitter::createColor(GLfloat color[4]) {
  // red
  if(colorStart[0] != colorEnd[0])
    color[0] = colorStart[0] + (rand01() * (colorEnd[0] - colorStart[0]));
  else
    color[0] = colorStart[0];
  // green
  if(colorStart[1] != colorEnd[1])
    color[1] = colorStart[1] + (rand01() * (colorEnd[1] - colorStart[1]));
  else
    color[1] = colorStart[1];
  // blue
  if(colorStart[2] != colorEnd[2])
    color[2] = colorStart[2] + (rand01() * (colorEnd[2] - colorStart[2]));
  else
    color[2] = colorStart[2];
  // alpha
  if(colorStart[3] != colorEnd[3])
    color[3] = colorStart[3] + (rand01() * (colorEnd[3] - colorStart[3]));
  else
    color[3] = colorStart[3];
}

void ParticleEmitter::createColorSame(GLfloat color1[4], GLfloat color2[4], GLfloat color3[4]) {
  // red
  if(colorStart[0] != colorEnd[0])
    color1[0] = color2[0] = color3[0] = colorStart[0] + (rand01() * (colorEnd[0] - colorStart[0]));
  else
    color1[0] = color2[0] = color3[0] = colorStart[0];
  // green
  if(colorStart[1] != colorEnd[1])
    color1[1] = color2[1] = color3[1] = colorStart[1] + (rand01() * (colorEnd[1] - colorStart[1]));
  else
    color1[1] = color2[1] = color3[1] = colorStart[1];
  // blue
  if(colorStart[2] != colorEnd[2])
    color1[2] = color2[2] = color3[2] = colorStart[2] + (rand01() * (colorEnd[2] - colorStart[2]));
  else
    color1[2] = color2[2] = color3[2] = colorStart[2];
  // alpha
  if(colorStart[3] != colorEnd[3])
    color1[3] = color2[3] = color3[3] = colorStart[3] + (rand01() * (colorEnd[3] - colorStart[3]));
  else
    color1[3] = color2[3] = color3[3] = colorStart[3];
}

void ParticleEmitter::createTTL(GLfloat &ttl) {
  if(minTTL != maxTTL)
    ttl = minTTL + (rand01() * (maxTTL - minTTL));
  else
    ttl = minTTL;
}

unsigned short ParticleEmitter::createConstantEmissionCount(float time) {
  static float remainder = 0;
  unsigned short request;

  remainder += rate * time;
  request = (unsigned short) remainder;
  remainder -= request;
  return request;
}

ParticleSystem::ParticleSystem(JetCOW *cow, Sint32 id, const char *type) : SceneNode(cow,id,type) {
  particles = NULL;
  size = 0;

  if (id<0) {
    /* Defaults for new objects */
    setAttr("size", 0);
    setAttr("BillboardMethod", "Spherical");
  }
}

Vector3 ParticleEmitter::randomDeviant(Vector3 direction, float ang) {
  Vector3 up = direction.perpendicular();
  Vector3 up2;
  Quat q;
  q.fromAxis(rand01() * M_PI * 2, direction);
  up2 = q * up;
  q.fromAxis(ang, up2);
  return (q * direction);
}

void ParticleSystem::loadCachedValues(void) {
 int newSize = getAttrIntProtected("Size");

 if (newSize != size) {
   size = newSize;
   if (particles)
     delete [] particles;
   if (size) {
     particles = new Particle[size];
     bzero(particles, sizeof(Particle) * size);
   }
   else
     particles = NULL;
 }

 std::string billboardMethodStr = getAttrStrProtected("BillboardMethod");

 if (billboardMethodStr == "Cylindrical")
   billboardmethod = Cylindrical;
 else
   billboardmethod = Spherical;
}

void ParticleSystem::saveCachedValues(void) {
  setAttrProtected("Size", size);

  switch (billboardmethod) {
  case Spherical:
    setAttrProtected("BillboardMethod", "Spherical");
    break;
  case Cylindrical:
    setAttrProtected("BillboardMethod", "Cylindrical");
    break;
  }  
}

ParticleSystem::~ParticleSystem() {
  if (particles)
    delete [] particles;
}

void ParticleSystem::animate(float dt) {
  {
  std::vector<ParticleAffector*>::iterator it = affectors.begin();
  for(; it != affectors.end(); it++)
    (*it)->affect(this, dt);
  }
  {
  std::vector<ParticleEmitter*>::iterator it = emitters.begin();
  for(; it != emitters.end(); it++)
    (*it)->create(this, dt);
  }
  for(int i = 0; i < size; i++) {
    if(particles[i].alive != 0.0f) {
      particles[i].ttl -= dt;
      if(particles[i].ttl <= 0.0f)
	bzero(&particles[i], sizeof(Particle));
    }
    if(particles[i].alive != 0.0f) {
      Vector3 poschange(particles[i].velocity); poschange *= dt;
      Vector3 position(particles[i].position); position += poschange;
      position.fillGL(particles[i].position);
    }
  }
}

void ParticleSystem::draw() {
  recalculatePolygons();

  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  glInterleavedArrays(GL_T2F_C4F_N3F_V3F, sizeof(Particle) / 3, particles);
  glDrawArrays(GL_TRIANGLES, 0, size * 3);
  glDisable(GL_COLOR_MATERIAL);
}

void ParticleSystem::attach(ParticleAffector *affector) {
  affectors.push_back(affector);
}

void ParticleSystem::attach(ParticleEmitter *emitter) {
  emitters.push_back(emitter);
}

bool ParticleSystem::findEmptySlot(Particle *&p) {
  for(int i = 0; i < size; i++) {
    if(particles[i].alive == 0.0f) {
      p = &(particles[i]);
      return true;
    }
  }
  return false;
}

void ParticleSystem::recalculatePolygons() {
  Vector3 up, right;
  switch(billboardmethod) {
    case Spherical:
      {
	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	up = Vector3(modelview[1], modelview[5], modelview[9]);
	right = Vector3(modelview[0], modelview[4], modelview[8]);
      }
      break;
    case Cylindrical:
      {
	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	up = Vector3(0.0f, 1.0f, 0.0f);
	right = Vector3(modelview[0], modelview[4], modelview[8]);
      }
      break;
    default:
      std::cerr << "ERROR - billboarding method unset\n";
      exit(1);
  }
  Vector3 t1, t2, t3, v1, v2, v3;
  t1 = (-right * 0.866f) - (up * 0.5f);
  t2 = (right * 0.866f) - (up * 0.5f);
  t3 = up;
  for(int i = 0; i < size; i++) {
    if(particles[i].alive != 0.0f) {
      Vector3 p(particles[i].position);
      v1 = (-(right * particles[i].size) * 0.866f) - ((up * particles[i].size) * 0.5f) + p;
      v2 = ((right * particles[i].size) * 0.866f) - ((up * particles[i].size) * 0.5f) + p;
      v3 = (up * particles[i].size) + p;
      v1.fillGL(particles[i].vertex1);
      v2.fillGL(particles[i].vertex2);
      v3.fillGL(particles[i].vertex3);
    }
  }
}

ParticleEmitterPoint::ParticleEmitterPoint() {
}

ParticleEmitterPoint::~ParticleEmitterPoint() {
}

void ParticleEmitterPoint::createParticle(Particle *p) {
  createPosition(p->position);
  createVelocity(p->velocity);
  createColorSame(p->color1, p->color2, p->color3);
  createTTL(p->ttl);
  p->alive = 1.0f;
  p->size = 0.1f;
}
  
ParticleAffectorLinearForce::ParticleAffectorLinearForce() {
  force[0] = force[1] = force[2];
}

ParticleAffectorLinearForce::~ParticleAffectorLinearForce() {
}

void ParticleAffectorLinearForce::affect(ParticleSystem *system, float time) {
  float dv[3];
  dv[0] = force[0] * time;
  dv[1] = force[1] * time;
  dv[2] = force[2] * time;
  for(int i = 0; i < system->size; i++) {
    system->particles[i].velocity[0] += dv[0];
    system->particles[i].velocity[1] += dv[1];
    system->particles[i].velocity[2] += dv[2];
  }
}

ParticleAffectorColorFader::ParticleAffectorColorFader() {
}

ParticleAffectorColorFader::~ParticleAffectorColorFader() {
}

void ParticleAffectorColorFader::affect(ParticleSystem *system, float time) {
}

/* The End */
