/*
 * NuppelWriter.cpp - A simple class for writing compressed NuppelVideo files.
 *                    NuppelVideo is a simple video format based on RTjpeg and LZO
 *                    that is designed for recording with minimal CPU. The
 *                    VideoRecorder object uses this to record real-time, assuming that
 *                    you'll recompress to Divx or something similar for redistribution.
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

#include <string>
#include <exception>
#include "NuppelWriter.h"
#include <stdio.h>
#include <string.h>

extern "C" {
#include <SDL/SDL.h>
#include "minilzo.h"
#include "RTjpegN.h"
#include "nuppelvideo.h"
}

/* Colorspace conversion routines from rgb2yuv420.cpp */
void InitLookupTable();
int RGB2YUV420 (int x_dim, int y_dim, 
		unsigned char *bmp, 
		unsigned char *yuv,
		unsigned char *tmp1, unsigned char *tmp2);


class NuppelException : public std::exception {
 public:
  NuppelException(const char *error) throw() {
    s = "NuppelWriter error: ";
    s += error;
  }
  ~NuppelException() throw() {}
  virtual const char *what(void) const throw() {
    return s.c_str();
  }
 private:
  std::string s;
};


NuppelWriter::NuppelWriter(int width_, int height_, const char *filename,
			   float frameRate_, float rtjQuality, bool rtjpeg_,
			   bool lzo_, bool rgb_, int keyframeFreq_) {
  rtfileheader nuvh;
  rtframeheader frameh;
  static unsigned long int tbls[128];
  int Q;

  width = width_;
  height = height_;
  frameRate = frameRate_;
  keyframeFreq = keyframeFreq_;
  lzo = lzo_;
  rgb = rgb_;
  rtjpeg = rtjpeg_;
  frameofgop = 0;
  frameNumber = 0;
  
  /* Initialize a nuppelvideo header */
  memset(&nuvh, 0, sizeof(nuvh));
  strcpy(nuvh.finfo,"NuppelVideo");
  strcpy(nuvh.version,"0.05");
  nuvh.width = width;
  nuvh.height = height;
  nuvh.pimode = 'P';
  nuvh.aspect = 1.0;
  nuvh.fps = frameRate;
  nuvh.videoblocks = -1;   /* unknown */
  
  /* Open file */
  file = fopen(filename,"wb");
  if (!file)
    throw NuppelException("Unable to open output file");

  /* Allocate framebuffer */
  bufferSize = width * height * 3;
  frameBuffer = new unsigned char [bufferSize];
  if (!frameBuffer)
    throw NuppelException("Can't allocate framebuffer");

  /* Allocate several temporary buffers */
  compressBuffer = new unsigned char [width*height+(width*height)/2];
  if (!compressBuffer)
    throw NuppelException("Can't allocate compression buffer");

  yuvBuffer = new unsigned char [width*height+(width*height)/2];
  if (!yuvBuffer)
    throw NuppelException("Can't allocate YUV buffer");
  InitLookupTable();

  yuvTmp1 = new unsigned char [width*height];
  yuvTmp2 = new unsigned char [width*height];
  if (!yuvTmp1 || !yuvTmp2)
    throw NuppelException("Can't allocate YUV temporary buffers");

  lzoTmp = new unsigned char [LZO1X_MEM_COMPRESS];
  if (!lzoTmp)
    throw NuppelException("Can't allocate LZO temporary buffer");

  /* Write the header */
  if (fwrite(&nuvh, 1, sizeof(nuvh), file) != sizeof(nuvh))
    throw NuppelException("Error writing header");
  
  /* According to the RTJpeg docs, 255 is the full scale for Q,
   * being equivalent to a JPEG with 70% quality.
   */
  Q = (int) (rtjQuality*255);

  /* Initialize the compression tables */
  RTjpeg_init_compress(tbls,width,height, Q);
  RTjpeg_init_mcompress();

  /* Write the compression configuration frame */
  memset(&frameh, 0, sizeof(frameh));
  frameh.frametype = 'D';
  frameh.comptype = 'R';
  frameh.packetlength = sizeof(tbls);
  fwrite(&frameh, 1, sizeof(frameh), file);
  fwrite(&tbls, 1, sizeof(tbls), file);

  /* Get a start timestamp (we assume realtime recording) */
  startTime = SDL_GetTicks();
}

NuppelWriter::~NuppelWriter() {
  delete lzoTmp;
  delete yuvTmp1;
  delete yuvTmp2;
  delete compressBuffer;
  delete frameBuffer;
  delete yuvBuffer;
  fclose(file);
}

void NuppelWriter::writeFrame(void) {
  rtframeheader frameh;
  lzo_uint lzoSize;
  unsigned char *currentBuffer;
  unsigned int currentBufferSize;

  /* Do we need to write a keyframe? */
  if ((frameofgop % keyframeFreq) == 0) {
    memset(&frameh, 'j', sizeof(frameh));
    frameh.frametype = 'R';
    frameh.comptype = 'T';
    fwrite(&frameh, 1, sizeof(frameh), file);
    memset(&frameh, 0, sizeof(frameh));
    frameofgop = 0;
    frameh.frametype = 'S';
    frameh.comptype = 'V';
    frameh.timecode = frameNumber;
    fwrite(&frameh, 1, sizeof(frameh), file);
  }
  
  /* Set up a video frame with RTJpeg and LZO compression */
  memset(&frameh, 0, sizeof(frameh));
  frameh.frametype = 'V';
  frameh.keyframe = frameofgop;
  frameh.timecode = SDL_GetTicks() - startTime;

  if (rgb) {
    /* Nonstandard: uncompressed bottom-up RGB24 */
    frameh.comptype = 'R';
    currentBuffer = frameBuffer;
    currentBufferSize = bufferSize;
  }
  else {
    /* Convert from RGB to YUV420. This routine has also
     * been modified to flip the video vertically.
     */
    RGB2YUV420(width, height, frameBuffer, yuvBuffer, yuvTmp1, yuvTmp2);
    currentBuffer = yuvBuffer;
    currentBufferSize = width*height+(width*height/2);
    frameh.comptype = '0';
  }

  if (rtjpeg) {
    /* Compress the frame using RTJpeg (lossy) */
    currentBufferSize = RTjpeg_mcompressYUV420((__s8*) compressBuffer, yuvBuffer, 1, 1);
    currentBuffer = compressBuffer;
    frameh.comptype = '1';
  }

  if (lzo) {
    /* Compress it again using LZO (lossless) */
    lzo1x_1_compress(currentBuffer, currentBufferSize, frameBuffer, &lzoSize, lzoTmp);
    currentBufferSize = lzoSize;
    currentBuffer = frameBuffer;
    if (rtjpeg)
      frameh.comptype = '2';
    else {
      if (rgb)
	frameh.comptype = 'r';   /* Nonstandard: LZO'ed bottom-up RGB24 */
      else
	frameh.comptype = '3';
    }
  }

  /* Write the frame */
  frameh.packetlength = currentBufferSize;
  fwrite(&frameh, 1, sizeof(frameh), file);
  if (fwrite(currentBuffer, 1, currentBufferSize, file) != currentBufferSize)
    throw NuppelException("Error writing to file");

  frameNumber++;
  frameofgop++;
}
