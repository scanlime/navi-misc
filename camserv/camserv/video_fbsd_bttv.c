/*  camserv - An internet streaming picture application
 *
 *  Copyright (C) 1999  Jon Travis (jon@dsndata.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ------------------------------
 *
 *  Props go out to Randall Hopper for his fxtv code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <machine/ioctl_bt848.h>
#include <machine/ioctl_meteor.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "video.h"
#include "log.h"
#include "grafxmisc.h"

extern int errno;

typedef struct fbttv_st {
  char section_name[ 1024 ]; /* Section name of the given device module */
  int initialized;           /* 1 if it has been initialzed, else 0     */
  int bttv_fd;               /* File descriptor of the bttv device      */
  int tune_fd;               /* File descriptor of the tuner device     */
  int width, height;         /* Current settings of the video device    */
  unsigned char *picbuf;     /* Picture buffer                          */

  int brightness;            /* Picture parameters                      */
  int chroma;                /* Chroma saturation                       */
  int contrast;              /* Contrast                                */

  int autobright;            /* Autobright value                        */
  int autoleft;              /* Frames until next auto adjustment       */
  int channelset;          /* channel set */
  int channel;                     /* channel setting */
} Fbttv;

#define MAX(x,y) ( (x) > (y) ? (x) : (y) )
#define MIN(x,y) ( (x) < (y) ? (x) : (y) )
#define array_elem(x) (sizeof(x) / sizeof( (x)[0] ))

#define MODNAME "fbsd_bttv"

#define FBTTVMOD_DEV_PATH       ( VIDCONFIG_PREFIX "device_path" )
#define FBTTVMOD_DEV_TUNERPATH  ( VIDCONFIG_PREFIX "tuner_path" )
#define FBTTVMOD_DEV_PORT       ( VIDCONFIG_PREFIX "port" )
#define FBTTVMOD_DEV_WIDTH      ( VIDCONFIG_PREFIX "width" )
#define FBTTVMOD_DEV_HEIGHT     ( VIDCONFIG_PREFIX "height" )
#define FBTTVMOD_DEV_AUTOBRIGHT ( VIDCONFIG_PREFIX "autobright" )
#define FBTTVMOD_DEV_CHROMA     ( VIDCONFIG_PREFIX "chroma" )
#define FBTTVMOD_DEV_CONTRAST   ( VIDCONFIG_PREFIX "contrast" )
#define FBTTVMOD_DEV_BRIGHTNESS ( VIDCONFIG_PREFIX "brightness" )
#define FBTTVMOD_DEV_CHANNELSET       ( VIDCONFIG_PREFIX "channelset" )
#define FBTTVMOD_DEV_CHANNEL  ( VIDCONFIG_PREFIX "channel" )

#define FBTTV_DEF_PATH         ("/dev/bktr0")
#define FBTTV_DEF_TUNERPATH    ("/dev/tuner0")
#define FBTTV_DEF_PORT         0
#define FBTTV_DEF_WIDTH        320
#define FBTTV_DEF_HEIGHT       240
#define FBTTV_DEF_AUTOBRIGHT   0
#define FBTTV_DEF_CHANNELSET   2 /* cableirc */
#define FBTTV_DEF_CHANNEL      3 /* local PBS */

static
const struct camparam_st {
  int min, max, range, drv_min, drv_range, def;
} CamParams[] = { 
  { 
    BT848_BRIGHTMIN, BT848_BRIGHTMIN + BT848_BRIGHTRANGE,
    BT848_BRIGHTRANGE, BT848_BRIGHTREGMIN, 
    BT848_BRIGHTREGMAX - BT848_BRIGHTREGMIN + 1, BT848_BRIGHTCENTER, },
  { 
    BT848_CONTRASTMIN, (BT848_CONTRASTMIN + BT848_CONTRASTRANGE),
    BT848_CONTRASTRANGE, BT848_CONTRASTREGMIN,
    (BT848_CONTRASTREGMAX - BT848_CONTRASTREGMIN + 1),
    BT848_CONTRASTCENTER, },
  {
    BT848_CHROMAMIN, (BT848_CHROMAMIN + BT848_CHROMARANGE), BT848_CHROMARANGE,
    BT848_CHROMAREGMIN, (BT848_CHROMAREGMAX - BT848_CHROMAREGMIN + 1 ),
    BT848_CHROMACENTER, },
};

