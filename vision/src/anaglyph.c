#include <cv_dc1394.h>
#include <cv_sdl.h>

void combine_anaglyph(IplImage *left, IplImage *right, IplImage *dest) {
  unsigned char *left_p,  *left_row  = (unsigned char*) left->imageData;
  unsigned char *right_p, *right_row = (unsigned char*) right->imageData;
  unsigned char *dest_p,  *dest_row  = (unsigned char*) dest->imageData;
  int l_y, l_u, l_v, r_y, r_u, r_v, r, g, b;
  int i, j;

  for (j=dest->height; j; --j) {
    left_p = left_row;
    right_p = right_row;
    dest_p = dest_row;
    for (i=dest->width; i; --i) {

      l_y = *(left_p++);
      l_u = *(left_p++);
      l_v = *(left_p++);

      r_y = *(right_p++);
      r_u = *(right_p++);
      r_v = *(right_p++);

      r = l_y;
      g = l_y / 64;
      b = r_y;

      *(dest_p++) = b;
      *(dest_p++) = g;
      *(dest_p++) = r;

    }
    left_row += left->widthStep;
    right_row += right->widthStep;
    dest_row += dest->widthStep;
  }
}

int main() {
  IplImage** images;
  cv_dc1394_init();

  while (cv_sdl_process_events()) {
    images = cv_dc1394_capture_yuv(2);
    combine_anaglyph(images[0], images[1], images[0]);

    cvCvtColor(images[0], images[0], CV_RGB2YCrCb);
    cv_sdl_show_yuv_image(images[0]);
  }
}
