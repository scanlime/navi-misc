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
 */

#include "camserv_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "databuf.h"
#include "socket.h"
#include "sockset.h"
#include "manager.h"
#include "sock_field.h"
#include "log.h"

#define MODNAME "relay"

#define BACKLOG  20

extern int errno;

#define CLIENT_T_UNK              0
#define CLIENT_T_CAMSERV          1
#define CLIENT_T_BROWSER          2
#define CLIENT_T_PROXY            3
#define CLIENT_T_SINGLE           4

#define CAMSTATE_SEND_PROXY       1  /* Sending proxy notifier            */
#define CAMSTATE_RECV_SIZE        2  /* Receiving the size of the picture */
#define CAMSTATE_RECV_PICTURE     3  /* Receiving the actual picture      */

#define BROWSERSTATE_SEND_PREAMBLE  0 /* Sending the preamble  */
#define BROWSERSTATE_SEND_PICTURE   1 /* Sending the picture   */
#define BROWSERSTATE_SEND_SEPERATOR 2 /* Sending the seperator */

#define PROXYCLSTATE_SEND_PICSIZE   0 /* Sending the pic size */
#define PROXYCLSTATE_SEND_PICTURE   1 /* Sending the actual picture */

typedef struct client_data_st {
  int clienttype;               /* one of CLIENT_T_* */
  DataBuf *writebuf;
  DataBuf *readbuf;         

  struct camdata_st {           /* Valid only if clientttype == CAMSERV  */
    unsigned long int picsize;  /* Picture size in network order */
    char *picbuf;               /* Actual picture data           */
    int camstate;               /* State of camera               */
  } camdata;

  struct browser_st {          
    int browserstate;
    void *management_data;
    int last_pic_id;
  } browserdata;

  struct proxycl_st {
    int proxystate;
    unsigned long int picsize;  /* Picture size in network order */
    void *management_data;
    int last_pic_id;
    char *pic_data;
  } proxycldata;

  char junkbuf[ 1024 ];
} ClientData;

typedef struct relay_data_st {
  char camera_ip[ 1024 ];   /* IP of the remote camserv or relay   */
  int camera_port;          /* Port of the remote camserv or relay */
  Socket *camera_sock;      /* Connection of camera sock           */
} RelayData;

#define RANDOMSTRING "ThisRandomString"
#define CONTENTTYPE  "image/jpeg"

static
char *get_preamble_text( size_t *len, int multi ){
#define MPREAMBLE_STR "HTTP/1.0 200 OK\n"  \
  "Content-type: multipart/x-mixed-replace;boundary=" RANDOMSTRING "\n" \
  "Cache-Control: no-cache\n" \
  "Cache-Control: private\n" \
  "Pragma: no-cache\n\n" \
  "--" RANDOMSTRING "\n" \
  "Content-type: " CONTENTTYPE "\n\n"
#define SPREAMBLE_STR "HTTP/1.0 200 OK\n"  \
  "Content-type: " CONTENTTYPE "\n" \
  "Cache-Control: no-cache\n" \
  "Cache-Control: private\n" \
  "Pragma: no-cache\n\n" 
  
  if( multi ) {
    if( len != NULL ) *len = sizeof( MPREAMBLE_STR ) - 1;
    return MPREAMBLE_STR;
  } else {
    if( len != NULL ) *len = sizeof( SPREAMBLE_STR ) - 1;
    return SPREAMBLE_STR;
  }
}

static
char *get_seperator_text( size_t *len ){
#define SEPERATOR_TEXT "\n--" RANDOMSTRING "\n"  \
  "Content-type: " CONTENTTYPE "\n\n" /* XXX */

  if( len != NULL ) *len = sizeof( SEPERATOR_TEXT ) - 1;
  return SEPERATOR_TEXT;
}


/*
 * client_data_new:  Create and initialize a new clientinfo structure.
 *                 
 * Return values:    Returns NULL on failure, else a valid new clientdata
 *                   on success.
 */

