/*
 * camshift_frontend.cpp - An easy to use image capture and video output frontend for
 *                         OpenCV's implementation of the CAMSHIFT object tracking algorithm.
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

#include "camshift_frontend.h"
#include <cv_sdl.h>

static void gray_to_yuv(IplImage *src, IplImage *dest);
static void draw_box(IplImage *image, CvBox2D box, double color=CV_RGB(0,255,128));
static void draw_crosshairs(IplImage *image, float x, float y, float size=15, double color=CV_RGB(255,0,128));


MultiCamshiftUI::MultiCamshiftUI(int _n_cameras, CvSize _image_size) {
  n_cameras = _n_cameras;
  image_size = _image_size;

  yuv_backprojections = new IplImage* [n_cameras];
  histograms = new CvHistogram* [n_cameras];
  search_windows = new CvRect [n_cameras];
  results = new CvBox2D [n_cameras];
  sample_square_size = 24;
  draw_sample_square = false;
  sample_from_sample_square = false;

  for (int i=0; i<3; i++)
    planes[i] = cvCreateImage(image_size, 8, 1);
  for (int i=0; i<n_cameras; i++)
    yuv_backprojections[i] = cvCreateImage(image_size, 8, 3);

  backprojection = cvCreateImage(image_size, 8, 1);

  /* Allocate histograms */
  for (int i=0; i<n_cameras; i++) {
    int hist_size[] = {64, 128, 128};
    float range[] = {0, 255};
    float* ranges[] = {range, range, range};
    histograms[i] = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);
  }

  /* Initially empty all search windows. They will be set by the
   * user when the first histogram samples are taken.
   */
  for (int i=0; i<n_cameras; i++) {
    search_windows[i].x = 0;
    search_windows[i].y = 0;
    search_windows[i].width = 0;
    search_windows[i].height = 0;
  }
}

MultiCamshiftUI::~MultiCamshiftUI() {
  /* FIXME */
}

CvBox2D *MultiCamshiftUI::track(IplImage **images, bool show_ui, bool show_backprojections) {
  for (int camera=0; camera<n_cameras; camera++) {
    /* Calculate the backprojection, in the original (YUV) colorspace */
    cvCvtPixToPlane(images[camera], planes[0], planes[1], planes[2], 0);
    cvCalcBackProject(planes, backprojection, histograms[camera]);

    if (show_ui && show_backprojections) {
      /* Make a YUV version of the output, for display */
      gray_to_yuv(backprojection, yuv_backprojections[camera]);
    }

    if (search_windows[camera].width > 0 && search_windows[camera].height > 0) {
      /* Use the CAMSHIFT algorithm to search for the object of interest */
      CvConnectedComp comp;
      cvCamShift(backprojection, search_windows[camera],
		 cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1, 1),
		 &comp, &results[camera]);
      search_windows[camera] = comp.rect;

      if (results[camera].size.width > 0 &&
	  results[camera].size.height > 0) {
	/* We found an interesting object, draw it if applicable */
	if (show_ui)
	  drawResults(camera, images[camera]);
      }
      else {
	/* We lost tracking, expand the search window */
	search_windows[camera].x = 0;
	search_windows[camera].y = 0;
	search_windows[camera].width = image_size.width;
	search_windows[camera].height = image_size.height;
      }
    }
  }

  if (sample_from_sample_square) {
    cvSetImageROI(images[sample_square_camera], sample_square);
    cvSetImageROI(planes[0], sample_square);
    cvSetImageROI(planes[1], sample_square);
    cvSetImageROI(planes[2], sample_square);

    cvCvtPixToPlane(images[sample_square_camera], planes[0], planes[1], planes[2], 0);
    cvCalcHist(planes, histograms[sample_square_camera], 1);

    cvResetImageROI(images[sample_square_camera]);
    cvResetImageROI(planes[0]);
    cvResetImageROI(planes[1]);
    cvResetImageROI(planes[2]);

    /* Also set the windowIn to the sampling rectangle, to point CAMSHIFT at
     * what we're interested in.
     */
    search_windows[sample_square_camera] = sample_square;
  }

  if (show_ui) {
    /* Tile cameras horizontally, with original image on
     * top and backprojection on bottom.
     */
    IplImage* view_grid[n_cameras * 2];
    int num_views = 0;
    for (int i=0; i<n_cameras; i++)
      view_grid[num_views++] = images[i];
    if (show_backprojections) {
      for (int i=0; i<n_cameras; i++)
	view_grid[num_views++] = yuv_backprojections[i];
    }

    if (draw_sample_square) {
      cvRectangle(images[sample_square_camera], cvPoint(sample_square.x-1, sample_square.y-1),
		  cvPoint(sample_square.x + sample_square.width + 1, sample_square.y + sample_square.width + 1),
		  CV_RGB(128,128,255), 1);
    }

    cv_sdl_show_yuv_tiles(view_grid, num_views, n_cameras);
  }

  return results;
}

