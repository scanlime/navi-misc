/*
 * ParticleSystem.h - Interface for a generic particle system
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

#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include "Util.h"
#include "Scene.h"
#include "Vector.h"
#include <GL/gl.h>
#include <vector>

class ParticleSystem;

struct Particle {
  // this structure is designed to be tightly packed in an array
  // for use with the vertex/color/texcoord array functions.

  GLfloat	texcoord1[2];
  GLfloat	color1[4];
  GLfloat	normal1[3];
  GLfloat	vertex1[3];
  GLfloat	position[3];

  GLfloat	texcoord2[2];
  GLfloat	color2[4];
  GLfloat	normal2[3];
  GLfloat	vertex2[3];
  GLfloat	velocity[3];

  GLfloat	texcoord3[2];
  GLfloat	color3[4];
  GLfloat	normal3[3];
  GLfloat	vertex3[3];
  GLfloat	alive;			// floats make the best bools
  GLfloat	ttl;
  GLfloat	size;
} STRUCT_PACK;

class ParticleAffector {
  public:
    				ParticleAffector();
    virtual			~ParticleAffector();

    virtual void		affect(ParticleSystem *system, float time) = 0;
};

class ParticleEmitter {
  public:
  				ParticleEmitter();
    virtual			~ParticleEmitter();

    void			create(ParticleSystem *system, float time);

    float			position[3];
    float			rate;
    float			direction[3];
    float			up[3];
    float			minSpeed;
    float			maxSpeed;
    float			minTTL;
    float			maxTTL;
    float			angle;
    float			colorStart[4];
    float			colorEnd[4];

  protected:
    virtual void		createParticle(Particle *p) = 0;
    void			createPosition(GLfloat position[3]);
    void			createVelocity(GLfloat velocity[3]);
    void			createColor(GLfloat color[4]);
    void			createColorSame(GLfloat color1[4], GLfloat color2[4], GLfloat color3[4]);
    void			createTTL(GLfloat &ttl);
    unsigned short		createConstantEmissionCount(float time);

    Vector3			randomDeviant(Vector3 direction, float ang);
};

class ParticleSystem : public SceneNode {
  public:
    virtual void		animate(float dt);
    virtual void		draw();

    void			attach(ParticleAffector *affector);
    void			attach(ParticleEmitter *emitter);

    bool			findEmptySlot(Particle *&p);

    /* Attributes:
     *     Size            - Number of particles 
     *     BillboardMethod - A string, "Spherical" or "Cylindrical"
     */

    Particle			*particles;
    int				size;        /* Read-only, the size can be changed
					      * by setting the "Size" attribute */

    /* JetCOW hooks - Never use these directly! */
    ParticleSystem(JetCOW *cow, Sint32 id, const char *type="ParticleSystem");
    virtual ~ParticleSystem();  
    virtual void loadCachedValues(void);
    virtual void saveCachedValues(void);

  private:
    std::vector<ParticleAffector*>	affectors;
    std::vector<ParticleEmitter*>	emitters;

    void			recalculatePolygons();

    enum {Spherical, Cylindrical}	billboardmethod;
};

class ParticleEmitterPoint : public ParticleEmitter {
  public:
  				ParticleEmitterPoint();
    virtual			~ParticleEmitterPoint();

  protected:
    virtual void		createParticle(Particle *p);
};

class ParticleAffectorLinearForce : public ParticleAffector {
  public:
  				ParticleAffectorLinearForce();
    virtual			~ParticleAffectorLinearForce();

    virtual void		affect(ParticleSystem *system, float time);

    float			force[3];
};

class ParticleAffectorColorFader : public ParticleAffector {
  public:
  				ParticleAffectorColorFader();
    virtual			~ParticleAffectorColorFader();

    virtual void		affect(ParticleSystem *system, float time);
};

#endif /* __PARTICLE_SYSTEM_H__ */

/* The End */