/* Indices into the camera parameter array */
#define BRIGHT 0
#define CONTR  1
#define CHROMA 2

#define NTSC_MAX_X        640
#define NTSC_MAX_Y        480
#define PAL_MAX_X         768
#define PAL_MAX_Y         576

#define FBTTV_MAX_WIDTH    MAX(NTSC_MAX_X, PAL_MAX_X)
#define FBTTV_MAX_HEIGHT   MAX(NTSC_MAX_Y, PAL_MAX_Y)
#define FBTTV_MIN_WIDTH    2
#define FBTTV_MIN_HEIGHT   2

Fbttv *video_open( CamConfig *ccfg, char *secname );
void video_close( Fbttv *fbttv_dev );
int video_init( Fbttv *fbttv_dev, CamConfig *ccfg );
int video_deinit( Fbttv *fbttv_dev );
int video_snap( Fbttv *fbttv_dev, char *place_buffer, Video_Info *vinfo,
		CamConfig *ccfg );

/*
 * video_open:  Open the video device file descriptor, allocate
 *              a Fbttv structure, initialize it, and return it.
 *
 * Arguments:      ccfg = Camera configuration containing the device
 *                        path FBTTV_DEV_PATH
 *
 * Return valeus:  Returns NULL on failure, else a valid pointer to
 *                 a freshly malloced Fbttv structure on success.
 */

Fbttv *video_open( CamConfig *ccfg, char *secname ){
  const char *cfg_device_path, *cfg_tuner_path;
  Fbttv *res;
  int fd, tune_fd;

  cfg_device_path = camconfig_query_str( ccfg, secname, FBTTVMOD_DEV_PATH );
  if( cfg_device_path == NULL ){
    camserv_log( MODNAME, "[%s]:%s unset, defaulting to %s",
		 secname, FBTTVMOD_DEV_PATH, FBTTV_DEF_PATH );
    cfg_device_path = FBTTV_DEF_PATH;
  }

  cfg_tuner_path = camconfig_query_str(ccfg, secname, FBTTVMOD_DEV_TUNERPATH );
  if( cfg_tuner_path == NULL ){
    camserv_log( MODNAME, "[%s]:%s unset, defaulting to %s",
		 secname, FBTTVMOD_DEV_TUNERPATH, FBTTV_DEF_TUNERPATH );
    cfg_tuner_path = FBTTV_DEF_TUNERPATH;
  }

  if( (fd = open( cfg_device_path, O_RDONLY )) == -1 ){
    perror( "("MODNAME") video_open" );
    return NULL;
  }

  if( (tune_fd = open( cfg_tuner_path, O_RDONLY )) == -1 ){
    perror( "("MODNAME") video_open" );
    close( fd );
    return NULL;
  }

  if( (res = malloc( sizeof( *res ))) == NULL ){
    close( fd );
    close( tune_fd );
    return NULL;
  }

  res->picbuf = mmap( (caddr_t)0, 
		      FBTTV_MAX_WIDTH * FBTTV_MAX_HEIGHT * 3,
		      PROT_READ, MAP_SHARED, fd,
		      (off_t) 0 );
  if( res->picbuf == (unsigned char *) -1 ){
    camserv_log( MODNAME, "mmap: %s", strerror( errno ));
    close( tune_fd );
    close( fd );
    free( res );
    return NULL;
  }
   
  strncpy( res->section_name, secname, sizeof( res->section_name ) - 1 );
  res->section_name[ sizeof( res->section_name ) - 1 ] = '\0';

  res->bttv_fd     = fd;
  res->tune_fd     = tune_fd;
  res->width       = FBTTV_DEF_WIDTH;
  res->height      = FBTTV_DEF_HEIGHT;
  res->brightness  = CamParams[ BRIGHT ].def;
  res->contrast    = CamParams[ CONTR ].def;
  res->chroma      = CamParams[ CHROMA ].def;
  res->autobright  = FBTTV_DEF_AUTOBRIGHT;
  res->autoleft    = 0;
  res->initialized = 0;
  res->channelset  = 0;
  res->channel           = 0;
  return res;
}

