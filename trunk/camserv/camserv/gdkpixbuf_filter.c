#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "camserv.h"
#include "alpha_map.h"
#include "camconfig.h"
#include "video.h"
#include "filter.h"
#include "mainloop.h"
#include "log.h"

#define MODNAME "gdkpixbuf_filter"

typedef struct rand_filter_st {
  GdkPixbuf *pixbuf;
  unsigned char *data;
  int x, y, width, height, has_alpha;
} Gdk_Filter;

/*
 * filter_init:        Standard filter initialization routine.
 *
 */

void *filter_init( CamConfig *ccfg, char *section_name ){
  Gdk_Filter *res;
  const char *image_name;
  char buf[ 1024 ];
  int isbw, err;

  isbw = camconfig_query_int( ccfg, SEC_VIDEO, VIDCONFIG_ISB_N_W, &err );
  if( err ) camserv_log( MODNAME, "Config inconsistancy!  (isb_n_w)" );
  if( isbw == 1 ) {
    camserv_log( MODNAME, "This filter does not support B&W!" );
    return NULL;
  }

  if( (res = malloc( sizeof( *res ))) == NULL ){
    camserv_log( MODNAME, "FATAL! Couldn't allocate %d bytes", sizeof( *res ));
    return NULL;
  }
  
  if( (image_name = camconfig_query_str( ccfg, section_name, "file" ))== NULL){
    camserv_log( MODNAME, "FATAL!  [%s]:file not specified!", section_name  );
    free( res );
    return NULL;
  }

  strncpy( buf, image_name, sizeof( buf ) -1 );
  buf[ sizeof( buf ) - 1 ] = '\0';

  if( !(res->pixbuf = gdk_pixbuf_new_from_file( buf ))){
    camserv_log( MODNAME, "FATAL!  Load error loading \"%s\"", buf );
    free( res );
    return NULL;
  }

  if( gdk_pixbuf_get_colorspace(res->pixbuf) != GDK_COLORSPACE_RGB ||
      gdb_pixbuf_get_n_channels(res->pixbuf) < 3){
    camserv_log( MODNAME, "FATAL!  Image loaded in an invalid format!" );
    gdk_pixbuf_unref( res->pixbuf );
    free( res );
    return NULL;
  }

  res->x         = camconfig_query_def_int( ccfg, section_name, "x", 0 );
  res->y         = camconfig_query_def_int( ccfg, section_name, "y", 0 );
  res->width     = gdk_pixbuf_get_width(res->pixbuf);
  res->height    = gdk_pixbuf_get_height(res->pixbuf);
  res->has_alpha = gdk_pixbuf_get_has_alpha(res->pixbuf);
  res->data      = gdk_pixbuf_get_pixels(res->pixbuf);
  return res;
}

/*
 * filter_deinit:  Standard filter deinit routine 
 */

void filter_deinit( void *filter_dat ){
  Gdk_Filter *gfilter = filter_dat;

  if( !gfilter ) return;
  gdk_pixbuf_unref( gfilter->pixbuf );
  free( gfilter );
}

static
int image_outside_pic( int img_x, int img_y, int img_width, int img_height,
		       const Video_Info *vinfo )
{
  if( img_x >= vinfo->width || img_y >= vinfo->height )
    return 1;
  
  if( img_x + img_width < 0 || img_y + img_height < 0 ) 
    return 1;

  return 0;
}

void filter_func( char *in_data, char **out_data, void *cldat, 
		  const Video_Info *vinfo_in, Video_Info *vinfo_out )
{
  Gdk_Filter *gfilter = cldat;
  unsigned char *src_cp, *dest_cp, alphaval, alphainv;
  int width, height, dest_addy, src_addy, x, y;
  int min_x, min_y, max_x, max_y;

  /* In-place mangling */
  *vinfo_out = *vinfo_in;
  *out_data = in_data;
  
  width  = gfilter->width;
  height = gfilter->height;

  if( image_outside_pic( gfilter->x, gfilter->y, width, height, vinfo_in ))
    return;

  min_x = MAX( 0, gfilter->x );
  min_y = MAX( 0, gfilter->y );
  max_x = MIN( vinfo_in->width,  gfilter->x + width );
  max_y = MIN( vinfo_in->height, gfilter->y + height );

  /* Start out the source at where we are going to place the pic */
  src_cp = (unsigned char *)gfilter->data;
  src_cp += (((min_y - gfilter->y) * width) + (min_x - gfilter->x)) * 4;
  /* Figure out how much we are going to add to the source pointer to get it
     from the end of 1 scanline to the beginning of the next */
  src_addy = (min_x - gfilter->x) + /* Beginning clipping */
             (gfilter->x + width - max_x );
  src_addy *= (gfilter->has_alpha ? 4 : 3);
  
  dest_cp = in_data;
  dest_cp += ((min_y * vinfo_in->width) + min_x) * 3;
  /* Figure out the destination addy, which will be at MOST vinfo->width,
     and at LEAST 0 */
  dest_addy = (vinfo_in->width - max_x + min_x) * 3;

  if( gfilter->has_alpha ) {
    for( y=min_y; y< max_y; y++ ){
      for( x= min_x; x< max_x; x++ ){
	alphaval   = *(src_cp + 3);
	alphainv = 255 - alphaval;
	*(dest_cp + 0) = alpha_map[ alphaval ][*(src_cp + 0)] +
	  alpha_map[ alphainv ][*(dest_cp + 0)];
	*(dest_cp + 1) = alpha_map[ alphaval ][*(src_cp + 1)] +
	  alpha_map[ alphainv ][*(dest_cp + 1)];
	*(dest_cp + 2) = alpha_map[ alphaval ][*(src_cp + 2)] +
	  alpha_map[ alphainv ][*(dest_cp + 2)];
	
	dest_cp += 3;
	src_cp += 4;
      }
      dest_cp += dest_addy;
      src_cp  += src_addy;
    }
  } else {
    for( y=min_y; y< max_y; y++ ){
      for( x= min_x; x< max_x; x++ ){
	*(dest_cp + 0) = *(src_cp + 0);
	*(dest_cp + 1) = *(src_cp + 1);
	*(dest_cp + 2) = *(src_cp + 2);
	dest_cp += 3;
	src_cp += 3;
      }
      dest_cp += dest_addy;
      src_cp  += src_addy;
    }
  }
}

void filter_validation(){
  Filter_Init_Func init = filter_init;
  Filter_Deinit_Func deinit = filter_deinit;
  Filter_Func_Func func = filter_func;

  if( init != NULL && deinit != NULL && func != NULL ) return;
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

  if( (res = modinfo_create( 3 )) == NULL )
    return NULL;

  modinfo_varname_set( res, 0, "file" );
  modinfo_desc_set( res, 0, "File location of the image to import" );
  res->vars[ 0 ].type = MODINFO_TYPE_STR;

  modinfo_varname_set( res, 1, "x" );
  modinfo_desc_set( res, 1, "X location to place the picture" );
  res->vars[ 1 ].type = MODINFO_TYPE_INT;

  modinfo_varname_set( res, 2, "x" );
  modinfo_desc_set( res, 2, "Y location to place the picture" );
  res->vars[ 2 ].type = MODINFO_TYPE_INT;

  return res;
}
