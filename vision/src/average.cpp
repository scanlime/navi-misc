#include <cv_dc1394.h>
#include <cv_sdl.h>

int main() {
  IplImage *frame, *accumulator=NULL;

  cv_dc1394_init();

  while (cv_sdl_process_events()) {
    frame = cv_dc1394_capture_yuv(1)[0];

    if (!accumulator)
      accumulator = cvCreateImage(cvGetSize(frame), 32, 3);

    cvRunningAvg(frame, accumulator, 0.1);
    cvConvert(accumulator, frame);

    cv_sdl_show_yuv_image(frame);
  }
}
