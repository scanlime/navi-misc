/* Demonstration of histogram backprojection and the CAMSHIFT algorithm */

#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <uinput_mouse.h>
#include <cv.hpp>
#include <cvaux.h>
#include <SDL.h>
#include <getopt.h>

void plot_crosshairs(IplImage *image, CvPoint point,
		     int size=15, double color=CV_RGB(255,0,128),
		     int thickness=1) {
  /* Show some simple crosshairs on an image at the given point */
  CvPoint endpoint1, endpoint2;
  endpoint1.x = point.x - size/2;
  endpoint1.y = point.y;
  endpoint2.x = point.x + size/2;
  endpoint2.y = point.y;
  cvLine(image, endpoint1, endpoint2, color, thickness);
  endpoint1.y = point.y - size/2;
  endpoint1.x = point.x;
  endpoint2.y = point.y + size/2;
  endpoint2.x = point.x;
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

void yuv_to_hs(IplImage *yuv, IplImage **planes,
		unsigned char ignored = 255,
		unsigned char min_saturation = 10,
		unsigned char min_value = 10,
		unsigned char max_value = 245) {
  /* Convert a YUV image to hue and saturation planes.
   * Anywhere the value component is too high or too low, or where saturation is too
   * low, the hue is set to the given 'ignored' value.
   */
  static IplImage *temp = 0;
  IplImage *hue = planes[0];
  IplImage *saturation = planes[1];

  if (!temp)
    temp = cvCreateImage(cvGetSize(yuv), 8, 3);
  cvCvtColor(yuv, temp, CV_YCrCb2RGB);
  cvCvtColor(yuv, temp, CV_RGB2HSV);

  unsigned char *src_p,  *src_row  = (unsigned char*) temp->imageData;
  unsigned char *hue_p,  *hue_row  = (unsigned char*) hue->imageData;
  unsigned char *saturation_p,  *saturation_row  = (unsigned char*) saturation->imageData;
  unsigned char h, s, v;
  int i, j;
  for (j=temp->height; j; --j) {
    src_p = src_row;
    hue_p = hue_row;
    saturation_p = saturation_row;
    for (i=temp->width; i; --i) {
      h = *(src_p++);
      s = *(src_p++);
      v = *(src_p++);

      if (s < min_saturation || v < min_value || v > max_value)
	h = ignored;

      *(hue_p++) = h;
      *(saturation_p++) = s;
    }
    src_row += temp->widthStep;
    hue_row += hue->widthStep;
    saturation_row += saturation->widthStep;
  }
}

void draw_box(IplImage *image, CvBox2D box, double color=CV_RGB(0,255,128)) {
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

void map_mouse_position(int *x, int *y, int *camera, IplImage *image, int grid_w, int grid_h) {
  /* Given a mouse position in window coordinates, return
   * a mouse position in image coordiantes and a camera number.
   * A sample image must be provided so this knows our camera resolution.
   */
  SDL_Surface *surface = SDL_GetVideoSurface();

  /* Convert the mouse coordinates to be relative to the YUV surface,
   * undoing any resizing the user may have performed on our window.
   */
  *x = *x * image->width * grid_w / surface->w;
  *y = *y * image->height * grid_h / surface->h;

  /* Assuming the cameras are horizontally tiled, figure out
   * the camera we're clicking in and the position within that camera's frame
   */
  *camera = *x / image->width;
  *x %= image->width;
  *y %= image->height;
}

void interactive_camshift(int n_cameras = 1,
			  bool show_backprojections = true,
			  bool mirror = false) {
  IplImage **images;
  IplImage *yuv_backprojections[n_cameras];
  IplImage *planes[2];
  IplImage *view_grid[n_cameras*2];
  IplImage *backprojection;
  CvHistogram* object_hist[n_cameras];
  int num_views = 0;
  CvRect windowIn[n_cameras];
  int sample_square_size = 20;
  int num_histogram_samples = 0;
  bool uinput_mouse_enabled = false;
  int i;
  CvKalman* fast_kalman = cvCreateKalman(2, 2, 0);
  CvKalman* slow_kalman = cvCreateKalman(2, 2, 0);
  CvMat* measurement = cvCreateMat( 2, 1, CV_32FC1 );

  /* Take a reference frame from each camera */
  images = cv_dc1394_capture_yuv(n_cameras);

  /* Initialize the Kalman filters. We have two filters:
   * a fast filter for keeping track of the current object position,
   * and a slower filter to use as a center position for measuring
   * the displacement of the object.
   */
  cvSetIdentity(fast_kalman->measurement_matrix, cvRealScalar(1));
  cvSetIdentity(fast_kalman->process_noise_cov, cvRealScalar(0.4));
  cvSetIdentity(fast_kalman->measurement_noise_cov, cvRealScalar(3));
  cvSetIdentity(fast_kalman->error_cov_post, cvRealScalar(1));

  cvSetIdentity(slow_kalman->measurement_matrix, cvRealScalar(1));
  cvSetIdentity(slow_kalman->process_noise_cov, cvRealScalar(0.001));
  cvSetIdentity(slow_kalman->measurement_noise_cov, cvRealScalar(3));
  cvSetIdentity(slow_kalman->error_cov_post, cvRealScalar(1));

  /* Allocate images, now that we know the camera resolution */
  for (i=0; i<2; i++)
    planes[i] = cvCreateImage(cvGetSize(images[0]), 8, 1);
  for (i=0; i<n_cameras; i++)
    yuv_backprojections[i] = cvCreateImage(cvGetSize(images[0]), 8, 3);
  backprojection = cvCreateImage(cvGetSize(images[0]), 8, 1);

  /* Allocate histograms */
  for (i=0; i<n_cameras; i++) {
    int hist_size[] = {182, 16}; /* Hue, saturation */
    float h_range[] = {0, 181};
    float s_range[] = {0, 255};
    float* ranges[] = {h_range, s_range};
    object_hist[i] = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
  }

  /* Initialize the search windows for CAMSHIFT. Subsequently these
   * will be set to the previously located image, to exploit
   * frame coherence for speed.
   */
  for (i=0; i<n_cameras; i++) {
    windowIn[i].x = 0;
    windowIn[i].y = 0;
    windowIn[i].width = images[i]->width;
    windowIn[i].height = images[i]->height;
  }

  /* Tile cameras horizontally, with original image on
   * top and backprojection on bottom.
   */
  for (i=0; i<n_cameras; i++)
    view_grid[num_views++] = images[i];
  if (show_backprojections)
    for (i=0; i<n_cameras; i++)
      view_grid[num_views++] = yuv_backprojections[i];

  while (1) {
    images = cv_dc1394_capture_yuv(n_cameras);

    for (i=0; i<n_cameras; i++) {
      if (mirror)
	cvFlip(images[i], images[i], 1);

      /* Calculate the backprojection, in YUV space */
      yuv_to_hs(images[i], planes);
      cvCalcBackProject(planes, backprojection, object_hist[i]);
      cvThreshold(backprojection, backprojection, 16, 255, CV_THRESH_TOZERO);

      if (show_backprojections) {
	/* Make a YUV version of the output, for display */
	gray_to_yuv(backprojection, yuv_backprojections[i]);
      }

      if (num_histogram_samples > 0) {
	/* Run the output through the CAMSHIFT algorithm to locate objects */
	CvBox2D box;
	CvConnectedComp comp;
	cvCamShift(backprojection, windowIn[i],
		   cvTermCriteria(CV_TERMCRIT_ITER, 20, 0),
		   &comp, &box);
	windowIn[i] = comp.rect;

	if (box.size.width > 0 && box.size.height > 0) {
	  /* If we found something, draw it */

	  /* Check the roll quality. For aspect ratios near 1, the box rotation
	   * can't be measured accurately. In these situations, we draw the box
	   * as a circle, to convey this information.
	   */
	  float q = box.size.height / box.size.width;
	  if (q < 1.25) {
	    cvCircleAA(images[i], cvPoint((int)box.center.x, (int)box.center.y),
		       (int)((box.size.height + box.size.width)/4), CV_RGB(0,255,128));
	  }
	  else {
	    draw_box(images[i], box);
	  }

	  /* Feed the current center point into our kalman filters.
	   * Draw crosshairs at the point predicted by the fast filter,
	   * and draw a line from that point to the one predicted by the slow filter.
	   */
	  const CvMat* fast_prediction = cvKalmanPredict(fast_kalman);
	  const CvMat* slow_prediction = cvKalmanPredict(slow_kalman);
	  cvLineAA(images[i],
		   cvPoint((int)cvmGet(fast_prediction, 0, 0),
			   (int)cvmGet(fast_prediction, 1, 0)),
		   cvPoint((int)cvmGet(slow_prediction, 0, 0),
			   (int)cvmGet(slow_prediction, 1, 0)),
		   CV_RGB(0,128,128));
	  plot_crosshairs(images[i],
			  cvPoint((int)cvmGet(fast_prediction, 0, 0),
				  (int)cvmGet(fast_prediction, 1, 0)));
	  cvmSet(measurement, 0, 0, box.center.x);
	  cvmSet(measurement, 1, 0, box.center.y);
	  cvKalmanCorrect(fast_kalman, measurement);
	  cvKalmanCorrect(slow_kalman, measurement);

	  if (uinput_mouse_enabled) {
	    /* Emulate a mouse using uinput */
	    float dx = cvmGet(fast_prediction, 0, 0) - cvmGet(slow_prediction, 0, 0);
	    float dy = cvmGet(fast_prediction, 1, 0) - cvmGet(slow_prediction, 1, 0);
	    uinput_mouse_move_subpixel(dx, dy);
	  }
	}
	else {
	  /* If not, reset our search window */
	  windowIn[i].x = 0;
	  windowIn[i].y = 0;
	  windowIn[i].width = images[i]->width;
	  windowIn[i].height = images[i]->height;
	}
      }
    }

    if (SDL_GetVideoSurface()) {
      /* Interactive interface for modifying the histogram,
       * only works after we've initialized our video by displaying
       * the first frame.
       */

      Uint8 *keystate = SDL_GetKeyState(NULL);
      CvRect sample_rect;
      int mouse_x, mouse_y, mouse_camera, mouse_buttons;
      bool show_rectangle = false;
      SDL_Event event;

      /* Process SDL events... */
      while (SDL_PollEvent(&event))
	switch (event.type) {

	case SDL_MOUSEBUTTONDOWN:
	  /* Use the scroll wheel to change sample square size */
	  switch (event.button.button) {
	  case 5:
	    sample_square_size -= 2;
	    if (sample_square_size < 1)
	      sample_square_size = 1;
	    show_rectangle = true;
	    break;

	  case 4:
	    sample_square_size += 2;
	    show_rectangle = true;
	    break;
	  }
	  break;

	case SDL_KEYDOWN:
	  switch (event.key.keysym.sym) {
	    /* The 'm' key toggles mouse emulation */
	  case 'm':
	    uinput_mouse_enabled = !uinput_mouse_enabled;
	    break;
	  }
	  break;

	default:
	  if (!cv_sdl_event_handler(&event))
	    return;
	}

      /* The spacebar just shows the rectangle */
      if (keystate[' '])
	show_rectangle = true;

      /* Get the current sampling rect, centered on the mouse cursor */
      mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
      map_mouse_position(&mouse_x, &mouse_y, &mouse_camera, images[0],
			 n_cameras, show_backprojections ? 2 : 1);
      sample_rect.x = mouse_x - sample_square_size/2;
      sample_rect.y = mouse_y - sample_square_size/2;
      sample_rect.width = sample_square_size;
      sample_rect.height = sample_square_size;
      if (sample_rect.x < 0) sample_rect.x = 0;
      if (sample_rect.y < 0) sample_rect.y = 0;
      if (sample_rect.x > images[0]->width - sample_rect.width - 1)
	sample_rect.x = images[0]->width - sample_rect.width - 1;
      if (sample_rect.y > images[0]->height - sample_rect.height - 1)
	sample_rect.y = images[0]->height - sample_rect.height - 1;

      /* The right mouse button clears our histogram */
      if (mouse_buttons & SDL_BUTTON(3)) {
	cvClearHist(object_hist[mouse_camera]);
	num_histogram_samples = 0;
      }

      /* If the left mouse button is down, use this image to collect samples towards a
       * reference histogram for the object we're trying to track.
       */
      if (mouse_buttons & SDL_BUTTON(1)) {
	cvSetImageROI(planes[0], sample_rect);
	cvSetImageROI(planes[1], sample_rect);
	cvCalcHist(planes, object_hist[mouse_camera], 1);
	cvResetImageROI(planes[0]);
	cvResetImageROI(planes[1]);

	/* Also set the windowIn to the sampling rectangle, to point CAMSHIFT at
	 * what we're interested in.
	 */
	windowIn[mouse_camera] = sample_rect;

	show_rectangle = true;
	num_histogram_samples++;
      }

      /* Draw a box around the current sampling rectangle */
      if (show_rectangle)
	cvRectangle(images[mouse_camera], cvPoint(sample_rect.x-1, sample_rect.y-1),
		    cvPoint(sample_rect.x + sample_rect.width + 1, sample_rect.y + sample_rect.width + 1),
		    CV_RGB(128,128,255), 1);
    }

    cv_sdl_show_yuv_tiles(view_grid, num_views, n_cameras);
  }
}

static void usage(char **argv) {
  printf("Usage: %s [options]\n"
	 "Interactive frontend for OpenCV's CAMSHIFT implementation\n"
	 "\n"
	 "  -b, --hide-backprojection  Hide the backprojection images shown below the cameras\n"
	 "  -c. --cameras N            Set the number of cameras to use\n"
	 "  -m, --mirror               Mirror mode, flips the video horizontally\n",
	 argv[0]);
}

int main(int argc, char **argv) {
  int c, option_index=0;
  int n_cameras = 1;
  bool show_backprojections = true;
  bool mirror = false;

  while (1) {
    static struct option long_options[] = {
      {"help",                0, NULL, 'h'},
      {"hide-backprojection", 0, NULL, 'b'},
      {"cameras",             1, NULL, 'c'},
      {"mirror",              0, NULL, 'm'},
      NULL,
    };
    c = getopt_long(argc, argv, "hbc:m",
		    long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {

    case 'b':
      show_backprojections = false;
      break;

    case 'c':
      n_cameras = atoi(optarg);
      break;

    case 'm':
      mirror = true;
      break;

    case 'h':
    default:
      usage(argv);
      return 1;
    }
  }

  if (optind < argc) {
    usage(argv);
    return 1;
  }

  cv_dc1394_init();
  uinput_mouse_init("CamShift");

  interactive_camshift(n_cameras, show_backprojections, mirror);
  return 0;
}

/* The End */
