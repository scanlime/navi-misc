#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <cv.hpp>
#include <cvaux.h>

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

class DuckTracker {
  int threshold;

public:
  DuckTracker() {
    threshold = 80;
  }

  bool track(IplImage *image, CvPoint *point,
	     bool show_threshold=true, float threshold_probability=0.01) {
    /* Track a duck, in 2D, in the given YUV image. The image is thresholded
     * in the U plane, and a weighted average of the coordinates of pixels
     * passing the threshold test is taken. Pixels to the right of other
     * passing pixels are given more weight.
     *
     * If the duck is found, 'point' is filled with its position and this
     * returns true.
     */
    unsigned char *p, *row;
    unsigned char ch_y, ch_u, ch_v;
    int x, y;
    int run_length=0;
    int num_selected_pixels=0;
    float weight;
    float x_sum=0, y_sum=0, total=0;

    row = (unsigned char*) image->imageData;
    for (y=0; y<image->height; y++) {
      p = row;
      for (x=0; x<image->width; x++) {
	ch_y = *(p++);
	ch_u = *(p++);
	ch_v = *(p++);

	if (ch_u < threshold) {
	  run_length++;
	  weight = run_length/4 * run_length;

	  x_sum += x * weight;
	  y_sum += y * weight;
	  total += weight;
	  num_selected_pixels++;
	}
	else {
	  run_length = 0;

	  /* We can optionally illustrate the threshold by showing
	   * unselected pixels in darkened monochrome.
	   */
	  if (show_threshold) {
	    ch_y = ch_y >> 1;
	    ch_u = 128;
	    ch_v = 128;

	    p[-3] = ch_y;
	    p[-2] = ch_u;
	    p[-1] = ch_v;
	}
	}
      }
      row += image->widthStep;
    }

    /* Automatic threshold adjustment so that the probability
     * a pixel passes the threshold test is threshold_probability
     */
    if (num_selected_pixels < image->width*image->height * threshold_probability)
      threshold++;
    else
      threshold--;

    if (total > 4000) {
      point->x = (int)(x_sum / total + 0.5);
      point->y = (int)(y_sum / total + 0.5);
      return true;
    }
    return false;
  }

  void track_and_show(IplImage *image) {
    /* Track a duck, showing its position on the image */
    CvPoint point;

    if (this->track(image, &point)) {
      plot_crosshairs(image, &point);
    }
  }
};

int main() {
  IplImage** images;
  int i;
  int n_cameras;

  cv_dc1394_init();
  n_cameras = cv_dc1394_get_camera_count();

  DuckTracker trackers[n_cameras];

  while (cv_sdl_process_events()) {
    images = cv_dc1394_capture_yuv(n_cameras);

    for (i=0; i<n_cameras; i++)
      trackers[i].track_and_show(images[i]);

    cv_sdl_show_yuv_tiles(images, n_cameras, n_cameras);
  }
}