static
ClientData *client_data_new(){
  ClientData *res;

  if( (res = malloc( sizeof( *res ))) == NULL )
    return NULL;

  if( (res->readbuf = databuf_new()) == NULL ){
    free( res );
    return NULL;
  }

  if( (res->writebuf = databuf_new()) == NULL ){
    databuf_dest( res->readbuf );
    free( res );
    return NULL;
  }

  res->clienttype = CLIENT_T_UNK;
  return res;
}

/*
 * client_data_dest:  Destroy a client data structure.
 *
 * Arguments:         cldata = Clientdata to destroy
 */

static
void client_data_dest( ClientData *cldata ){
  databuf_dest( cldata->readbuf );
  databuf_dest( cldata->writebuf );
  free( cldata );
}

/*
 * relay_connect_camserv:  Connect to a camera server.  
 *
 * Arguments:              rdata = Relay data with info of where to connect.
 *
 * Return values:          Returns -1 if the connection could not be made, else
 *                         0.  If successful, rdata->camera_sock will be
 *                         set to the new sock, else it will be set to NULL
 */

static
int relay_connect_camserv( SockField_Data *sfdata, RelayData *rdata ){
  ClientData *cldata;

  rdata->camera_sock = socket_connect( rdata->camera_ip, rdata->camera_port );
  if( rdata->camera_sock == NULL )
    return -1;

  if( (cldata = client_data_new()) == NULL ){
    camserv_log( MODNAME, "Error mallocing clientdata");
    socket_dest( rdata->camera_sock );
    return -1;
  }

  cldata->clienttype = CLIENT_T_CAMSERV;
  
  if( sock_field_manage_socket( sfdata, rdata->camera_sock, 
				cldata ) == -1 )
  {
      camserv_log( MODNAME, "Error managing connect camera socket!");
      client_data_dest( cldata );
      socket_dest( rdata->camera_sock );
      rdata->camera_sock = NULL;
      return -1;
  }

  /* Send the proxy keyword */
  databuf_buf_set( cldata->writebuf, "PROXY", sizeof( "PROXY" ));
  cldata->camdata.picsize  = 0;
  cldata->camdata.picbuf   = NULL;
  cldata->camdata.camstate = CAMSTATE_SEND_PROXY;
  
  return 0;
}

/*
 * relay_init:  Initialize the relay.  This involves making the connection
 *              to the camera server if possible.  
 *
 * Arguments:   sfdata = Sockfield data passed from the almighty.
 *
 * Return values:  Returns SOCKFIELD_OK on success, else SOCKFIELD_CLOSE.
 */

static
int relay_init( SockField_Data *sfdata, void *sys_cldata ){
  RelayData *rdata = sys_cldata;
  
  if( relay_connect_camserv( sfdata, rdata ) == -1 )
    camserv_log( MODNAME, "Couldn't connect to camserv: %s %d", 
	     rdata->camera_ip, rdata->camera_port );

  return SOCKFIELD_OK;
}

/*
 * relay_preclose:  Called by the field loop prior to closing a socket.
 *                  This routine should cleanup any of the clientdata passed
 *                  to the fieldloop management routines on socket init.
 *
 * Arguments:       sock   = Socket about to be closed.
 *                  cldata = Clientdata passed in for socket @ manage time.
 *                  sys_cldata = System clientdata, passed in as argument
 *                               to main field_loop
 */

static
void relay_preclose( Socket *sock, void *cldata, void *sys_cldata ){
  RelayData *rdata = sys_cldata;
  ClientData *clientdata = cldata;

  if( clientdata->clienttype == CLIENT_T_CAMSERV ) {
    camserv_log( MODNAME, "Closing connection to camsera server!" );
    /* Camera sock is now invalid */
    rdata->camera_sock = NULL;
  } else {
    camserv_log( MODNAME, "Closing client from: \"%s\"", 
		 socket_query_remote_name( sock ));
  }
  client_data_dest( cldata );
}

