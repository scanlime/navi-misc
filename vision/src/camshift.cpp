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
  IplImage *planes[3];
  IplImage *view_grid[n_cameras*2];
  IplImage *backprojection;
  CvHistogram* object_hist[n_cameras];
  int num_views = 0;
  CvRect windowIn[n_cameras];
  int sample_square_size = 20;
  int num_histogram_samples = 0;
  bool uinput_mouse_enabled = false;
  int i;
  CvPoint2D32f prev_box_center;
  bool prev_box_center_valid = false;

  /* Take a reference frame from each camera */
  images = cv_dc1394_capture_yuv(n_cameras);

  /* Allocate images, now that we know the camera resolution */
  for (i=0; i<3; i++)
    planes[i] = cvCreateImage(cvGetSize(images[0]), 8, 1);
  for (i=0; i<n_cameras; i++)
    yuv_backprojections[i] = cvCreateImage(cvGetSize(images[0]), 8, 3);
  backprojection = cvCreateImage(cvGetSize(images[0]), 8, 1);

  /* Allocate histograms */
  for (i=0; i<n_cameras; i++) {
    int hist_size[] = {180, 128, 32};
    float h_range[] = {0, 180};
    float sv_range[] = {0, 255};
    float* ranges[] = {h_range, sv_range, sv_range};
    object_hist[i] = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);
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

  uinput_mouse_init("CamShift");

  while (1) {
    images = cv_dc1394_capture_yuv(n_cameras);

    for (i=0; i<n_cameras; i++) {
      if (mirror)
	cvFlip(images[i], images[i], 1);

      /* Calculate the backprojection, in YUV space */
      cvCvtPixToPlane(images[i], planes[0], planes[1], planes[2], 0);
      cvCalcBackProject(planes, backprojection, object_hist[i]);

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
	  plot_crosshairs(images[i], cvPoint((int)box.center.x, (int)box.center.y));

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

	  if (uinput_mouse_enabled) {
	    /* Emulate a mouse using uinput */
	    float dx = box.center.x - prev_box_center.x;
	    float dy = box.center.y - prev_box_center.y;

	    uinput_mouse_move((int)(dx * 2.5),
			      (int)(dy * 2.5));

	    prev_box_center = box.center;
	    prev_box_center_valid = true;
	  }
	}
	else {
	  /* If not, reset our search window */
	  windowIn[i].x = 0;
	  windowIn[i].y = 0;
	  windowIn[i].width = images[i]->width;
	  windowIn[i].height = images[i]->height;

	  prev_box_center_valid = false;
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

      /* The right mouse button clears our histogram */
      if (mouse_buttons & SDL_BUTTON(3)) {
	cvClearHist(object_hist[mouse_camera]);
	num_histogram_samples = 0;
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
  interactive_camshift(n_cameras, show_backprojections, mirror);
  return 0;
}

/* The End */
