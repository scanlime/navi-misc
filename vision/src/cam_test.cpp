#include <cv_dc1394.h>
#include <cv_sdl.h>

int main() {
  IplImage** images;
  cv_dc1394_init();
  while (cv_sdl_process_events()) {
    /* Capture images from two cameras, display them in a 2x1 grid */
    images = cv_dc1394_capture_yuv(2);
    cv_sdl_show_yuv_tiles(images, 2, 2);
  }
}
