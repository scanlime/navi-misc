/*
 * cv_dc1394.c - Implementation of an interface between OpenCV and libdc1394.
 *               This simple interface makes it easy to intiailize all attached
 *               Firewire digital cameras using libdc1394 and, for each camera,
 *               capture frames into OpenCV images.
 *
 *               Captured frames are rescaled up to a full 24 bits per pixel,
 *               but remain in YUV colorspace. The images can often be processed
 *               without ever being converted to RGB, but if RGB conversion is
 *               necessary it must be done separately.
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

#include "cv_dc1394.h"

/* Defaults are hardcoded for now, overrideable with the cv_dc1394_set_* functions.
 * In the future there may be autodetection of these settings.
 */
static int default_framerate = FRAMERATE_30;
static int default_mode = MODE_640x480_YUV411;

/* This seems to be enough for the libdc1394 examples... */
#define DROP_FRAMES 1
#define MAX_PORTS   4
#define MAX_CAMERAS 8
#define NUM_BUFFERS 8

static cv_dc1394_camera* cameras[MAX_CAMERAS];
static int num_opened_cameras;

/* Store a separate arrays of capture information and OpenCV images.
 * Corresponding members of cv_dc1394_camera point to the entries in these arrays.
 */
static dc1394_cameracapture camera_captures[MAX_CAMERAS];
static IplImage* camera_images[MAX_CAMERAS];

typedef void (*image_converter_t)(IplImage *, unsigned char *);

static void init_camera(int port, nodeid_t node, int camera_number);
static void fatal_error(const char *description);
static IplImage* create_image();
IplImage** cv_dc1394_capture_generic(int num_images, image_converter_t conv);

static void convert_yuv411_to_yuv24(IplImage *img, unsigned char *src);
static void convert_yuv411_to_bgr24(IplImage *img, unsigned char *src);


/************************************************************************************/
/***************************************************************** Public Functions */
/************************************************************************************/

/* Initialize the module, returning a NULL-terminated list of camera pointers */
cv_dc1394_camera** cv_dc1394_init() {
  raw1394handle_t raw_handle;
  struct raw1394_portinfo ports[MAX_PORTS];
  int port, camCount, i, numPorts;
  nodeid_t *camera_nodes;

  raw_handle = raw1394_new_handle();
  if (!raw_handle)
    fatal_error("Unable to open the raw1394 device");

  /* Get information on each 1394 port */
  numPorts = raw1394_get_port_info(raw_handle, ports, numPorts);
  raw1394_destroy_handle(raw_handle);

  /* Initialize each camera we find */
  for (port=0; port<numPorts; port++) {
    raw_handle = raw1394_new_handle();
    raw1394_set_port(raw_handle, port);
    camera_nodes = dc1394_get_camera_nodes(raw_handle, &camCount, 1);
    raw1394_destroy_handle(raw_handle);

    for (i = 0; i < camCount; i++)
      init_camera(port, camera_nodes[i], i);

    dc1394_free_camera_nodes(camera_nodes);
  }
}

void cv_dc1394_set_framerate(int rate) {
  default_framerate = rate;
}

void cv_dc1394_set_mode(int mode) {
  default_mode = mode;
}

IplImage** cv_dc1394_capture_yuv(int num_images) {
  return cv_dc1394_capture_generic(num_images, convert_yuv411_to_yuv24);
}

IplImage** cv_dc1394_capture_rgb(int num_images) {
  return cv_dc1394_capture_generic(num_images, convert_yuv411_to_bgr24);
}


/************************************************************************************/
/**************************************************************** Private Functions */
/************************************************************************************/

IplImage** cv_dc1394_capture_generic(int num_images, image_converter_t conv) {
  cv_dc1394_camera **cam_p;
  IplImage **img_p;

  /* Wait for all cameras to have a frame available */
  dc1394_dma_multi_capture(camera_captures, num_images);

  /* Copy, convert, and free buffers from each camera. */
  for (cam_p=cameras, img_p=camera_images; num_images>0; cam_p++, img_p++, num_images--) {
    if (!img_p[0])
      img_p[0] = create_image();
    conv(img_p[0], (unsigned char*) cam_p[0]->capture->capture_buffer);
    dc1394_dma_done_with_buffer(cam_p[0]->capture);
  }

  return camera_images;
}

