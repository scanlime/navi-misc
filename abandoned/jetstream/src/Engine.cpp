/*
 * Engine.cpp - A modular system for initialization, main loop, and shutdown
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

#include "Engine.h"
#include "ViewingFrustum.h"
#include <SDL/SDL.h>
#include <GL/gl.h>

Engine *Engine::instance = NULL;

Engine::Engine() {
  instance = this;
  running = true;
  frustumLock = false;
  throttleFrameRate = false;
  old_ticks = 0;
}

DetailSettings *Engine::getDetail(void) {
  return &detail;
}

void Engine::iteration(void) {
  std::multimap<priority,MainLoopHandlerBase*>::iterator i;

  /* Frame rate calculations */
  new_ticks = SDL_GetTicks();
  frameTime = (new_ticks - old_ticks) / 1000.0f;
  old_ticks = new_ticks;
  if (throttleFrameRate) {
    float target_frameTime;
    target_frameTime = 1.0 / targetFrameRate;
    if (target_frameTime > frameTime)
      SDL_Delay((Uint32) ((target_frameTime - frameTime)*1000));
    frameTime = target_frameTime;
  }
  
  /* Run all handlers, in order */
  handlerListMutex.lock();
  for(i=mainloopHandlers.begin(); i!=mainloopHandlers.end(); i++)
    i->second->run();
  handlerListMutex.unlock();
}

Engine *Engine::getInstance() {
  if(instance == NULL)
    instance = new Engine;
  return instance;
}

/* The End */
