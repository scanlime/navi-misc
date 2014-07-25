/*
 * NuppelWriter.h - A simple class for writing compressed NuppelVideo files.
 *                  NuppelVideo is a simple video format based on RTjpeg and LZO
 *                  that is designed for recording with minimal CPU. The
 *                  VideoRecorder object uses this to record real-time, assuming that
 *                  you'll recompress to Divx or something similar for redistribution.
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

#ifndef _H_NUPPELWRITER
#define _H_NUPPELWRITER

#include <stdio.h>

class NuppelWriter {
 public:

  /* Note: The 'rgb' flag uses an nonstandard RGB NuppelVideo frame format.
   *       It saves the large CPU cost of converting frames to YUV colorspace,
   *       but normal NuppelVideo decoders can't handle it.
   */
  NuppelWriter(int width, int height, const char *filename, 
	       float frameRate, float rtjQuality=1.0, bool rtjpeg=true,
	       bool lzo=true, bool rgb=false, int keyframeFreq=30);
  ~NuppelWriter();

  void writeFrame(void);

  /* This buffer is true color, 3 bytes per pixel, packed in RGB order,
   * scanned from bottom to top. It is compatible with glReadPixels.
   */
  unsigned char *frameBuffer;
  unsigned long bufferSize;

  int width, height;
  float frameRate;

 private:
  FILE *file;
  unsigned char *compressBuffer, *yuvBuffer;
  unsigned char *yuvTmp1, *yuvTmp2, *lzoTmp;
  unsigned long startTime;
  int keyframeFreq;
  int frameofgop;
  int frameNumber;
  bool rtjpeg, lzo, rgb;
};

#endif /* _H_NUPPELWRITER */
