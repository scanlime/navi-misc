/**************************************************************************
**       Title: display video from multiple cameras/multiple cards
**    $RCSfile: dc1394_multiview.c,v $
**   $Revision: 1.7 $$Name:  $
**       $Date: 2004/01/20 16:14:01 $
**   Copyright: LGPL $Author: ddennedy $
** Description:
**
**    View format0-only camera video from one camera on one card,
**    muliple cameras on one card, or multiple cameras on multiple cards.
**
** TODO:
**    - Option to tile displays instead vertical stacking.
**
**-------------------------------------------------------------------------
**
**  $Log: dc1394_multiview.c,v $
**  Revision 1.7  2004/01/20 16:14:01  ddennedy
**  fix segfault in dc1394_multiview
**
**  Revision 1.6  2004/01/20 04:12:27  ddennedy
**  added dc1394_free_camera_nodes and applied to examples
**
**  Revision 1.5  2003/09/02 23:42:36  ddennedy
**  cleanup handle destroying in examples; fix dc1394_multiview to use handle per camera; new example
**
**  Revision 1.4  2002/07/27 21:24:51  ddennedy
**  just increase buffers some to reduce chance of hangs
**
**  Revision 1.3  2002/07/27 04:45:07  ddennedy
**  added drop_frames option to dma capture, prepare versions/NEWS for 0.9 release
**
**  Revision 1.2  2002/07/24 02:22:40  ddennedy
**  cleanup, add drop frame support to dc1394_multiview
**
**  Revision 1.1  2002/07/22 02:57:02  ddennedy
**  added examples/dc1394_multiview to test/demonstrate dma multicapture over multiple ports
**
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xvlib.h>
#include <X11/keysym.h>

#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>


/* uncomment the following to drop frames to prevent delays */
#define DROP_FRAMES 1
#define MAX_PORTS   4
#define MAX_CAMERAS 8
#define NUM_BUFFERS 8

/* ok the following constant should be by right included thru in Xvlib.h */
#ifndef XV_YV12
#define XV_YV12 0x32315659
#endif

#ifndef XV_YUY2
#define XV_YUY2 0x32595559
#endif

#ifndef XV_UYVY
#define XV_UYVY 0x59565955
#endif


/* declarations for libdc1394 */
int numPorts = MAX_PORTS;
raw1394handle_t handles[MAX_CAMERAS];
int numCameras = 0;
dc1394_cameracapture cameras[MAX_CAMERAS];
nodeid_t *camera_nodes;
dc1394_feature_set features;

/* declarations for video1394 */
char *device_name=NULL;

/* declarations for Xwindows */
Display *display=NULL;
Window window=(Window)NULL;
long width,height;
long device_width,device_height;
int connection=-1;
XvImage *xv_image=NULL;
XvAdaptorInfo *info;
GC gc;


/* Other declarations */
long frame_length;
long frame_free;
int frame=0;
int adaptor=-1;

int freeze=0;
int average=0;
unsigned char frame_buffer[640*480*4];


int cam0_r = 255;
int cam0_g = 4;
int cam0_b = 0;
int cam1_r = 0;
int cam1_g = 0;
int cam1_b = 255;

/* macro by Bart Nabbe */
#define RGB2YUV(r, g, b, y, u, v)\
  y = (9798*r + 19235*g + 3736*b)  / 32768;\
  u = (-4784*r - 9437*g + 14221*b)  / 32768 + 128;\
  v = (20218*r - 16941*g - 3277*b) / 32768 + 128;\
  y = y < 0 ? 0 : y;\
  u = u < 0 ? 0 : u;\
  v = v < 0 ? 0 : v;\
  y = y > 255 ? 255 : y;\
  u = u > 255 ? 255 : u;\
  v = v > 255 ? 255 : v