/*
 * read_camera:  This routine is called everytime the camserv socket has
 *               sent us some data to read.  It is in 2 states the entire
 *               time -- that of getting the size of the next picture, then
 *               actually receiving the picture.  After a successful
 *               receipt of a picture, it is then put into the management bins.
 * 
 * Arguments:    sfdata = SockField data
 *               sock   = Socket of the camserv object
 *               cldata = Clientdata of the camserv object.
 *
 * Return values:  Returns one of SOCKFIELD_*
 */

static
int read_camera( SockField_Data *sfdata, Socket *sock, ClientData *cldata ){
  if( cldata->camdata.camstate == CAMSTATE_RECV_SIZE ) {
    /* We just got the size ... malloc the data and attempt to receive it too*/
    cldata->camdata.camstate = CAMSTATE_RECV_PICTURE;
    cldata->camdata.picbuf  = malloc( ntohl( cldata->camdata.picsize ));
    if( cldata->camdata.picbuf == NULL ) {
      camserv_log( MODNAME, "Couldn't malloc: %ld bytes for picture!",
		   (long)ntohl( cldata->camdata.picsize ));
      return SOCKFIELD_CLOSE;
    }
    databuf_buf_set( cldata->readbuf, cldata->camdata.picbuf,
		     ntohl( cldata->camdata.picsize ) );
  } else {
    /* Else we just completed getting a whole picture */
    cldata->camdata.camstate = CAMSTATE_RECV_SIZE;
    databuf_buf_set( cldata->readbuf, &cldata->camdata.picsize,
		     sizeof( cldata->camdata.picsize ) );
    if( manager_new_picture( cldata->camdata.picbuf, 
			     ntohl( cldata->camdata.picsize ), 100 ) == -1 )
    {
      camserv_log( MODNAME, "Unable to manage picture!");
      free( cldata->camdata.picbuf );
      return SOCKFIELD_CLOSE;
    }
    sock_field_unhold_write( sfdata );
  }
  return SOCKFIELD_OK;
}

/*
 * read_client:  Called when a client has sent data to the relay.  We just
 *               ignore everything the client sends us after initialization.
 */

static
int read_client( SockField_Data *sfdata, Socket *sock, ClientData *cldata ){
  /* Just refill the buffer .. client's that write anything, we are really
     just disregarded */
  databuf_buf_set( cldata->readbuf, &cldata->junkbuf[ 0 ],
		   sizeof( cldata->junkbuf ));
  return SOCKFIELD_OK;
}

/*
 * set_client_writebuf:  Set the writebuffer of the client for the first
 *                       time.  This is usually the 'init' state for the
 *                       client.
 *
 * Arguments:            cldata = client to set the writebuffer for.
 */

static
void set_client_writebuf( ClientData *cldata  ){
  size_t len;
  char *cp;

  if( cldata->clienttype == CLIENT_T_BROWSER ){
    cp = get_preamble_text( &len, 1 );
    cldata->browserdata.browserstate = BROWSERSTATE_SEND_PREAMBLE;
  } else if( cldata->clienttype == CLIENT_T_SINGLE ) {
    cp = get_preamble_text( &len, 0 );
    cldata->browserdata.browserstate = BROWSERSTATE_SEND_PREAMBLE;
  } else if( cldata->clienttype == CLIENT_T_PROXY ) {
    cp = "";
    len = 0;
    cldata->proxycldata.proxystate = PROXYCLSTATE_SEND_PICTURE;
    cldata->proxycldata.management_data = NULL;
  } else 
    camserv_log( MODNAME, "Unknown client state: \"%d\"", cldata->clienttype );

  databuf_buf_set( cldata->writebuf, cp, len );
}

/*
 * relay_client_read:  Called by the sockfield loop when a socket has become
 *                     readable -- On a client of which we have not identified
 *                     prior, we determine their type by their initial msg.
 *                     Otherwise on known sockets we attempt to read all the
 *                     data we are currently trying to get from them, then
 *                     dispatch them to the correct handling procedures 
 *                     (camserv, vs. client).
 *                     
 * Arguments:          Standard sockfield callback arguments
 *
 * Return Values:      Returns one of SOCKFIELD_*
 */