/*
 * video_close:  Close the video device, and frees up the entire fbttv
 *               structure.  No further accesses to this object should be 
 *               made, after it is passed into here.
 *
 * Arguments:    fbbtv_dev = Fbttv device previously created by
 *                           calling video_open
 */

void video_close( Fbttv *fbttv_dev ){
  if( fbttv_dev->initialized ){
    /* Deinitialization cleanup XXX */
  }
  
  if( munmap( fbttv_dev->picbuf, FBTTV_MAX_WIDTH * FBTTV_MAX_HEIGHT * 3) == -1)
    camserv_log( MODNAME, "munmap: %s", strerror( errno ));

  close( fbttv_dev->bttv_fd );
  close( fbttv_dev->tune_fd );
  free( fbttv_dev );
}

/*
 * setup_pixelformat:  Setup the correct RGB pixelformat for the video
 *                     device -- We are looking for an RGB 8 format.
 *
 * Arguments:          fbttv_dev = Video device to set the PF for.
 *
 * Return values:      Returns -1 on failure, else 0
 */

static
int setup_pixelformat( Fbttv *fbttv_dev ){
  int i;
  struct meteor_pixfmt pf;

  for( i=0; ; i++ ){
    pf.index = i;
    if( ioctl( fbttv_dev->bttv_fd, METEORGSUPPIXFMT, &pf ) < 0 ){
      if( errno == EINVAL )
	break;
      camserv_log( MODNAME, "Error getting pixformat %d: %s", i,
		   strerror( errno ) );
      return -1;
    }
		
    if( pf.type == METEOR_PIXTYPE_RGB &&
	pf.Bpp == 3 )
      {
	/* Found a good pixeltype -- set it up */
	if( ioctl( fbttv_dev->bttv_fd, METEORSACTPIXFMT, &i ) < 0 ){
	  camserv_log( MODNAME, "Error setting pixformat: %s", 
		       strerror( errno ));
				/* Not immediately fatal */
	} else return 0;
      }
  }
  return -1;
}

/*
 * set_geometry:  Set the geometry of the bttv device based on the bttv
 *                camera configuration. 
 *
 * Arguments:     fbbtv_dev = Device to set the geometry of.
 *                ccfg      = Camera configuration of the device.
 * 
 * Return values: Returns -1 on failure, else 0
 */

static
int set_geometry( Fbttv *fbttv_dev, CamConfig *ccfg ){
  struct meteor_geomet geom;

  fbttv_dev->width = camconfig_query_def_int( ccfg, fbttv_dev->section_name,
					      FBTTVMOD_DEV_WIDTH,
					      FBTTV_DEF_WIDTH );
  fbttv_dev->height = camconfig_query_def_int( ccfg, fbttv_dev->section_name,
					       FBTTVMOD_DEV_HEIGHT,
					       FBTTV_DEF_HEIGHT );

  geom.columns = MAX( MIN( FBTTV_MAX_WIDTH, fbttv_dev->width ),
		      FBTTV_MIN_WIDTH );
  geom.rows = MAX( MIN( FBTTV_MAX_HEIGHT, fbttv_dev->height ),
		   FBTTV_MIN_HEIGHT );
  geom.oformat = METEOR_GEO_RGB24;
  geom.frames  = 1;

  if( ioctl( fbttv_dev->bttv_fd, METEORSETGEO, &geom ) < 0 ) {
    camserv_log( MODNAME, "Couldn't set the geometry: %s",
		 strerror( errno ) );
    return -1;
  }

  camserv_log( MODNAME, "Camera Geometry: %d x %d", geom.columns, geom.rows );
  return 0;
}


/*
 * set_input:  Attempt to set the input port of the bttv device.  The value
 *             in the camserv configuration will be used.  If this doesn't
 *             work, then a default 'base' input will be used.
 *
 * Arguments:  fbttv_dev = Video device to set the input of.
 *             ccfg      = Camera configuration to use to set the input.
 *
 * Return Value:  Returns -1 on failure, 0 on success.
 */

