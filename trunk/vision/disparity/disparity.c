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
#define NUM_BUFFERS 10


int main( int argc, char** argv )
{
    int p, i, cam, numCameras=0;
    unsigned int channel, speed;
    IplImage *left_frame, *right_frame, *depth, *buffer;
    raw1394handle_t raw_handle;
    struct raw1394_portinfo ports[MAX_PORTS];
    raw1394handle_t handles[MAX_CAMERAS];
    int numPorts = MAX_PORTS;
    dc1394_cameracapture cameras[MAX_CAMERAS];
    nodeid_t *camera_nodes;

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
					 FORMAT_VGA_NONCOMPRESSED, MODE_640x480_MONO,
					 SPEED_400, FRAMERATE_30, NUM_BUFFERS, 1,
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

    cvNamedWindow( "left", 1 );
    cvNamedWindow( "right", 1 );
    cvNamedWindow( "disparity", 1 );

    buffer = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
    depth = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 1);
    left_frame = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);
    right_frame = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);


    for(;;)
      {

	dc1394_dma_multi_capture(cameras, numCameras);

	cvSetData(buffer, cameras[0].capture_buffer, 640);
	cvPyrDown(buffer, left_frame, CV_GAUSSIAN_5x5);
	cvSetData(buffer, cameras[1].capture_buffer, 640);
	cvPyrDown(buffer, right_frame, CV_GAUSSIAN_5x5);

	cvFindStereoCorrespondence(left_frame, right_frame, CV_DISPARITY_BIRCHFIELD, depth,
				   10, 25, 5, 12, 15, 25);

	cvShowImage( "disparity", depth );
	cvShowImage( "left", left_frame );
	cvShowImage( "right", right_frame );

	for (i = 0; i < numCameras; i++)
	  dc1394_dma_done_with_buffer(&cameras[i]);
	if( cvWaitKey( 1 ) >= 0 )
	  break;
      }

    cvDestroyWindow("result");

    return 0;
}