static
int relay_client_read( SockField_Data *sfdata, Socket *sock, void *cldata ){
  ClientData *clientdata = cldata;
  int readres;
  char sparebuf[ 1024 ];

  if( clientdata->clienttype == CLIENT_T_UNK ){
    read( socket_query_fd( sock ), sparebuf, sizeof( sparebuf ) );
    sparebuf[ sizeof( sparebuf ) - 1 ] = '\0';
    /* Client initializing itself */
    if( !strncmp( sparebuf, "GET", 3 ) ){
      if( strstr( sparebuf, "/singleframe" ))
	clientdata->clienttype = CLIENT_T_SINGLE;
      else
	clientdata->clienttype = CLIENT_T_BROWSER;
    } else if( !strncmp( sparebuf, "PROXY", 5 )) {
      clientdata->clienttype = CLIENT_T_PROXY;
    } else {
      clientdata->clienttype = CLIENT_T_BROWSER;
    }
    databuf_buf_set( clientdata->readbuf, &clientdata->junkbuf[ 0 ],
		     sizeof( clientdata->junkbuf ));

    set_client_writebuf( clientdata );
    sock_field_unhold_write( sfdata );
    return SOCKFIELD_OK;
  }

  readres = databuf_read( clientdata->readbuf, socket_query_fd( sock ));
  if( readres == -1 ) return SOCKFIELD_CLOSE;
  if( readres ==  1 ) return SOCKFIELD_OK;

  if( clientdata->clienttype == CLIENT_T_CAMSERV )
    return read_camera( sfdata, sock, clientdata );
  else
    return read_client( sfdata, sock, clientdata );
      
  return SOCKFIELD_OK;
}

/*
 * write_camera:  Called when our camserv socket is set to writeable.  For
 *                most of the time we will just put ourselves on hold, however
 *                the initial connection with the camserv requires us to
 *                send the PROXY message.
 *
 * Return values:  Returns one of SOCKFIELD_*
 */

static
int write_camera( SockField_Data *sfdata, Socket *sock, ClientData *cldata ){
  if( cldata->camdata.camstate != CAMSTATE_SEND_PROXY ){
    sock_field_hold_write( sfdata, sock );
    return SOCKFIELD_OK;
  }

  /* Here we have just initialized our proxy with the connected camera.  So
     we put ourselves in a 'read' mode for the rest of the period */
  cldata->camdata.camstate = CAMSTATE_RECV_SIZE;
  databuf_buf_set( cldata->readbuf, &cldata->camdata.picsize,
		   sizeof( cldata->camdata.picsize ) );
  return SOCKFIELD_OK;
}

/*
 * write_client_proxy:  Called when a client (who has previously identified
 *                      itself as a proxy) has become writeable.  Depending
 *                      on the current state of the proxy, a picture will
 *                      either be snagged from the management bins, or 
 *                      the picture will be sent.
 *
 * Return values:       Returns one of SOCKFIELD_*
 */