static
int set_input( Fbttv *fbttv_dev, CamConfig *ccfg ){
  int portnum, actport;
  int portdata[] = { METEOR_INPUT_DEV0, METEOR_INPUT_DEV1,
		     METEOR_INPUT_DEV2, METEOR_INPUT_DEV3,
		     METEOR_INPUT_DEV_SVIDEO  };

  portnum = camconfig_query_def_int( ccfg, fbttv_dev->section_name, 
			             FBTTVMOD_DEV_PORT, FBTTV_DEF_PORT );
  if( portnum >= array_elem( portdata ) ){
    camserv_log( MODNAME, "Port %d out of range (0-4)", portnum );
    portnum = FBTTV_DEF_PORT;
  }

  actport = portdata[ portnum ];
  if( ioctl( fbttv_dev->bttv_fd, METEORSINPUT, &actport ) < 0 ){
    if( portnum != FBTTV_DEF_PORT ) {
      camserv_log( MODNAME, "Port %d invalid -- Trying default of %d",
		   portnum, FBTTV_DEF_PORT );
      portnum = FBTTV_DEF_PORT;
      actport = portdata[ portnum ];
      if( ioctl( fbttv_dev->bttv_fd, METEORSINPUT, &actport ) < 0 ) {
	camserv_log( MODNAME, "Port %d init: %s", portnum, strerror( errno ));
	return -1;
      }
    } else {
      camserv_log( MODNAME, "Port %d init: %s", portnum, strerror( errno ));
      return -1;
    }
  }
  return 0;
}

/*
 * camparam_normalize:  Normalize a given camera parameter and obtain
 *                      the new (clamped) value as well as a value valid for
 *                      passing to the actual device.
 *
 * Arguments:           param     = One of CONTR, BRIGHT, CHROMA.
 *                      cfg_value = Value to normalize.
 *                      ioctl_val = Place to return an IOCTL value in.
 *
 * Return values:       Returns a new clamped configuration value.
 */

static
int camparam_normalize( int param, int cfg_value, int *ioctl_val ) {
  int val;

  cfg_value = MIN( CamParams[ param ].max,
		   MAX( CamParams[ param ].min, cfg_value ) );
  val = (cfg_value - CamParams[ param ].min ) / 
    (CamParams[ param ].range + 0.01) * CamParams[param].drv_range +
    CamParams[param].drv_min;
  val = MAX( CamParams[ param ].min, 
	     MIN( CamParams[ param ].drv_min + CamParams[ param ].drv_range-1,
		  val ));
  *ioctl_val = val;
  return cfg_value;
}

/*
 * set_chroma:      Set the chroma of the camera to a new chroma level.
 *                  The new (possibly modified) level will also be written
 *                  into the camconfig configuration.
 * 
 * Arguments:       fbttv_dev = Video device to set chroma of
 *                  ccfg      = Camera configuration within which to set
 *                              the chroma
 *                  new_chroma = New chroma level.
 *
 * Return values:   Returns -1 on error, 0 on success.
 */

static
int set_chroma( Fbttv *fbttv_dev, CamConfig *ccfg, int new_chroma ) {
  int ioctlval;
  
  new_chroma = camparam_normalize( CHROMA, new_chroma, &ioctlval );

  if( ioctl( fbttv_dev->tune_fd, BT848_SCSAT, &ioctlval ) < 0 ) {
    camserv_log( MODNAME, "Error setting CHROMA: %s", 
		 strerror( errno ));
    return -1;
  }
  
  fbttv_dev->chroma = new_chroma;
  camconfig_set_int( ccfg, fbttv_dev->section_name, FBTTVMOD_DEV_CHROMA, 
		     new_chroma );
  
  return 0;
}

/*
 * set_contrast:    Set the contrast of the camera to a new contrast level.
 *                  The new (possibly modified) level will also be written
 *                  into the camconfig configuration.
 * 
 * Arguments:       fbttv_dev = Video device to set contrast of
 *                  ccfg      = Camera configuration within which to set
 *                              the contrast
 *                  new_contrast = New contrast level.
 *
 * Return values:   Returns -1 on error, 0 on success.
 */

