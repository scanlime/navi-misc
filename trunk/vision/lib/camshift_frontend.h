/*
 * camshift_frontend.h - An easy to use image capture and video output frontend for
 *                       OpenCV's implementation of the CAMSHIFT object tracking algorithm.
 *
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __CAMSHIFT_FRONTEND__H_
#define __CAMSHIFT_FRONTEND__H_

#include <cv.h>

class MultiCamshiftUI {
 public:
  MultiCamshiftUI(int n_Cameras, CvSize image_size);
  ~MultiCamshiftUI();

  CvBox2D *track(IplImage **images, bool show_ui=true, bool show_backprojections=true);
  bool processEvents();

 protected:
  void drawResults(int camera, IplImage *image);

  CvSize image_size;
  int n_cameras;

  IplImage *planes[3];
  IplImage **yuv_backprojections;
  IplImage *backprojection;
  CvHistogram **histograms;
  CvRect *search_windows;
  CvBox2D *results;

  CvRect sample_square;
  int sample_square_camera;
  int sample_square_size;
  bool draw_sample_square;
  bool sample_from_sample_square;
};


#endif /* __CV_SDL__H_ */

/* The End */
