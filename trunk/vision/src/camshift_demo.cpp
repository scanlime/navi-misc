/* Demonstration of histogram backprojection and the CAMSHIFT algorithm */

#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <camshift_frontend.h>
#include <uinput_mouse.h>
#include <getopt.h>

void interactive_camshift(int n_cameras) {
  IplImage **images;
  MultiCamshiftUI *tracker = NULL;
  SDL_Event event;
  bool show_ui = true;
  bool show_backprojections = true;
  bool mirror = false;

  while (1) {
    images = cv_dc1394_capture_yuv(n_cameras);

    if (!tracker)
      tracker = new MultiCamshiftUI(n_cameras, cvGetSize(images[0]));

    if (mirror)
      for (int i=0; i<n_cameras; i++)
	cvFlip(images[i], images[i], 1);

    tracker->updateEventState();

    while (SDL_PollEvent(&event)) {
      if (tracker->processEvent(&event))
	continue;
      if (!cv_sdl_event_handler(&event))
	return;

      switch (event.type) {
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {

	case 'b':
	  show_backprojections = !show_backprojections;
	  break;

	case 'i':
	  mirror = !mirror;
	  break;

	case 'u':
	  show_ui = !show_ui;
	  break;

	}
      }
    }

    tracker->track(images, show_ui, show_backprojections);
  }
}

static void usage(char **argv) {
  printf("Usage: %s [options]\n"
	 "Interactive frontend for OpenCV's CAMSHIFT implementation\n"
	 "\n"
	 "  -c. --cameras N            Set the number of cameras to use\n",
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
      {"cameras",             1, NULL, 'c'},
      NULL,
    };
    c = getopt_long(argc, argv, "hbc:m",
		    long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {

    case 'c':
      n_cameras = atoi(optarg);
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

  interactive_camshift(n_cameras);
  return 0;
}

/* The End */