static
int set_contrast( Fbttv *fbttv_dev, CamConfig *ccfg, int new_contrast ) {
  int ioctlval;
  
  new_contrast = camparam_normalize( CONTR, new_contrast, &ioctlval );
  
  if( ioctl( fbttv_dev->tune_fd, BT848_SCONT, &ioctlval ) < 0 ) {
    camserv_log( MODNAME, "Error setting contrast: %s",
		 strerror( errno ));
    return -1;
  }
  
  fbttv_dev->contrast = new_contrast;
  camconfig_set_int( ccfg, fbttv_dev->section_name, FBTTVMOD_DEV_CONTRAST, 
		     new_contrast );
  return 0;
}

/*
 * set_brightness:  Set the brightness of the camera to a new brightness level.
 *                  The new (possibly modified) level will also be written
 *                  into the camconfig configuration.
 * 
 * Arguments:       fbttv_dev = Video device to set brightness of
 *                  ccfg      = Camera configuration within which to set
 *                              the brightness
 *                  new_bright = New brightness level.
 *
 * Return values:   Returns -1 on error, 0 on success.
 */

static
int set_brightness( Fbttv *fbttv_dev, CamConfig *ccfg, int new_bright ) {
  int ioctlval;

  new_bright = camparam_normalize( BRIGHT, new_bright, &ioctlval );
  if( ioctl( fbttv_dev->tune_fd, BT848_SBRIG, &ioctlval ) < 0 ) {
    camserv_log( MODNAME, "Error brightness->%d : %s", ioctlval,
		 strerror( errno ) );
    return -1;
  }

  fbttv_dev->brightness = new_bright;
  camconfig_set_int( ccfg, fbttv_dev->section_name, FBTTVMOD_DEV_BRIGHTNESS, 
		     new_bright );
  return 0;
}

static
int set_channel( Fbttv *fbttv_dev, CamConfig *ccfg, int new_channel ) {
  int ioctlval;

  ioctlval = new_channel;
  if( ioctl( fbttv_dev->tune_fd, TVTUNER_SETCHNL, &ioctlval ) < 0 ) {
    camserv_log( MODNAME, "Error channel->%d : %s", ioctlval,
               strerror( errno ) );
    return -1;
  } else {
      camserv_log( MODNAME, "channel set to %d", ioctlval);
  }

  fbttv_dev->channel = new_channel;
  camconfig_set_int( ccfg, fbttv_dev->section_name, FBTTVMOD_DEV_CHANNEL, 
                   new_channel );

  return 0;
}

static
int set_channelset (Fbttv * fbttv_dev, CamConfig *ccfg, int new_channelset) {
  int ioctlval;

  ioctlval = new_channelset;
  if( ioctl( fbttv_dev->tune_fd, TVTUNER_SETTYPE, &ioctlval ) < 0 ) {
    camserv_log( MODNAME, "Error channelset->%d : %s", ioctlval, 
                 strerror( errno ) );
    return -1;
  } else {         
        camserv_log( MODNAME, "channelset set to %d", ioctlval);
  }   

  fbttv_dev->channelset = new_channelset;
  camconfig_set_int( ccfg, fbttv_dev->section_name, FBTTVMOD_DEV_CHANNELSET, 
                   new_channelset );
  return 0;
}

/*
 * video_init:   Initialize the video camera.  This routine
 *               will query the video device for the 
 *               capabilities, and select the optimal properties
 *               for the given parameters.  
 *
 * Arguments:    fbbtv_dev = valid Fbttv object. 
 *               ccfg      = Parameters to initialize the device with
 *
 * Return Value: Returns -1 on failure, else 0
 */

