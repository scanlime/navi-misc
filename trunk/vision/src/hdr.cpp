/* Simple test app for high dynamic range imaging using relatively cheap
 * cameras. This can be used to photograph very dark scenes or scenes with
 * a wide brightness range by exploiting CCD noise. While noise in the
 * camera's CCD causes almost no image to be observable, the object
 * being photographed does affect the noise's mean value. This effect is
 * similar to taking very long exposures with a film camera: it is only
 * effective on very still objects.
 *
 * This needs to be run once with the camera's lens covered to capture
 * a black level for the CCD. This will be saved to "hdr.result". Rename
 * it and pass that filename on the command line to future invocations.
 * This black level will need to be recalibrated if any of the camera's
 * settings change, and a more accurate black level is necessary to get
 * more accurate final images.
 *
 * While running, the slider adjusts the scale factor (x10) used to display
 * the HDR image being built. Press any key to quit capturing and write the
 * result to hdr.result, an OpenCV FileStorage containing a single "image"
 * in 32-bit floating point.
 */

#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <highgui.h>

int image_scale = 10;
IplImage *image = NULL, *accumulator = NULL;

void update_image() {
  static IplImage *result = NULL;
  if (!result)
    result = cvCreateImage(cvGetSize(image), 8, 3);
  cvScale(image, result, image_scale / 10.0);
  cvShowImage("image", result);
}

void on_trackbar(int h) {
  update_image();
}

void sample_to_accumulator(IplImage **acc) {
  IplImage *frame;
  const int total_frames = 100;
  int i;

  for (i=0; i<total_frames; i++) {
    printf("%d / %d\n", i+1, total_frames);
  }
  cvScale(*acc, *acc,  1.0 / total_frames);
}

int main(int argc, char **argv) {
  IplImage *frame, *black_level = NULL;
  int frame_count = 0;

  cv_dc1394_set_framerate(FRAMERATE_15);
  cv_dc1394_set_mode(MODE_640x480_RGB);

  cv_dc1394_init();
  cvNamedWindow("image", 1);
  cvCreateTrackbar("scale", "image", &image_scale, 1000, on_trackbar);

  if (argv[1]) {
    CvFileStorage *fs = cvOpenFileStorage(argv[1], NULL, CV_STORAGE_READ);
    black_level = (IplImage*) cvRead(fs, "image");
  }

  while (cvWaitKey(1) < 0) {
    for (int i=0; i<5; i++) {
      frame = cv_dc1394_capture_rgb_raw(1)[0];

      if (!accumulator)
	accumulator = cvCreateImage(cvGetSize(frame), 32, 3);
      if (!image)
	image = cvCreateImage(cvGetSize(frame), 32, 3);

      cvAcc(frame, accumulator);
      frame_count++;
    }
    cvScale(accumulator, image, 1.0 / frame_count);
    if (black_level)
      cvSub(image, black_level, image);

    update_image();
  }

  CvFileStorage *fs = cvOpenFileStorage("hdr.result", NULL, CV_STORAGE_WRITE_BINARY);
  cvWrite(fs, "image", image);
  cvReleaseFileStorage(&fs);
}

