/*
 * cv_dc1394.h - Specification of an interface between OpenCV and libdc1394.
 *               This simple interface makes it easy to intiailize all attached
 *               Firewire digital cameras using libdc1394 and, for each camera,
 *               capture frames into OpenCV images.
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

#ifndef __CV_DC1394__H_
#define __CV_DC1394__H_

#include <cv.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

/* Information about each camera */
typedef struct _cv_dc1394_camera {
  raw1394handle_t       handle;
  dc1394_cameracapture *capture;
  nodeid_t              node;
  dc1394_feature_set    features;
} cv_dc1394_camera;


/* Initialize the module, returning a NULL-terminated list of camera pointers */
cv_dc1394_camera** cv_dc1394_init();

/* Return the number of cameras detected */
int cv_dc1394_get_camera_count();

/* Set the frame rate to use, must be called before cv_dc1394_init.
 * The rate must be given as a dc1394 FRAMERATE_* constant.
 */
void cv_dc1394_set_framerate(int rate);

/* Set the mode to use, must be called before cv_dc1394_init.
 * The mode must be given as a dc1394 MODE_* constant.
 */
void cv_dc1394_set_mode(int mode);

/* Capture an image from all cameras, returning an array of images that
 * do not need to be freed. The same images are reused for the next capture.
 * All images are 3 channels, 8-bit per channel, YUV colorspace.
 */
IplImage** cv_dc1394_capture_yuv(int num_images);

/* Like above, but convert the captured images to RGB */
IplImage** cv_dc1394_capture_rgb(int num_images);

/* A temporary kludge for capturing directly in RGB, not converting from YUV.
 * This should be supported transparently by cv_dc1394_capture_rgb.
 */
IplImage** cv_dc1394_capture_rgb_raw(int num_images);

/* ...or in grayscale */
IplImage** cv_dc1394_capture_gray(int num_images);

#endif /* __CV_DC1394__H_ */

/* The End */