int video_init( Fbttv *fbttv_dev, CamConfig *ccfg ){
  int capmethod = METEOR_CAP_CONTINOUS;

  if( fbttv_dev->initialized ) 
    camserv_log( MODNAME, "Double initialization detected!" );

  fbttv_dev->initialized = 0;

  if( set_input( fbttv_dev, ccfg ) == -1 ||
      setup_pixelformat( fbttv_dev ) == -1  ||
      set_geometry( fbttv_dev, ccfg ) == -1 )
    {
      camserv_log( MODNAME, "Error initializing video" );
      return -1;
    }

  set_brightness( fbttv_dev, ccfg, 
		  camconfig_query_def_int( ccfg, fbttv_dev->section_name,
					   FBTTVMOD_DEV_BRIGHTNESS,
					   CamParams[ BRIGHT ].def) );

  set_chroma( fbttv_dev, ccfg, 
	      camconfig_query_def_int( ccfg, fbttv_dev->section_name,
				       FBTTVMOD_DEV_CHROMA,
				       CamParams[ CHROMA ].def ) );

  set_contrast( fbttv_dev, ccfg, 
		camconfig_query_def_int( ccfg, fbttv_dev->section_name,
					 FBTTVMOD_DEV_CONTRAST,
					 CamParams[ CONTR ].def ) ); 

  set_channelset( fbttv_dev, ccfg,
		  camconfig_query_def_int( ccfg, fbttv_dev->section_name,
					   FBTTVMOD_DEV_CHANNELSET,
					   FBTTV_DEF_CHANNELSET));
  set_channel( fbttv_dev, ccfg,
               camconfig_query_def_int( ccfg, fbttv_dev->section_name,
                                        FBTTVMOD_DEV_CHANNEL,
					FBTTV_DEF_CHANNEL));

  fbttv_dev->autobright = camconfig_query_def_int( ccfg, 
						   fbttv_dev->section_name,
						   FBTTVMOD_DEV_AUTOBRIGHT,
						   FBTTV_DEF_AUTOBRIGHT );


  if( ioctl( fbttv_dev->bttv_fd, METEORCAPTUR, &capmethod ) < 0 ){
    camserv_log( MODNAME, "CaptureMode: %s", strerror( errno ));
    return -1;
  }

  /* Setup the parameters so the other filters can use the information */
  camconfig_set_int( ccfg, SEC_VIDEO, VIDCONFIG_WIDTH, fbttv_dev->width );
  camconfig_set_int( ccfg, SEC_VIDEO, VIDCONFIG_HEIGHT, fbttv_dev->height );
  camconfig_set_int( ccfg, SEC_VIDEO, VIDCONFIG_MAXWIDTH, FBTTV_MAX_WIDTH );
  camconfig_set_int( ccfg, SEC_VIDEO, VIDCONFIG_MINWIDTH, FBTTV_MIN_WIDTH );
  camconfig_set_int( ccfg, SEC_VIDEO, VIDCONFIG_MAXHEIGHT, FBTTV_MAX_HEIGHT );
  camconfig_set_int( ccfg, SEC_VIDEO, VIDCONFIG_MINHEIGHT, FBTTV_MIN_HEIGHT );
  camconfig_set_int( ccfg, SEC_VIDEO, VIDCONFIG_ISB_N_W, 0 );

  fbttv_dev->initialized = 1;
  return 0;
}

/*
 * video_deinit:  Deinitialize a fbbtv device.  This clears up captured
 *                frames and the buffers used for storage .. This should
 *                be done everytime the camera is to be re-configured,
 *                (for every init call)
 *
 * Arguments:     fbbtv_dev = Device to deinitialize
 *
 * Return values:  Returns -1 on error, 0 on success.
 */

int video_deinit( Fbttv *fbttv_dev ){
  int stopcap;

  if( fbttv_dev->initialized == 0 ){
    camserv_log( MODNAME, "Deinitialized without initializing device\n");
    return -1;
  }

  stopcap = METEOR_CAP_STOP_CONT;
  if( ioctl( fbttv_dev->bttv_fd, METEORCAPTUR, &stopcap ) < 0 ) 
    camserv_log( MODNAME, "StopCapture: %s", strerror( errno ));

  fbttv_dev->initialized = 0;
  return 0;
}

static
int adjust_contrast( int width, int height, const char *picbuf, int mean,
		     Fbttv *fbttv_dev, CamConfig *ccfg ) 
{
  int dev, adjust = 0, newcontr;

  dev = camserv_get_pic_stddev( width, height, picbuf, 1, mean );

  if( dev < (256 / 6) - 3 || dev > (256 / 6) + 3 ) {
    newcontr = fbttv_dev->contrast;
    adjust = 1;
    if( dev > (256 / 6 ) + 3 ) {
      newcontr--;
    } else {
      newcontr++;
    }
  }

  if( adjust ) {
    set_contrast( fbttv_dev, ccfg, newcontr );
    return 1;
  } else {
    return 0;
  }
}