void display_frames()
{
	int i;

	if(!freeze && adaptor>=0){
		{
		  unsigned char *cam0 = (unsigned char*) cameras[0].capture_buffer;
		  unsigned char *cam1 = (unsigned char*) cameras[1].capture_buffer;
		  unsigned char *dest = frame_buffer;
		  int i=0,j=0;
		  const int NumPixels = device_width*device_height;
		  int y0_0, y1_0, y2_0, y3_0, u_0, v_0;
		  int y0, y1, y2, y3, u, v;
		  int y0_1, y1_1, y2_1, y3_1, u_1, v_1;
		  int r,g,b;
		  while (i < NumPixels*3/2)
		    {
		      u_1  = cam1[i];
		      u_0  = cam0[i++];
		      y0_1 = cam1[i];
		      y0_0 = cam0[i++];
		      y1_1 = cam1[i];
		      y1_0 = cam0[i++];
		      v_1  = cam1[i];
		      v_0  = cam0[i++];
		      y2_1 = cam1[i];
		      y2_0 = cam0[i++];
		      y3_1 = cam1[i];
		      y3_0 = cam0[i++];

		      r = (y0_0 * cam0_r + y0_1 * cam1_r) >> 8;
		      g = (y0_0 * cam0_g + y0_1 * cam1_g) >> 8;
		      b = (y0_0 * cam0_b + y0_1 * cam1_b) >> 8;
		      RGB2YUV(r,g,b,y0,u,v);
		      dest[j++] = y0;
		      dest[j++] = u;
		      dest[j++] = y0;
		      dest[j++] = v;

		      r = (y1_0 * cam0_r + y1_1 * cam1_r) >> 8;
		      g = (y1_0 * cam0_g + y1_1 * cam1_g) >> 8;
		      b = (y1_0 * cam0_b + y1_1 * cam1_b) >> 8;
		      RGB2YUV(r,g,b,y0,u,v);
		      dest[j++] = y0;
		      dest[j++] = u;
		      dest[j++] = y0;
		      dest[j++] = v;

		      r = (y2_0 * cam0_r + y2_1 * cam1_r) >> 8;
		      g = (y2_0 * cam0_g + y2_1 * cam1_g) >> 8;
		      b = (y2_0 * cam0_b + y2_1 * cam1_b) >> 8;
		      RGB2YUV(r,g,b,y0,u,v);
		      dest[j++] = y0;
		      dest[j++] = u;
		      dest[j++] = y0;
		      dest[j++] = v;

		      r = (y3_0 * cam0_r + y3_1 * cam1_r) >> 8;
		      g = (y3_0 * cam0_g + y3_1 * cam1_g) >> 8;
		      b = (y3_0 * cam0_b + y3_1 * cam1_b) >> 8;
		      RGB2YUV(r,g,b,y0,u,v);
		      dest[j++] = y0;
		      dest[j++] = u;
		      dest[j++] = y0;
		      dest[j++] = v;
		    }
		}

		xv_image=XvCreateImage(display,info[adaptor].base_id, XV_YUY2, frame_buffer,
			device_width*2, device_height);
		XvPutImage(display,info[adaptor].base_id,window,gc,xv_image,
			0,0,device_width*2,device_height,
			0,0,width,height);

		xv_image=NULL;
	}
}


/* macro by Bart Nabbe */
#define RGB2YUV(r, g, b, y, u, v)\
  y = (9798*r + 19235*g + 3736*b)  / 32768;\
  u = (-4784*r - 9437*g + 14221*b)  / 32768 + 128;\
  v = (20218*r - 16941*g - 3277*b) / 32768 + 128;\
  y = y < 0 ? 0 : y;\
  u = u < 0 ? 0 : u;\
  v = v < 0 ? 0 : v;\
  y = y > 255 ? 255 : y;\
  u = u > 255 ? 255 : u;\
  v = v > 255 ? 255 : v

void QueryXv()
{
	int num_adaptors;
	int num_formats;
	XvImageFormatValues *formats=NULL;
	int i,j;
	char xv_name[5];

	XvQueryAdaptors(display,DefaultRootWindow(display),&num_adaptors,&info);

	for(i=0;i<num_adaptors;i++) {
		formats=XvListImageFormats(display,info[i].base_id,&num_formats);
		for(j=0;j<num_formats;j++) {
			xv_name[4]=0;
			memcpy(xv_name,&formats[j].id,4);
			fprintf(stderr,"Xv supported 0x%x %s %s\n",formats[j].id,xv_name,(formats[j].format==XvPacked)?"packed":"planar");
			if(formats[j].id==XV_YUY2) {
				fprintf(stderr,"using Xv format 0x%x %s %s\n",formats[j].id,xv_name,(formats[j].format==XvPacked)?"packed":"planar");
				if(adaptor<0)adaptor=i;
			}
		}
	}
		XFree(formats);
	if(adaptor<0)
		fprintf(stderr,"No suitable Xv adaptor found");
}


void cleanup(void) {
	int i;
	for (i=0; i < numCameras; i++)
	{
		dc1394_dma_unlisten( handles[i], &cameras[i] );
		dc1394_dma_release_camera( handles[i], &cameras[i]);
		dc1394_destroy_handle(handles[i]);
	}
	if ((void *)window != NULL)
		XUnmapWindow(display,window);
	if (display != NULL)
		XFlush(display);
	if (frame_buffer != NULL)
		free( frame_buffer );
}


