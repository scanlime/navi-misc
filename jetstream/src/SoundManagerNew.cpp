/*
 * SoundManagerNew.cpp - Sound manager class based on OpenAL
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

#include "SoundManagerNew.h"
#include <AL/alut.h>

#define VORBIS_FUNC "alutLoadVorbis_LOKI"

SoundManager::SoundManager() {
  running = true;

  device = alcOpenDevice(NULL);
  if(device == NULL) {
    running = false;
    return;
  }
  context = alcCreateContext(device, NULL);
  if(context == NULL) {
    running = false;
    return;
  }

  alutLoadVorbisp = (vorbisLoader *) alGetProcAddress((ALubyte *) VORBIS_FUNC);
}

SoundManager::~SoundManager() {
  if(running) {
    alcDestroyContext(context);
    alcCloseDevice(device);
  }
}

bool SoundManager::LoadSample(std::string filename) {
  std::string file = filename.substr(0, filename.rfind('.'));
  std::string extension = filename.substr(filename.rfind('.'));
  sample s;
  ALboolean error;

  if(extension == "wav") {
    alGenBuffers(1, &s.buffer);
    error = alutLoadWAV(filename.c_str(), &s.wave, &s.format, &s.size, &s.bits, &s.freq);
    if(error == AL_FALSE) {
      return false;
    }
    alBufferData(s.buffer, s.format, s.wave, s.size, s.freq);
    free(s.wave);
  } else if(extension == "ogg" && (alutLoadVorbisp != NULL)) {
  } else {
    return false;
  }
  samples[file] = s;
  return true;
}

bool SoundManager::FreeSample(std::string name) {
  sample &s = samples[name];
  alDeleteBuffers(1, &s.buffer);
}

int SoundManager::PlaySound(std::string name, float vol) {
}

int SoundManager::PlaySound(std::string name, Vector3 pos, bool loop) {
}

int SoundManager::PlaySound(std::string name, Vector3 pos, Vector3 vel, bool loop) {
}

bool SoundManager::StopSound(int id) {
}

bool SoundManager::PauseSound(int id) {
}

bool SoundManager::ContinueSound(int id) {
}

void SoundManager::SetMasterVolume(float vol) {
  alListenerf(AL_GAIN, vol);
}

float SoundManager::GetMasterVolume() {
  float result;
  alGetListenerf(AL_GAIN, &result);
  return result;
}

bool SoundManager::PauseAll() {
  if(paused)
    return false;
}

bool SoundManager::ContinueAll() {
  if(!paused)
    return false;
}

bool SoundManager::Paused() {
  return paused;
}

void SoundManager::SetListener(Vector3 position, Vector3 velocity, Vector3 orientation) {
}

void SoundManager::SetListenerPos(Vector3 position) {
  alListenerfv(AL_POSITION, (ALfloat*) position.get());
}

void SoundManager::SetListenerVel(Vector3 velocity) {
  alListenerfv(AL_VELOCITY, (ALfloat*) velocity.get());
}

void SoundManager::SetListenerOri(Vector3 orientation) {
  // FIXME - 6-tuple
//  alListenerfv(AL_POSITION, position.get());
}

Vector3 SoundManager::GetListenerPos() {
  float position[3];
  alGetListenerfv(AL_POSITION, position);
  return Vector3(position);
}

Vector3 SoundManager::GetListenerVel() {
  float velocity[3];
  alGetListenerfv(AL_VELOCITY, velocity);
  return Vector3(velocity);
}

Vector3 SoundManager::GetListenerOri() {
}

void SoundManager::PlayBGM() {
}

void SoundManager::PauseBGM() {
}

void SoundManager::StopBGM() {
}

void SoundManager::ContinueGBM() {
}

void SoundManager::Update() {
}

/* The End */
