/*
 * cv_sdl.c - Glue to make it easy to display video from OpenCV applications
 *            in SDL, taking advantage of hardware accelerated YUV overlays.
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

#include "cv_sdl.h"

static int sdl_is_initialized = 0;
static int win_width = -1, win_height = -1;
static SDL_Surface *sdl_screen = NULL;
static SDL_Overlay *sdl_overlay = NULL;
static SDL_Rect sdl_dest_rect;

static void cv_sdl_init_window(int width, int height);
static void convert_yuv24_to_yuy2(IplImage *src, SDL_Overlay *dest, int x, int y);


/************************************************************************************/
/***************************************************************** Public Functions */
/************************************************************************************/

/* Show a single YUV image in the window, initializing and/or resizing as necessary */
void cv_sdl_show_yuv_image(IplImage *image) {
  cv_sdl_init_window(image->width, image->height);
  SDL_LockYUVOverlay(sdl_overlay);
  convert_yuv24_to_yuy2(image, sdl_overlay, 0, 0);
  SDL_UnlockYUVOverlay(sdl_overlay);
  SDL_DisplayYUVOverlay(sdl_overlay, &sdl_dest_rect);
}

/* Show several YUV images, tiled, in one window */
void cv_sdl_show_yuv_tiles(IplImage **images, int num_images, int width) {
  int height = (num_images-1)/width + 1;
  int i, tile_x, tile_y;
  IplImage *ref_image = images[0];

  cv_sdl_init_window(ref_image->width * width, ref_image->height * height);
  SDL_LockYUVOverlay(sdl_overlay);

  tile_x = tile_y = 0;
  for (i=0; i<num_images; i++) {
    convert_yuv24_to_yuy2(images[i], sdl_overlay, tile_x * ref_image->width, tile_y * ref_image->height);
    tile_x++;
    if (tile_x == width) {
      tile_x = 0;
      tile_y++;
    }
  }

  SDL_UnlockYUVOverlay(sdl_overlay);
  SDL_DisplayYUVOverlay(sdl_overlay, &sdl_dest_rect);
}

int cv_sdl_process_events() {
  SDL_Event event;

  if (!sdl_is_initialized)
    return 1;

  while (SDL_PollEvent(&event))
    switch (event.type) {

    case SDL_QUIT:
      return 0;

    case SDL_VIDEORESIZE:
      SDL_SetVideoMode(event.resize.w, event.resize.h, 0, SDL_RESIZABLE);
      sdl_dest_rect.x = 0;
      sdl_dest_rect.y = 0;
      sdl_dest_rect.w = event.resize.w;
      sdl_dest_rect.h = event.resize.h;
      break;

    }

  return 1;
}


/************************************************************************************/
/**************************************************************** Private Functions */
/************************************************************************************/

static void cv_sdl_init_window(int width, int height) {
  if (!sdl_is_initialized) {
    SDL_Init(SDL_INIT_VIDEO);
    sdl_is_initialized = 1;
  }

  if (width != win_width || height != win_height) {
    win_width = width;
    win_height = height;

    sdl_screen = SDL_SetVideoMode(width, height, 0, SDL_RESIZABLE);

    if (sdl_overlay)
            SDL_FreeYUVOverlay(sdl_overlay);
    sdl_overlay = SDL_CreateYUVOverlay(width, height, SDL_YUY2_OVERLAY, sdl_screen);
    sdl_dest_rect.x = 0;
    sdl_dest_rect.y = 0;
    sdl_dest_rect.w = width;
    sdl_dest_rect.h = height;
  }
}

static void convert_yuv24_to_yuy2(IplImage *src, SDL_Overlay *dest, int x, int y) {
  unsigned char *src_p, *src_row_p = (unsigned char*) src->imageData;
  unsigned char *dest_p, *dest_row_p = (unsigned char*) dest->pixels[0];
  int i, j;
  int y0, y1, u0, u1, v0, v1, u, v;

  dest_row_p += x*2 + y*dest->pitches[0];

  for (j=src->height; j; --j) {
    src_p = src_row_p;
    dest_p = dest_row_p;
    for (i=src->width>>1; i; --i) {
      y0 = *(src_p++);
      u0 = *(src_p++);
      v0 = *(src_p++);
      y1 = *(src_p++);
      u1 = *(src_p++);
      v1 = *(src_p++);
      u = (u0 + u1)>>1;
      v = (v0 + v1)>>1;
      *(dest_p++) = y0;
      *(dest_p++) = u;
      *(dest_p++) = y1;
      *(dest_p++) = v;
    }
    src_row_p += src->widthStep;
    dest_row_p += dest->pitches[0];
  }
}

/* The End */