static void gray_to_yuv(IplImage *src, IplImage *dest) {
  unsigned char *src_p,  *src_row  = (unsigned char*) src->imageData;
  unsigned char *dest_p,  *dest_row  = (unsigned char*) dest->imageData;
  int i, j;
  for (j=dest->height; j; --j) {
    src_p = src_row;
    dest_p = dest_row;
    for (i=dest->width; i; --i) {
      *(dest_p++) = *(src_p++);
      *(dest_p++) = 128;
      *(dest_p++) = 128;
    }
    src_row += src->widthStep;
    dest_row += dest->widthStep;
  }
}

static void draw_box(IplImage *image, CvBox2D box, double color) {
  CvPoint2D32f boxPoints[4];

  /* CamShift seems to get this backwards */
  box.angle = -box.angle;

  cvBoxPoints(box, boxPoints);
  cvLineAA(image,
	   cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
	   cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
	   color);
  cvLineAA(image,
	   cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
	   cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
	   color);
  cvLineAA(image,
	   cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
	   cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
	   color);
  cvLineAA(image,
	   cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
	   cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
	   color);
}

static void draw_crosshairs(IplImage *image, float x, float y, float size, double color) {
  /* Show some simple crosshairs on an image at the given point,
   * antialiased with subpixel positioning.
   */
  CvPoint endpoint1, endpoint2;
  endpoint1.x = (int)((x - size/2) * 16 + 0.5);
  endpoint1.y = (int)(y * 16 + 0.5);
  endpoint2.x = (int)((x + size/2) * 16 + 0.5);
  endpoint2.y = (int)(y * 16 + 0.5);
  cvLineAA(image, endpoint1, endpoint2, color, 4);
  endpoint1.y = (int)((y - size/2) * 16 + 0.5);
  endpoint1.x = (int)(x * 16 + 0.5);
  endpoint2.y = (int)((y + size/2) * 16 + 0.5);
  endpoint2.x = (int)(x * 16 + 0.5);
  cvLineAA(image, endpoint1, endpoint2, color, 4);
}

void MultiCamshiftUI::drawResults(int camera, IplImage *image) {
  CvBox2D *box = &results[camera];

  /* Check the roll quality. For aspect ratios near 1, the box rotation
   * can't be measured accurately. In these situations, we draw the box
   * as a circle, to convey this information.
   */
  float q = box->size.height / box->size.width;
  if (q < 1.25) {
    cvCircleAA(image, cvPoint((int)box->center.x, (int)box->center.y),
	       (int)((box->size.height + box->size.width)/4), CV_RGB(0,255,128));
  }
  else {
    draw_box(image, *box);
  }

  /* Always draw some crosshairs at the box center */
  draw_crosshairs(image, box->center.x, box->center.y);
}

bool MultiCamshiftUI::processEvent(SDL_Event *event) {
  switch (event->type) {

    case SDL_MOUSEBUTTONDOWN:
      /* Use the scroll wheel to change sample square size */
      switch (event->button.button) {
      case 5:
	sample_square_size -= 2;
	if (sample_square_size < 1)
	  sample_square_size = 1;
	draw_sample_square = true;
	return true;

      case 4:
	sample_square_size += 2;
	draw_sample_square = true;
	return true;
      }
      break;

    }
  return false;
}

void MultiCamshiftUI::updateEventState() {
  if (!SDL_GetVideoSurface())
    return;

  Uint8 *keystate = SDL_GetKeyState(NULL);
  int mouse_x, mouse_y, mouse_camera, mouse_buttons;

  /* If we don't discover a good reason to use the sample square below, don't */
  draw_sample_square = false;
  sample_from_sample_square = false;

  /* The spacebar just shows the rectangle */
  if (keystate[' '])
    draw_sample_square = true;

  /* Get the current sampling rect, centered on the mouse cursor */
  cv_sdl_get_mouse_state(&mouse_camera, &mouse_x, &mouse_y, &mouse_buttons);
  mouse_camera %= n_cameras;

  sample_square_camera = mouse_camera;
  sample_square.x = mouse_x - sample_square_size/2;
  sample_square.y = mouse_y - sample_square_size/2;
  sample_square.width = sample_square_size;
  sample_square.height = sample_square_size;
  if (sample_square.x < 0) sample_square.x = 0;
  if (sample_square.y < 0) sample_square.y = 0;
  if (sample_square.x > image_size.width - sample_square.width - 1)
    sample_square.x = image_size.width - sample_square.width - 1;
  if (sample_square.y > image_size.height - sample_square.height - 1)
    sample_square.y = image_size.height - sample_square.height - 1;

  /* The right mouse button clears our histogram */
  if (mouse_buttons & SDL_BUTTON(3)) {
    cvClearHist(histograms[mouse_camera]);
  }

  /* If the left mouse button is down, use this image to collect samples towards a
   * reference histogram for the object we're trying to track.
   */
  if (mouse_buttons & SDL_BUTTON(1)) {
    sample_from_sample_square = true;
    draw_sample_square = true;
  }
}

/* The End */
