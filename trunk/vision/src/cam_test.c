#include <cv_dc1394.h>
#include <cv_sdl.h>

int main() {
  IplImage** images;
  cv_dc1394_init();
  while (cv_sdl_process_events()) {
    images = cv_dc1394_capture_yuv(2);
    cv_sdl_show_yuv_tiles(images, 2, 2);
  }
}
