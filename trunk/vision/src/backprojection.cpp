#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <cv.hpp>
#include <cvaux.h>

#define N_CAMERAS    1
#define SAMPLE_SIZE  64


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

int main() {
  IplImage **images;
  IplImage *back_projections[N_CAMERAS];
  IplImage *planes[3];
  IplImage *view_grid[N_CAMERAS*2];
  IplImage *channel_temp;
  CvHistogram* object_hist[N_CAMERAS];
  int i, j;
  int hist_size[] = {16, 32, 32};
  float channel_range[] = {0, 255};
  float* ranges[] = {channel_range, channel_range, channel_range};
  int mouse_x, mouse_y;

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

    /* If the first mouse button is down, use this image to make a reference histogram
     * for the object we're trying to track. The reference histogram is made from a small
     * square around the mouse position.
     */
    if (SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(1)) {
      CvRect roi;

      /* Assuming the cameras are horizontally tiled, figure out
       * the camera we're clicking in and the position within that camera's frame
       */
      i = mouse_x / images[0]->width;
      mouse_x %= images[0]->width;
      mouse_y %= images[0]->height;

      roi.x = mouse_x - SAMPLE_SIZE/2;
      roi.y = mouse_y - SAMPLE_SIZE/2;
      roi.width = SAMPLE_SIZE;
      roi.height = SAMPLE_SIZE;

      cvSetImageROI(images[i], roi);
      cvSetImageROI(planes[0], roi);
      cvSetImageROI(planes[1], roi);
      cvSetImageROI(planes[2], roi);

      object_hist[i] = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);
      cvCvtPixToPlane(images[i], planes[0], planes[1], planes[2], 0);
      cvCalcHist(planes, object_hist[i], 0, 0);

      cvResetImageROI(images[i]);
      cvResetImageROI(planes[0]);
      cvResetImageROI(planes[1]);
      cvResetImageROI(planes[2]);

      /* Draw a box around the area we just sampled */
      cvRectangle(images[i], cvPoint(roi.x-1, roi.y-1),
		  cvPoint(roi.x + roi.width + 1, roi.y + roi.width + 1),
		  CV_RGB(128,128,255), 1);
    }

    for (i=0; i<N_CAMERAS; i++) {
      cvCvtPixToPlane(images[i], planes[0], planes[1], planes[2], 0);
      cvCalcBackProject(planes, channel_temp, object_hist[i]);
      gray_to_yuv(channel_temp, back_projections[i]);
    }

    cv_sdl_show_yuv_tiles(view_grid, N_CAMERAS*2, N_CAMERAS);
  }
}