static void fatal_error(const char *description) {
  printf("%s\n", description);
  exit(1);
}

static void init_camera(int port, nodeid_t node, int camera_number) {
  /* Initialize a camera and append it to cameras[] */
  unsigned int channel, speed;
  cv_dc1394_camera* camera = (cv_dc1394_camera*) malloc(sizeof(cv_dc1394_camera));
  memset(camera, 0, sizeof(cv_dc1394_camera));

  camera->node = node;
  camera->capture = &camera_captures[num_opened_cameras];

  camera->handle = dc1394_create_handle(port);
  if (!camera->handle)
    fatal_error("Unable to acquire a camera handle");

  if (dc1394_get_camera_feature_set(camera->handle, node, &camera->features) != DC1394_SUCCESS)
    fatal_error("Unable to get camera features");

  if (dc1394_get_iso_channel_and_speed(camera->handle, camera->node, &channel, &speed) != DC1394_SUCCESS)
    fatal_error("Unable to get camera channel and speed");

  if (dc1394_dma_setup_capture(camera->handle, camera->node, camera_number+1,
			       FORMAT_VGA_NONCOMPRESSED, default_mode, SPEED_400,
			       default_framerate, NUM_BUFFERS, DROP_FRAMES,
			       NULL, camera->capture) != DC1394_SUCCESS)
    fatal_error("Unable to set up capture, maybe the mode is unsupported?");

  if (dc1394_start_iso_transmission(camera->handle, camera->node) != DC1394_SUCCESS)
    fatal_error("Unable to start isochronous transmission");

  /* Add the camera to our list */
  cameras[num_opened_cameras++] = camera;
  cameras[num_opened_cameras] = NULL;
}

static IplImage* create_image() {
  /* FIXME: hardcoded size and depth */
  return cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
}

static void convert_yuv411_to_yuv24(IplImage *img, unsigned char *src) {
  int x,y;
  unsigned char y0, y1, y2, y3, u, v;
  unsigned char *dest = (unsigned char*) img->imageData;

  for (y=0; y<img->height; y++)
    for (x=0; x<img->width; x+=4) {
      u  = *(src++);
      y0 = *(src++);
      y1 = *(src++);
      v  = *(src++);
      y2 = *(src++);
      y3 = *(src++);

      *(dest++) = y0;
      *(dest++) = u;
      *(dest++) = v;
      *(dest++) = y1;
      *(dest++) = u;
      *(dest++) = v;
      *(dest++) = y2;
      *(dest++) = u;
      *(dest++) = v;
      *(dest++) = y3;
      *(dest++) = u;
      *(dest++) = v;
    }
}

#define YUV_TO_RGB(y,u,v,r,g,b) { \
  int c = 298 * ((y) - 16); \
  int d = (u) - 128; \
  int e = (v) - 128; \
  (r) = (c + 409*e + 128) >> 8; \
  if ((r)>255) (r) = 255; \
  if ((r)<0) (r) = 0; \
  (g) = (c - 100*d - 208*e + 128) >> 8; \
  if ((g)>255) (g) = 255; \
  if ((g)<0) (g) = 0; \
  (b) = (c + 516*d + 128) >> 8; \
  if ((b)>255) (b) = 255; \
  if ((b)<0) (b) = 0; \
} while (0);

static void convert_yuv411_to_bgr24(IplImage *img, unsigned char *src) {
  int x,y;
  int y0, y1, y2, y3, u, v, r, g, b;
  unsigned char *dest = (unsigned char*) img->imageData;

  for (y=0; y<img->height; y++)
    for (x=0; x<img->width; x+=4) {
      u  = *(src++);
      y0 = *(src++);
      y1 = *(src++);
      v  = *(src++);
      y2 = *(src++);
      y3 = *(src++);

      YUV_TO_RGB(y0, u, v, r, g, b);
      *(dest++) = b;
      *(dest++) = g;
      *(dest++) = r;

      YUV_TO_RGB(y1, u, v, r, g, b);
      *(dest++) = b;
      *(dest++) = g;
      *(dest++) = r;

      YUV_TO_RGB(y2, u, v, r, g, b);
      *(dest++) = b;
      *(dest++) = g;
      *(dest++) = r;

      YUV_TO_RGB(y3, u, v, r, g, b);
      *(dest++) = b;
      *(dest++) = g;
      *(dest++) = r;
    }
}

/* The End */