static
int adjust_bright( int width, int height, const char *picbuf, 
	           Fbttv *fbttv_dev, CamConfig *ccfg )
{
  int totmean, newbright, adjust = 0, adjustcont = 0;

  if( !fbttv_dev->autobright || --fbttv_dev->autoleft > 0 )
    return 0;

  totmean = calculate_pic_mean( width, height, picbuf, 1, 0, 0, 
				width, height );
  if( totmean < (256 / 2) - 10 || totmean > (256 / 2) + 10 ) {
    newbright = fbttv_dev->brightness;
    if( totmean > (256 / 2) + 10 ){
      newbright-=1;
    } else { 
      newbright+=1;
    }

    adjust = 1;
  }

  adjustcont = adjust_contrast( width, height, picbuf, totmean, 
				fbttv_dev, ccfg );

  if( adjust ) {
    set_brightness( fbttv_dev, ccfg, newbright );
    return 1;
  } 

  if( adjustcont )
    return 1;
    
  fbttv_dev->autoleft = fbttv_dev->autobright;
  return 0;
}
		   


static void
bgr2rgb (char *out_addr, char *in_addr, int rowstride, int width, int height)
{
  int i, j;

  for (i=0; i<height; i++){
    char *q = out_addr + i * rowstride;
    char *p = in_addr + i * rowstride;
    
    for (j=0; j<width; j++)
      {
	q[2] = p[0];
	q[1] = p[1];
	q[0] = p[2];
	
	q += 3;
	p += 3;
      }
  }

}

/*
 * video_snap:  Take a snapshot from the video device, and put it into
 *              place_buffer.  The format is a RGB format, and place_buffer
 *              is expected to contain enough space to store the 
 *              width * height * 3 bytes
 *
 * Arguments:   fbbtv_dev = Video device to snap the picture of
 *              place_buffer = Storage location to put the picture
 *              vinfo = Storage location for information about the picture
 *                      snapped.
 *
 * Return values:  Returns -1 on error, 0 on success.
 */

int video_snap( Fbttv *fbttv_dev, char *place_buffer, Video_Info *vinfo,
		CamConfig *ccfg )
{
  bgr2rgb( place_buffer, fbttv_dev->picbuf,
	   fbttv_dev->width * 3, fbttv_dev->width, 
	   fbttv_dev->height );
  vinfo->width = fbttv_dev->width;
  vinfo->height = fbttv_dev->height;
  vinfo->is_black_white = 0;
  vinfo->nbytes = vinfo->width * vinfo->height * 3;
  adjust_bright( vinfo->width, vinfo->height, place_buffer,
		 fbttv_dev, ccfg );
 
  return 0;
}

/*
 * video_get_geom:  Get geometry information about the video device. 
 *                  The video device must be opened before the geometry
 *                  can be gotten.  
 *
 * Arguments:       fbbtv_dev  = Video device as from video_open
 *                  geom       = Location to place geometry information
 *
 * Return values:   Returns an ORed combination of VIDEO_GEOM_*, representing
 *                  which information in the returned structure is valid.
 *                  0 is returned on function failure.
 */

int video_get_geom( Fbttv *fbttv_dev, Video_Geometry *geom ){
  geom->max_width  = FBTTV_MAX_WIDTH;
  geom->max_height = FBTTV_MAX_HEIGHT;
  geom->min_width  = FBTTV_MIN_WIDTH;
  geom->min_height = FBTTV_MIN_HEIGHT;

  if( fbttv_dev->initialized == 1 ) {
    geom->cur_width = fbttv_dev->width;
    geom->cur_height = fbttv_dev->height;
    return VIDEO_GEOM_MAX | VIDEO_GEOM_MIN | VIDEO_GEOM_CUR;
  }

  return VIDEO_GEOM_MAX | VIDEO_GEOM_MIN;
}
  
 
/*
 * modinfo_query:  Routine to return information about the variables
 *                 accessed by this particular module.
 *
 * Return values:  Returns a malloced ModInfo structure, for which
 *                 the caller must free, or NULL on failure.
 */

ModInfo *modinfo_query(){
  ModInfo *res;
  char varname[ 1024 ];

  if( (res = modinfo_create( 0 )) == NULL )
    return NULL;

  return res;
}

