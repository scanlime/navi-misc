/* Modified version of the face tracking example that comes with OpenCV */

#include <cv_dc1394.h>
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

#define ORIG_WIN_SIZE  24
static CvMemStorage* storage = 0;
static CvHidHaarClassifierCascade* hid_cascade = 0;

void detect_and_draw( IplImage* image );

int main( int argc, char** argv )
{
  CvHaarClassifierCascade* cascade =
    cvLoadHaarClassifierCascade( "<default_face_cascade>",
				 cvSize( ORIG_WIN_SIZE, ORIG_WIN_SIZE ));
  hid_cascade = cvCreateHidHaarClassifierCascade( cascade, 0, 0, 0, 1 );
  cvReleaseHaarClassifierCascade( &cascade );

  cv_dc1394_init();

  cvNamedWindow( "result", 1 );
  storage = cvCreateMemStorage(0);

  while (1) {
    detect_and_draw( cv_dc1394_capture_rgb(1)[0] );
    cvWaitKey(1);
  }

  cvDestroyWindow("result");

  return 0;
}

void detect_and_draw( IplImage* image )
{
    int scale = 2;
    IplImage* temp = cvCreateImage( cvSize(image->width/2,image->height/2), 8, 3 );
    CvPoint pt1, pt2;
    int i;

    cvPyrDown( image, temp, CV_GAUSSIAN_5x5 );
    cvClearMemStorage( storage );

    if( hid_cascade )
    {
      CvSeq* faces = cvHaarDetectObjects( temp, hid_cascade, storage,
					  1.2, 2, CV_HAAR_DO_CANNY_PRUNING );
      for( i = 0; i < faces->total; i++)
	{
	  int x, y;
	  int dist;
	  CvRect* r = (CvRect*)cvGetSeqElem( faces, i, 0 );

	  pt1.x = r->x*scale;
	  pt2.x = (r->x+r->width)*scale;
	  pt1.y = r->y*scale;
	  pt2.y = (r->y+r->height)*scale;
	  cvRectangle( image, pt1, pt2, CV_RGB(255,0,0), 3 );
	}
    }

    cvShowImage( "result", image );
    cvReleaseImage( &temp );
}
