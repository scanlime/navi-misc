/*
 * SoundManagerNew.h - Sound manager class based on OpenAL
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

#ifndef H_SOUNDMANAGER
#define H_SOUNDMANAGER

#include <AL/al.h>
#include <AL/alc.h>
#include <map>
#include <string>
#include "Vector.h"

class SoundManager {
  public:
    SoundManager();
    ~SoundManager();

    bool		LoadSample(std::string filename);
    bool		FreeSample(std::string name);

    int			PlaySound(std::string name, float vol = 0.75f);
    int			PlaySound(std::string name, Vector3 pos, bool loop = false);
    int			PlaySound(std::string name, Vector3 pos, Vector3 vel, bool loop = false);
    bool		StopSound(int id);
    bool		PauseSound(int id);
    bool		ContinueSound(int id);

    void		SetMasterVolume(float vol);
    float		GetMasterVolume();

    bool		PauseAll();
    bool		ContinueAll();
    bool		Paused();

    void		SetListener(Vector3 position, Vector3 velocity, Vector3 orientation);
    void		SetListenerPos(Vector3 position);
    void		SetListenerVel(Vector3 velocity);
    void		SetListenerOri(Vector3 orientation);
    Vector3		GetListenerPos();
    Vector3		GetListenerVel();
    Vector3		GetListenerOri();

    void		PlayBGM();
    void		PauseBGM();
    void		StopBGM();
    void		ContinueGBM();

    void		Update();

  private:
    struct sample {
      ALsizei		size;
      ALsizei		bits;
      ALsizei		freq;
      ALsizei		format;
      void		*wave;
      ALuint		buffer;
      ALuint		id;
    };
    struct bgm {
      std::string	name;
      std::string	file;
      bool		playing;
      ALuint		buffers[2];
    };
    struct entity {
      std::string	sound;
      bool		loop;
      bool		paused;
      float		volume;
      float		pan;
      bool		is3d;
      Vector3		position;
      Vector3		velocity;
      Vector3		dist;
    };

    std::map<std::string, sample>	samples;
    std::map<int,entity>		entities;

    bool		running;
    bool		paused;
    ALCdevice		*device;
    ALvoid		*context;
    int			current;

    typedef ALboolean (vorbisLoader)(ALuint, ALvoid *, ALint);
    vorbisLoader	*alutLoadVorbisp;
};

#endif

/* The End */