static
int write_client_proxy( SockField_Data *sfdata, Socket *sock, 
			ClientData *cldata )
{
  if( cldata->proxycldata.proxystate == PROXYCLSTATE_SEND_PICTURE )
  {
    char *pic_data;
    size_t pic_size;
    int pic_id;

    /* Just finished sending a picture */
    if( cldata->proxycldata.management_data &&
	manager_dest_client( cldata->proxycldata.management_data ) == -1 )
      camserv_log( MODNAME, "Error destroying client proxy management!");

    /* Get a new picture to send */
    cldata->proxycldata.management_data = 
      manager_new_client( &pic_data, &pic_size, &pic_id );

    if( cldata->proxycldata.management_data == NULL ){
      camserv_log( MODNAME, "Error managing client proxy!  "
		   "(no pictures may be available yet)" );
      sock_field_hold_write( sfdata, sock );
      return SOCKFIELD_OK;
    }

    if( pic_id == cldata->proxycldata.last_pic_id ) { /* Proxy read too fast */
      manager_dest_client( cldata->proxycldata.management_data );
      cldata->proxycldata.management_data = NULL;
      sock_field_hold_write( sfdata, sock );
      return SOCKFIELD_OK;
    }

    cldata->proxycldata.proxystate = PROXYCLSTATE_SEND_PICSIZE;
    cldata->proxycldata.picsize = htonl( pic_size );
    cldata->proxycldata.pic_data = pic_data;
    cldata->proxycldata.last_pic_id = pic_id;
    databuf_buf_set( cldata->writebuf, 
		     &cldata->proxycldata.picsize, 
		     sizeof( cldata->proxycldata.picsize ));
  } else {
    /* Else just finished sending the picture size */
    cldata->proxycldata.proxystate = PROXYCLSTATE_SEND_PICTURE;
    databuf_buf_set( cldata->writebuf, 
		     cldata->proxycldata.pic_data,
		     ntohl( cldata->proxycldata.picsize ));
  }    

  return SOCKFIELD_OK;
}

/*
 * write_client_regular:  Called when a regular client has been set writeable.
 *                        in such a case we switch between all of the different
 *                        states of a client and send the appropriate thing.
 *
 * Return values:         Returns one of SOCKFIELD_*
 */

static
int write_client_regular( SockField_Data *sfdata, Socket *sock,
			  ClientData *cldata )
{
  if( cldata->browserdata.browserstate == BROWSERSTATE_SEND_PREAMBLE ||
      cldata->browserdata.browserstate == BROWSERSTATE_SEND_SEPERATOR )
  {
    char *pic_data;
    size_t pic_size;
    int pic_id;

    cldata->browserdata.management_data = 
      manager_new_client( &pic_data, &pic_size, &pic_id );
    if( cldata->browserdata.management_data == NULL ){
      camserv_log( MODNAME, "Error managing client!  "
		   "(no pictures may be available yet)");
      sock_field_hold_write( sfdata, sock );
      return SOCKFIELD_OK;
    }

    if( pic_id == cldata->browserdata.last_pic_id ) { /* Client reads fast */
      manager_dest_client( cldata->browserdata.management_data );
      cldata->browserdata.management_data = NULL;
      sock_field_hold_write( sfdata, sock );
      return SOCKFIELD_OK;
    }

    databuf_buf_set( cldata->writebuf, pic_data, pic_size );
    cldata->browserdata.last_pic_id = pic_id;
    cldata->browserdata.browserstate = BROWSERSTATE_SEND_PICTURE;
  } else {
    char *sep_data;
    size_t sep_size;
    
    if( manager_dest_client( cldata->browserdata.management_data ) == -1 ){
      camserv_log( MODNAME, "Error destroying client management!");
    }
    cldata->browserdata.management_data = NULL;
    cldata->browserdata.browserstate = BROWSERSTATE_SEND_SEPERATOR;
    sep_data = get_seperator_text( &sep_size );
    databuf_buf_set( cldata->writebuf, sep_data, sep_size );
  }
  return SOCKFIELD_OK;
}

static
int write_client( SockField_Data *sfdata, Socket *sock, ClientData *cldata ){
  /* If client hasn't initialized itself with us yet, put him on hold */
  if( cldata->clienttype == CLIENT_T_UNK ){
    sock_field_hold_write( sfdata, sock );
    return SOCKFIELD_OK;
  }

  if( cldata->clienttype == CLIENT_T_PROXY )
    return write_client_proxy( sfdata, sock, cldata );
  else
    return write_client_regular( sfdata, sock, cldata );

  return SOCKFIELD_OK;
}

