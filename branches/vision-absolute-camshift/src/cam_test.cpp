#include <cv_dc1394.h>
#include <cv_sdl.h>

int main() {
  IplImage** images;
  int n_cameras;

  cv_dc1394_init();
  n_cameras = cv_dc1394_get_camera_count();

  /* Capture from all camera, tile the images horizontally */
  while (cv_sdl_process_events()) {
    images = cv_dc1394_capture_yuv(n_cameras);
    cv_sdl_show_yuv_tiles(images, n_cameras, n_cameras);
  }
}
