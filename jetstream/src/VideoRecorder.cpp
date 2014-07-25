/*
 * VideoRecorder.cpp - A module to record the engine's output live to disk
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

#include "VideoRecorder.h"

/* Necessary for our use of gl_feedback() below */
extern "C" {
#include <pgserver/common.h>
#include <pgserver/video.h>
#include <pgserver/gl.h>
}

VideoRecorder::VideoRecorder() {
  running = false;
  writer = NULL;

  /* Default attributes */
  width = 512;
  height = 384;
  framerate = 30.0f;
  quality = 1.0f;
  rtjpeg = 1;
  lzo = 1;
  rgb = 0;
  filename = "snapshot.nuv";

  /* Main loop handler */
  mainHandler = new MainLoopHandler<VideoRecorder>(this,
						   Engine::priority_captureFrame,
						   &VideoRecorder::mainloopIteration);
}

VideoRecorder::~VideoRecorder() {
  delete mainHandler;
  if (writer)
    delete writer;
}

void VideoRecorder::start(void) {
  running = true;
}

void VideoRecorder::stop(void) {
  running = false;
}

void VideoRecorder::mainloopIteration(void) {
  if (running) {
    /* Create the writer if it doesn't exist */
    if (!writer) {
      writer = new NuppelWriter(width,height,filename.c_str(),framerate,quality,rtjpeg,lzo,rgb);
      
      Engine::getInstance()->throttleFrameRate = true;
      Engine::getInstance()->targetFrameRate = writer->frameRate;
    }

    /* If we're capturing to a video smaller than our screen,
     * use the gl_feedback function in PicoGUI's OpenGL driver to scale it down.
     * The gl_feedback function copies data from an OpenGL buffer into a texture,
     * then in the current OpenGL matrix projects it back onto the backbuffer with optional
     * mipmap biasing. This is useful for blurs, pixelization, scaling, and a lot of other
     * neat effects.
     */
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    if (writer->width < viewport[2] || writer->height < viewport[3]) {
      /* Set up PicoGUI's OpenGL parameters and force a matrix that maps
       * 1:1 to pixels (with no camera controls)
       */
      gl_frame_setup();
      glLoadIdentity();
      gl_matrix_pixelcoord();
      glScalef(((float)writer->width ) / viewport[2],
	       ((float)writer->height) / viewport[3], 1.0f);
      gl_feedback(viewport[0], viewport[1], viewport[2], viewport[3],
		  PG_LGOP_NONE, GL_LINEAR_MIPMAP_LINEAR, GL_BACK, GL_TRUE, 0);
      gl_frame_cleanup();
    }

    /* Grab the current backbuffer contents */
    glReadBuffer(GL_BACK);
    glReadPixels(0,viewport[3]-writer->height,
		 writer->width,writer->height,
		 GL_RGB,GL_UNSIGNED_BYTE,writer->frameBuffer);
    writer->writeFrame();
  }
  else {
    /* Destroy the writer if it still exists */
    if (writer) {
      delete writer;
      writer = NULL;
      Engine::getInstance()->throttleFrameRate = false;
    }
  }
}

int VideoRecorder::isRunning(void) {
  return running;
}

/* The End */