static
int relay_client_write( SockField_Data *sfdata, Socket *sock, void *cldata ){
  ClientData *clientdata = cldata;
  int writeres;

  writeres = databuf_write( clientdata->writebuf, socket_query_fd( sock ));
  if( writeres == -1 ) return SOCKFIELD_CLOSE;
  if( writeres ==  1 ) return SOCKFIELD_OK;  /* Still data left to write */
  /* Else we have written all the data we were supposed to */

  if( clientdata->clienttype == CLIENT_T_CAMSERV ) 
    return write_camera( sfdata, sock, clientdata );
  else
    return write_client( sfdata, sock, clientdata );
  
  return SOCKFIELD_OK;
}

static
void relay_accept( SockField_Data *sfdata, Socket *listen_sock,
		   void *sys_cldata )
{
  ClientData *cldata;
  Socket *new_socket;

  if( (new_socket = socket_accept( listen_sock )) == NULL ){
    camserv_log( MODNAME, "Error accepting new socket!");
    return;
  }

  if( (cldata = client_data_new()) == NULL ){
    camserv_log( MODNAME, "Error allocating clientdata!");
    socket_dest( new_socket );
    return;
  }

  cldata->clienttype = CLIENT_T_UNK;
  if( sock_field_manage_socket( sfdata, new_socket, cldata ) == -1 ){
    camserv_log( MODNAME, "Error managing new socket" );
    socket_dest( new_socket );
    client_data_dest( cldata );
    return;
  }
  camserv_log( MODNAME, "Accepted client: \"%s\"", 
	       socket_query_remote_name( new_socket ));
}


static
void relay_timeout( SockField_Data *sfdata, void *sys_cldata ){
  RelayData *rdata = sys_cldata;
  
  if( rdata->camera_sock == NULL ) {
    camserv_log( MODNAME, "Attempting connect to camserver");
    if( relay_connect_camserv( sfdata, rdata ) == -1 ){
      camserv_log( MODNAME, "Error connecting to camserv: %s %d",
	       rdata->camera_ip, rdata->camera_port );
    }
  }
}


int main( int argc, char *argv[] ){
  Socket *listen_sock;
  int localport, remoteport;
  RelayData rdata;
  struct timeval retry_time;

  if( argc < 3 ) {
    fprintf( stderr, "camserv relay v%s - by Jon Travis (jon@dsndata.com)\n",
	     VERSION );
    fprintf( stderr, "Syntax: %s <port> <camserv IP> <ccamserv port>\n", 
	     argv[ 0 ]);
    fprintf( stderr, "\tThe relay will bind port <port> on the localhost\n");
    fprintf( stderr, "\tand connect to the camserv port to do the relay.\n");
    return -1;
  }

  signal( SIGPIPE, SIG_IGN );
  if( sscanf( argv[1], "%d", &localport ) != 1 ) {
    camserv_log( MODNAME, "Error:  port \"%s\" invalid!", argv[ 1 ] );
    return -1;
  }

  if( localport < 1024 )
    camserv_log( MODNAME, "Warning:  Port numbers < 1024 shouldn't be used");

  if( sscanf( argv[3], "%d", &remoteport ) != 1 ) {
    camserv_log( MODNAME, "Error:  camserv port \"%s\" invalid!", argv[3] );
    return -1;
  }

  strncpy( rdata.camera_ip, argv[2], sizeof( rdata.camera_ip ) - 1);
  rdata.camera_ip[ sizeof( rdata.camera_ip ) - 1 ] = '\0';
  rdata.camera_port = remoteport;
  rdata.camera_sock = NULL;
  if( (listen_sock = socket_serve_tcp( NULL, localport, BACKLOG )) == NULL ){
    camserv_log( MODNAME, "Could not bind local port: %s", 
	     strerror( errno ));
    return -1;
  }

  retry_time.tv_sec = 10;
  retry_time.tv_usec = 0;
  if( sock_field( listen_sock, &rdata, relay_init, relay_accept, 
		  relay_client_read, relay_client_write,
		  relay_preclose, relay_timeout, &retry_time ) == -1 )
  {
    socket_dest( listen_sock );
    camserv_log( MODNAME, "Camserv relay abnormally terminated");
    return -1;
  }
  socket_dest( listen_sock );
  return 0;
}
