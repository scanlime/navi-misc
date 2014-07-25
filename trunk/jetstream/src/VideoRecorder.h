/*
 * VideoRecorder.h - A module to record the engine's output live to disk
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

#ifndef _H_VIDEORECORDER
#define _H_VIDEORECORDER

#include "nuppelvideo/NuppelWriter.h"
#include "Engine.h"
#include <string>

class VideoRecorder {
 public:
  VideoRecorder();
  ~VideoRecorder();

  void start(void);
  void stop(void);

  int isRunning(void);

  /* The following parameters are sampled when start() is called */
  int width, height;
  float framerate;
  std::string filename;
  float quality;
  int rtjpeg, lzo, rgb;    /* These should be bool, but Boost.Python was having trouble with it */

 private:
  MainLoopHandler<VideoRecorder> *mainHandler;
  void mainloopIteration(void);
  NuppelWriter *writer;
  bool running;
};

#endif /* _H_VIDEORECORDER */

/* The End */
