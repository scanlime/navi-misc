#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <cv.hpp>
#include <cvaux.h>
#include <SDL.h>

#define N_CAMERAS    1


void plot_crosshairs(IplImage *image, CvPoint *point,
		     int size=15, double color=CV_RGB(255,0,0),
		     int thickness=1) {
  /* Show some simple crosshairs on an image at the given point */
  CvPoint endpoint1, endpoint2;
  endpoint1.x = point->x - size/2;
  endpoint1.y = point->y;
  endpoint2.x = point->x + size/2;
  endpoint2.y = point->y;
  cvLine(image, endpoint1, endpoint2, color, thickness);
  endpoint1.y = point->y - size/2;
  endpoint1.x = point->x;
  endpoint2.y = point->y + size/2;
  endpoint2.x = point->x;
  cvLine(image, endpoint1, endpoint2, color, thickness);
}

void gray_to_yuv(IplImage *src, IplImage *dest) {
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

void map_mouse_position(int *x, int *y, int *camera, IplImage *image) {
  /* Given a mouse position in window coordinates, return
   * a mouse position in image coordiantes and a camera number.
   * A sample image must be provided so this knows our camera resolution.
   */
  SDL_Surface *surface = SDL_GetVideoSurface();

  /* Convert the mouse coordinates to be relative to the YUV surface,
   * undoing any resizing the user may have performed on our window.
   */
  *x = *x * image->width * N_CAMERAS / surface->w;
  *y = *y * image->height * 2 / surface->h;

  /* Assuming the cameras are horizontally tiled, figure out
   * the camera we're clicking in and the position within that camera's frame
   */
  *camera = *x / image->width;
  *x %= image->width;
  *y %= image->height;
}

int main() {
  IplImage **images;
  IplImage *back_projections[N_CAMERAS];
  IplImage *planes[3];
  IplImage *view_grid[N_CAMERAS*2];
  IplImage *channel_temp;
  CvHistogram* object_hist[N_CAMERAS];
  int i, j;
  int hist_size[] = {64, 64, 64};
  float channel_range[] = {0, 255};
  float* ranges[] = {channel_range, channel_range, channel_range};
  int mouse_x, mouse_y, mouse_camera, mouse_buttons;
  int sample_square_size = 32;

  cv_dc1394_init();

  /* Take a reference frame from each camera */
  images = cv_dc1394_capture_yuv(N_CAMERAS);

  /* Allocate images, now that we know the camera resolution */
  for (i=0; i<3; i++)
    planes[i] = cvCreateImage(cvGetSize(images[0]), 8, 1);
  for (i=0; i<N_CAMERAS; i++)
    back_projections[i] = cvCreateImage(cvGetSize(images[0]), 8, 3);
  channel_temp = cvCreateImage(cvGetSize(images[0]), 8, 1);

  /* Allocate histograms */
  for (i=0; i<N_CAMERAS; i++)
    object_hist[i] = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);

  /* Tile cameras horizontally, with original image on
   * top and backprojection on bottom.
   */
  j = 0;
  for (i=0; i<N_CAMERAS; i++) {
    view_grid[j] = images[i];
    j++;
  }
  for (i=0; i<N_CAMERAS; i++) {
    view_grid[j] = back_projections[i];
    j++;
  }

  while (cv_sdl_process_events()) {
    images = cv_dc1394_capture_yuv(N_CAMERAS);

    /* Calculate the backprojection */
    for (i=0; i<N_CAMERAS; i++) {
      cvCvtPixToPlane(images[i], planes[0], planes[1], planes[2], 0);
      cvCalcBackProject(planes, channel_temp, object_hist[i]);
      gray_to_yuv(channel_temp, back_projections[i]);
    }

    if (SDL_GetVideoSurface()) {
      /* Interactive interface for modifying the histogram,
       * only works after we've initialized our video by displaying
       * the first frame.
       */

      Uint8 *keystate = SDL_GetKeyState(NULL);
      CvRect sample_rect;

      /* The < and > keys change the size of the sample square */
      if (keystate[',']) {
	sample_square_size--;
	if (sample_square_size < 1)
	  sample_square_size = 1;
      }
      if (keystate['.']) {
	sample_square_size++;
      }

      /* Get the current sampling rect, centered on the mouse cursor */
      mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
      map_mouse_position(&mouse_x, &mouse_y, &mouse_camera, images[0]);
      sample_rect.x = mouse_x - sample_square_size/2;
      sample_rect.y = mouse_y - sample_square_size/2;
      sample_rect.width = sample_square_size;
      sample_rect.height = sample_square_size;

      /* The right mouse button clears our histogram */
      if (mouse_buttons & SDL_BUTTON(3)) {
	cvClearHist(object_hist[mouse_camera]);
      }

      /* If the left mouse button is down, use this image to collect samples towards a
       * reference histogram for the object we're trying to track.
       */
      if (mouse_buttons & SDL_BUTTON(1)) {
	cvSetImageROI(images[mouse_camera], sample_rect);
	cvSetImageROI(planes[0], sample_rect);
	cvSetImageROI(planes[1], sample_rect);
	cvSetImageROI(planes[2], sample_rect);

	cvCvtPixToPlane(images[mouse_camera], planes[0], planes[1], planes[2], 0);
	cvCalcHist(planes, object_hist[mouse_camera], 1);

	cvResetImageROI(images[mouse_camera]);
	cvResetImageROI(planes[0]);
	cvResetImageROI(planes[1]);
	cvResetImageROI(planes[2]);
      }

      /* Draw a box around the current sampling rectangle */
      cvRectangle(images[mouse_camera], cvPoint(sample_rect.x-1, sample_rect.y-1),
		  cvPoint(sample_rect.x + sample_rect.width + 1, sample_rect.y + sample_rect.width + 1),
		  CV_RGB(128,128,255), 1);
    }

    cv_sdl_show_yuv_tiles(view_grid, N_CAMERAS*2, N_CAMERAS);
  }
}