/* trap ctrl-c */
void signal_handler( int sig) {
	signal( SIGINT, SIG_IGN);
	cleanup();
	exit(0);
}

int main(int argc,char *argv[])
{
	XEvent xev;
	XGCValues xgcv;
	long background=0x010203;
	unsigned int channel;
	unsigned int speed;
	int i, p;
	raw1394handle_t raw_handle;
	struct raw1394_portinfo ports[MAX_PORTS];

	/* get the number of ports (cards) */
	raw_handle = raw1394_new_handle();
	if (raw_handle==NULL) {
	  perror("Unable to aquire a raw1394 handle\n");
	  perror("did you load the drivers?\n");
	  exit(-1);
	}

	numPorts = raw1394_get_port_info(raw_handle, ports, numPorts);
	raw1394_destroy_handle(raw_handle);
	printf("number of ports = %d\n", numPorts);

	/* get dc1394 handle to each port */
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
				cleanup();
				exit(-1);
			}

			cameras[numCameras].node = camera_nodes[i];

			if(dc1394_get_camera_feature_set(handles[numCameras], cameras[numCameras].node, &features) !=DC1394_SUCCESS)
			{
				printf("unable to get feature set\n");
			} else {
				dc1394_print_feature_set(&features);
			}

			if (dc1394_get_iso_channel_and_speed(handles[numCameras], cameras[numCameras].node,
										 &channel, &speed) != DC1394_SUCCESS)
			{
				printf("unable to get the iso channel number\n");
				cleanup();
				exit(-1);
			}

			if (dc1394_dma_setup_capture(handles[numCameras], cameras[numCameras].node, i+1 /*channel*/,
									FORMAT_VGA_NONCOMPRESSED, MODE_640x480_YUV411,
									SPEED_400, FRAMERATE_30, NUM_BUFFERS, DROP_FRAMES,
									device_name, &cameras[numCameras]) != DC1394_SUCCESS)
			{
				fprintf(stderr, "unable to setup camera- check line %d of %s to make sure\n",
					   __LINE__,__FILE__);
				perror("that the video mode,framerate and format are supported\n");
				printf("is one supported by your camera\n");
				cleanup();
				exit(-1);
			}

			/*have the camera start sending us data*/
			if (dc1394_start_iso_transmission(handles[numCameras], cameras[numCameras].node) !=DC1394_SUCCESS)
			{
				perror("unable to start camera iso transmission\n");
				cleanup();
				exit(-1);
			}
			numCameras++;
		}
		dc1394_free_camera_nodes(camera_nodes);
	}

	fflush(stdout);
	if (numCameras < 1) {
		perror("no cameras found :(\n");
		cleanup();
		exit(-1);
	}

	/* make the window */
	display=XOpenDisplay(getenv("DISPLAY"));
	if(display==NULL)
	{
		fprintf(stderr,"Could not open display \"%s\"\n",getenv("DISPLAY"));
		cleanup();
		exit(-1);
	}

	device_width = 640;
	device_height = 480;
	width=device_width;
	height=device_height;

	QueryXv();

	window=XCreateSimpleWindow(display,DefaultRootWindow(display),0,0,width,height,0,
		WhitePixel(display,DefaultScreen(display)),
		background);

	XSelectInput(display,window,StructureNotifyMask|KeyPressMask);
	XMapWindow(display,window);
	connection=ConnectionNumber(display);

	gc=XCreateGC(display,window,0,&xgcv);

	/* main event loop */
	while(1){

		dc1394_dma_multi_capture(cameras, numCameras);

		display_frames();
		XFlush(display);

		while(XPending(display)>0){
			XNextEvent(display,&xev);
			switch(xev.type){
				case ConfigureNotify:
					width=xev.xconfigure.width;
					height=xev.xconfigure.height;
					display_frames();
					break;
				case KeyPress:
					switch(XKeycodeToKeysym(display,xev.xkey.keycode,0)){
						case XK_q:
						case XK_Q:
							cleanup();
							exit(0);
							break;
						case XK_comma:
						case XK_less:
							width=width/2;
							height=height/2;
							XResizeWindow(display,window,width,height);
							display_frames();
							break;
						case XK_period:
						case XK_greater:
							width=2*width;
							height=2*height;
							XResizeWindow(display,window,width,height);
							display_frames();
							break;
						case XK_0:
							freeze = !freeze;
							break;
						}
					break;
				}
			} /* XPending */

			for (i = 0; i < numCameras; i++)
			{
				dc1394_dma_done_with_buffer(&cameras[i]);
			}

		} /* while not interrupted */

	exit(0);
}
