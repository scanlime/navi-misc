/* Demonstration of histogram backprojection and the CAMSHIFT algorithm */

#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <camshift_frontend.h>
#include <uinput_mouse.h>
#include <getopt.h>

void interactive_camshift(int n_cameras = 1,
			  bool show_backprojections = true,
			  bool mirror = false) {
  IplImage **images;
  MultiCamshiftUI *tracker = NULL;

  do {
    images = cv_dc1394_capture_yuv(n_cameras);

    if (!tracker)
      tracker = new MultiCamshiftUI(n_cameras, cvGetSize(images[0]));

    if (mirror)
      for (int i=0; i<n_cameras; i++)
	cvFlip(images[i], images[i], 1);

    tracker->track(images);
  } while (tracker->processEvents());
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
