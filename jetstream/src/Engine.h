/*
 * Engine.h - A modular system for initialization, main loop, and shutdown
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

#ifndef _H_ENGINE
#define _H_ENGINE

#include "Mutex.h"
#include "DetailSettings.h"
#include <list>
#include <map>

class Engine;

/* Base class for MainLoopHandler template */
class MainLoopHandlerBase {
 public:
  virtual void run(void) = 0;
};

class Engine {
 public:
  Engine();

  /* Run iteration() as long as 'running' is true */
  void iteration(void);
  bool running;

  /* Frustum lock: this is set by the python code for debugging,
   *               and the Surface objects read this as true they will
   *               delay updating their frustums.
   */
  bool frustumLock;

  /* Current measured frame time, in seconds */
  float frameTime;
  
  /* Target frame rate, used if throttleFrameRate is true */
  float targetFrameRate;
  bool throttleFrameRate;

  /* This defines the order in which main loop handlers are run.
   * The order for handlers with the same priority is undefined.
   */
  enum priority {
    priority_drawScene,
    priority_drawUI,
    priority_captureFrame,
    priority_sound,
  };

  /* Main loop handlers, keyed by priority */
  std::multimap<priority,MainLoopHandlerBase*> mainloopHandlers;
  Mutex handlerListMutex;

  static Engine *getInstance();

  DetailSettings detail;

  /* Helper function for python bindings */
  DetailSettings *getDetail(void);

 private:
  Uint32 old_ticks, new_ticks;
  static Engine *instance;
};


/* Template for main loop handlers, parameterized by engine module class */
template <class T>
class MainLoopHandler : public MainLoopHandlerBase {
 public:
  MainLoopHandler(T *module_, Engine::priority p, void (T::*function_)(void)) {
    module = module_;
    function = function_;
    Engine::getInstance()->handlerListMutex.lock();
    Engine::getInstance()->mainloopHandlers.insert(std::pair<Engine::priority,MainLoopHandlerBase*>(p,this));
    Engine::getInstance()->handlerListMutex.unlock();
  }
  virtual void run(void) {
    (module->*function)();
  }
  void (T::*function)(void);
  T *module;
};


#endif /* _H_ENGINE */

/* The End */
