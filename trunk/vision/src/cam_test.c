#include <cv_dc1394.h>
#include <highgui.h>

int main() {
  IplImage** images;

  cv_dc1394_init();
  cvNamedWindow( "left", 1 );
  cvNamedWindow( "right", 1 );

  while (1) {
    images = cv_dc1394_capture_rgb(2);
    cvShowImage( "left", images[0] );
    cvShowImage( "right", images[1] );
    cvWaitKey(1);
  }
}
