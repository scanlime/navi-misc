#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

#define MAX_PORTS   4
#define MAX_CAMERAS 8
#define NUM_BUFFERS 8

#define ORIG_WIN_SIZE  24
static CvMemStorage* storage = 0;
static CvHidHaarClassifierCascade* hid_cascade = 0;

void detect_and_draw( IplImage* left_img, IplImage* right_img );

int main( int argc, char** argv )
{
    int p, i, cam, numCameras=0;
    unsigned int channel, speed;
    IplImage *left_frame, *right_frame, *buffer;
    raw1394handle_t raw_handle;
    struct raw1394_portinfo ports[MAX_PORTS];
    raw1394handle_t handles[MAX_CAMERAS];
    int numPorts = MAX_PORTS;
    dc1394_cameracapture cameras[MAX_CAMERAS];
    nodeid_t *camera_nodes;

    CvHaarClassifierCascade* cascade =
    cvLoadHaarClassifierCascade( "<default_face_cascade>",
                         cvSize( ORIG_WIN_SIZE, ORIG_WIN_SIZE ));
    hid_cascade = cvCreateHidHaarClassifierCascade( cascade, 0, 0, 0, 1 );
    cvReleaseHaarClassifierCascade( &cascade );

    raw_handle = raw1394_new_handle();
    numPorts = raw1394_get_port_info(raw_handle, ports, numPorts);
    raw1394_destroy_handle(raw_handle);

    for (p = 0; p < numPorts; p++)
      {
	int camCount;

	/* get the camera nodes and describe them as we find them */
	raw_handle = raw1394_new_handle();
	raw1394_set_port( raw_handle, p );
	camera_nodes = dc1394_get_camera_nodes(raw_handle, &camCount, 1);
	raw1394_destroy_handle(raw_handle);

	/* setup cameras for capture */
	for (i = 0; i < camCount; i++)
	  {
	    handles[numCameras] = dc1394_create_handle(p);
	    if (handles[numCameras]==NULL) {
	      perror("Unable to aquire a raw1394 handle\n");
	      perror("did you load the drivers?\n");
	      exit(-1);
	    }

	    cameras[numCameras].node = camera_nodes[i];

	    if (dc1394_get_iso_channel_and_speed(handles[numCameras], cameras[numCameras].node,
						 &channel, &speed) != DC1394_SUCCESS) 
	      {
		printf("unable to get the iso channel number\n");
		exit(-1);
	      }

	    if (dc1394_dma_setup_capture(handles[numCameras], cameras[numCameras].node, i+1 /*channel*/,
					 FORMAT_VGA_NONCOMPRESSED, MODE_640x480_RGB,
					 SPEED_400, FRAMERATE_15, NUM_BUFFERS, 1,
					 NULL, &cameras[numCameras]) != DC1394_SUCCESS) 
	      {
		fprintf(stderr, "unable to setup camera- check line %d of %s to make sure\n",
			__LINE__,__FILE__);
		perror("that the video mode,framerate and format are supported\n");
		printf("is one supported by your camera\n");
		exit(-1);
	      }

	    /*have the camera start sending us data*/
	    if (dc1394_start_iso_transmission(handles[numCameras], cameras[numCameras].node) !=DC1394_SUCCESS) 
	      {
		perror("unable to start camera iso transmission\n");
		exit(-1);
	      }
	    numCameras++;
	  }
	dc1394_free_camera_nodes(camera_nodes);
      }

    cvNamedWindow( "result", 1 );
    cvNamedWindow( "right", 1 );
    storage = cvCreateMemStorage(0);

    left_frame = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
    right_frame = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
    buffer = cvCreateImageHeader(cvSize(640,480), IPL_DEPTH_8U, 3);

    for(;;)
      {

	dc1394_dma_multi_capture(cameras, numCameras);

	cvSetData(buffer, cameras[0].capture_buffer, 640*3);
	cvCvtColor(buffer, left_frame, CV_RGB2BGR);

	cvSetData(buffer, cameras[1].capture_buffer, 640*3);
	cvCvtColor(buffer, right_frame, CV_RGB2BGR);

	detect_and_draw( left_frame, right_frame );

	for (i = 0; i < numCameras; i++)
	  dc1394_dma_done_with_buffer(&cameras[i]);

	if( cvWaitKey( 1 ) >= 0 )
	  break;
      }

    cvDestroyWindow("result");

    return 0;
}

void detect_and_draw( IplImage* left_img, IplImage* right_img )
{
    int scale = 2;
    IplImage* left_temp = cvCreateImage( cvSize(left_img->width/2,left_img->height/2), 8, 3 );
    IplImage* right_temp = cvCreateImage( cvSize(right_img->width/2,right_img->height/2), 8, 3 );
    CvPoint pt1, pt2;
    int i;

    cvPyrDown( left_img, left_temp, CV_GAUSSIAN_5x5 );
    cvPyrDown( right_img, right_temp, CV_GAUSSIAN_5x5 );
    cvClearMemStorage( storage );

    if( hid_cascade )
    {
      CvSeq* left_faces = cvHaarDetectObjects( left_temp, hid_cascade, storage,
					       1.2, 2, CV_HAAR_DO_CANNY_PRUNING );
      CvSeq* right_faces = cvHaarDetectObjects( right_temp, hid_cascade, storage,
						1.2, 2, CV_HAAR_DO_CANNY_PRUNING );
      int n_faces;

      if (left_faces && right_faces) {
	if (left_faces->total < right_faces->total)
	  n_faces = left_faces->total;
	else
	  n_faces = right_faces->total;

	for( i = 0; i < n_faces; i++)
	  {
	    int left_x, left_y, right_x, right_y;
	    int dist;
            CvRect* left_r = (CvRect*)cvGetSeqElem( left_faces, i, 0 );
            CvRect* right_r = (CvRect*)cvGetSeqElem( right_faces, i, 0 );

            pt1.x = left_r->x*scale;
            pt2.x = (left_r->x+left_r->width)*scale;
            pt1.y = left_r->y*scale;
            pt2.y = (left_r->y+left_r->height)*scale;
            cvRectangle( left_img, pt1, pt2, CV_RGB(255,0,0), 3 );

	    left_x = left_r->x + left_r->width / 2;
	    left_y = left_r->y + left_r->height / 2;
	    right_x = right_r->x + right_r->width / 2;
	    right_y = right_r->y + right_r->height / 2;
	    dist = sqrt((left_x - right_x) * (left_x - right_x) + (left_y - right_y) * (left_y - right_y));

	    pt1.y = pt2.y + 8;
	    pt2.y = pt1.y + 5;
	    pt2.x = pt1.x + dist;
            cvRectangle( left_img, pt1, pt2, CV_RGB(255,255,0), CV_FILLED);
	  }
      }
    }

    cvShowImage( "result", left_img );
    cvShowImage( "right", right_img );
    cvReleaseImage( &left_temp );
    cvReleaseImage( &right_temp );
}
