/*
 * server.c - Main file for the Input Pipe server. It accepts
 *            connections from clients and creates corresponding
 *            input devices on this machine via the 'uinput' device.
 *
 * Input Pipe, network transparency for the Linux input layer
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/select.h>

#include <linux/input.h>
#include <linux/uinput.h>
#include "inputpipe.h"


/* Our server's representation of one input client */
struct client {
  int uinput_fd;

  /* The uinput device starts out waiting for information
   * about the device to create. In this state, device_created
   * will be zero. The uinput_user_dev  here will start to be filled
   * with information about the device. When it's created,
   * we send the uinput_user_dev to the kernel, add the uinput_fd
   * to our select() lists, and set device_created=1.
   */
  int device_created;
  struct uinput_user_dev dev_info;

  int tcp_fd;
  struct sockaddr_in addr;

  /* We use buffered I/O to receive our packets over TCP.
   * fread() will guarantee that we don't receive only part
   * of a header or part of the content, but we might get
   * stuck between the header and content.
   */
  FILE *tcp_file;
  struct inputpipe_packet tcp_packet;
  int received_header;

  struct client *next, *prev;
};

/* The listener object listens for new connections on a port */
struct listener {
  int fd;
  int sock_type;
  int port;
};


/* Configuration options */
static char* config_uinput_path = "/dev/uinput";
static int   config_tcp_port    = 5591;
static int   config_verbose     = 1;

/* Global doubly-linked list of clients */
static struct client* client_list_head = NULL;
static struct client* client_list_tail = NULL;

/* Global list of FDs each object is interested in,
 * and FDs that there have been activity on. Each object
 * adds itself to the fd_request_* lists on creation and removes
 * itself on deletion, and checks the fd_* lists on poll.
 */
static fd_set fd_request_read, fd_request_write, fd_request_except;
static fd_set fd_read, fd_write, fd_except;
int fd_count = 0;

static struct client*   client_new             (int socket_fd);
static void             client_delete          (struct client* self);
static void             client_poll            (struct client* self);
static const char*      client_format_addr     (struct client* self);
static void             client_received_packet (struct client* self,
						int type,
						int length,
						void *content);

static void             client_list_remove (struct client* client);
static void             client_list_insert (struct client* client);

static struct listener* listener_new       (int sock_type,
					    int port);
static void             listener_delete    (struct listener* self);
static void             listener_poll      (struct listener* self);

static int              main_loop(void);


/***********************************************************************/
/******************************************************* Client ********/
/***********************************************************************/

/* Accept a connection from the given socket fd, returning
 * a new client for that connection.
 */
static struct client* client_new(int socket_fd)
{
  struct client *self;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  /* Create and init the client itself */
  self = malloc(sizeof(struct client));
  assert(self != NULL);
  memset(self, 0, sizeof(struct client));

  /* Accept the client's connection, save this as our TCP socket */
  self->tcp_fd = accept(socket_fd, (struct sockaddr*) &self->addr, &addrlen);
  if (self->tcp_fd < 0) {
    perror("Accepting client");
    client_delete(self);
    return NULL;
  }

  /* Open the uinput device for this input client. Our clients
   * and our uinput devices both only support a single device.
   */
  self->uinput_fd = open(config_uinput_path, O_RDWR | O_NDELAY);
  if (self->uinput_fd < 0) {
    perror("Opening uinput");
    client_delete(self);
    return NULL;
  }

  /* Register the socket's fd, but don't register uinput until
   * the client has registered a new device. select() on a uinput
   * device in this state will cause an oops on some kernels.
   */
  FD_SET(self->tcp_fd, &fd_request_read);
  if (self->tcp_fd >= fd_count)
    fd_count = self->tcp_fd + 1;

  /* Create a FILE* for our socket, to make buffered I/O easy */
  self->tcp_file = fdopen(self->tcp_fd, "rw");
  assert(self->tcp_file);

  return self;
}

static void client_delete(struct client* self)
{
  if (self->tcp_fd > 0) {
    FD_CLR(self->tcp_fd, &fd_request_read);
    fclose(self->tcp_file);
  }
  if (self->uinput_fd > 0) {
    FD_CLR(self->uinput_fd, &fd_request_read);
    close(self->uinput_fd);
  }
  free(self);
}

static const char* client_format_addr (struct client* self)
{
  static char buffer[25];
  unsigned char *ip = (unsigned char*) &self->addr.sin_addr.s_addr;
  int port = ntohs(self->addr.sin_port);
  sprintf(buffer, "%d.%d.%d.%d:%d",
	  ip[0], ip[1], ip[2], ip[3], port);
  return buffer;
}

static void client_poll(struct client* self)
{
  /* Is our TCP socket ready? */
  if (FD_ISSET(self->tcp_fd, &fd_read)) {

    while (1) {
      /* Already received a packet header? Try to get the content */
      if (self->received_header) {
	int type = ntohs(self->tcp_packet.type);
	int length = ntohs(self->tcp_packet.length);

	if (length > 0) {
	  /* We have content to receive... */
	  void *content = malloc(length);
	  assert(content != NULL);

	  if (fread(content, length, 1, self->tcp_file)) {
	    /* Yay, got a whole packet to process */
	    self->received_header = 0;
	    client_received_packet(self, type, length, content);
	    free(content);
	  }
	  else {
	    /* Can't do anything else until we get the content */
	    free(content);
	    break;
	  }
	}
	else {
	  /* This packet included no content, we're done */
	  self->received_header = 0;
	  client_received_packet(self, type, length, NULL);
	}
      }

      /* See if we can get another header */
      if (fread(&self->tcp_packet, sizeof(self->tcp_packet), 1, self->tcp_file)) {
	/* Yep. Next we'll try to get the content. */
	self->received_header = 1;
      }
      else
	break;
    }

    if (feof(self->tcp_file)) {
      /* Connection closed, self-destruct this client */
      client_list_remove(self);
      client_delete(self);
      return;
    }
  }
}

static void client_received_packet(struct client* self,
				   int type, int length, void *content)
{
  switch (type) {

  case IPIPE_DEVICE_NAME:
    /* Truncate to uinput's limit and copy to our dev_info */
    if (length >= UINPUT_MAX_NAME_SIZE)
      length = UINPUT_MAX_NAME_SIZE-1;
    memcpy(self->dev_info.name, content, length);
    self->dev_info.name[length] = 0;
    break;

  case IPIPE_CREATE:
    /* Yay, send the uinput_user_dev and actually create our device */
    if (self->device_created) {
      if (config_verbose) {
	printf("Client %s: Duplicate IPIPE_CREATE received\n",
	       client_format_addr(self));
      }
      break;
    }
    write(self->uinput_fd, &self->dev_info, sizeof(self->dev_info));
    ioctl(self->uinput_fd, UI_DEV_CREATE, 0);
    self->device_created = 1;
    if (config_verbose) {
      printf("Client %s: Created new device \"%s\"\n",
	     client_format_addr(self), self->dev_info.name);
    }
    break;

  default:
    if (config_verbose) {
      printf("Client %s: Received unknown packet type 0x%04X\n",
	     client_format_addr(self), type);
    }
  }
}


/***********************************************************************/
/******************************************************* Client List ***/
/***********************************************************************/

static void client_list_remove(struct client* client)
{
  if (config_verbose) {
    printf("Removed client from %s\n", client_format_addr(client));
  }

  if (client->prev) {
    client->prev->next = client->next;
  }
  else {
    assert(client_list_head == client);
    client_list_head = client->next;
  }
  if (client->next) {
    client->next->prev = client->prev;
  }
  else {
    assert(client_list_tail == client);
    client_list_tail = client->prev;
  }
}

static void client_list_insert(struct client* client)
{
  if (config_verbose) {
    printf("New client from %s\n", client_format_addr(client));
  }

  assert(client->prev == NULL);
  assert(client->next == NULL);
  if (client_list_tail) {
    client->prev = client_list_tail;
  }
  else {
    assert(client_list_head == NULL);
    client_list_head = client;
  }
  client_list_tail = client;
}


/***********************************************************************/
/******************************************************* Listener ******/
/***********************************************************************/

static struct listener* listener_new(int sock_type, int port)
{
  struct listener *self;
  struct sockaddr_in in_addr;
  int opt;

  self = malloc(sizeof(struct listener));
  assert(self != NULL);
  memset(self, 0, sizeof(struct listener));

  self->sock_type = sock_type;
  self->port = port;

  self->fd = socket(PF_INET, SOCK_STREAM, 0);
  if (self->fd < 0) {
    perror("socket");
    listener_delete(self);
    return NULL;
  }

  opt = 1;
  if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt))) {
    perror("setsockopt");
    listener_delete(self);
    return NULL;
  }

  memset(&in_addr, 0, sizeof(in_addr));
  in_addr.sin_family = AF_INET;
  in_addr.sin_addr.s_addr = INADDR_ANY;
  in_addr.sin_port = htons(port);
  if (bind(self->fd, (struct sockaddr*) &in_addr, sizeof(in_addr))) {
    perror("listen");
    listener_delete(self);
    return NULL;
  }
  if (listen(self->fd, 10)) {
    perror("listen");
    listener_delete(self);
    return NULL;
  }

  fcntl(self->fd, F_SETFL, fcntl(self->fd, F_GETFL, 0) | O_NONBLOCK);
  FD_SET(self->fd, &fd_request_read);
  if (self->fd >= fd_count)
    fd_count = self->fd + 1;

  if (config_verbose) {
    printf("Listening on port %d\n", port);
  }

  return self;
}

static void listener_delete(struct listener* self)
{
  if (self->fd > 0) {
    FD_CLR(self->fd, &fd_request_read);
    close(self->fd);
  }
  free(self);
}

static void listener_poll(struct listener* self)
{
  struct client *c;

  if (FD_ISSET(self->fd, &fd_read)) {
    c = client_new(self->fd);
    if (c)
      client_list_insert(c);
  }
}


/***********************************************************************/
/******************************************************* Main Loop *****/
/***********************************************************************/

static int main_loop(void) {
  struct listener *tcp_listener;
  struct client *client_iter;
  int n;

  FD_ZERO(&fd_request_read);
  FD_ZERO(&fd_request_write);
  FD_ZERO(&fd_request_except);

  tcp_listener = listener_new(SOCK_STREAM, config_tcp_port);
  if (!tcp_listener)
    return 1;

  while (1) {
    memcpy(&fd_read, &fd_request_read, sizeof(fd_set));
    memcpy(&fd_write, &fd_request_write, sizeof(fd_set));
    memcpy(&fd_except, &fd_request_except, sizeof(fd_set));

    n = select(fd_count, &fd_read, &fd_write, &fd_except, NULL);
    if (n<0) {
      perror("select");
      return 1;
    }
    else if (n>0) {

      /* Poll all listeners */
      listener_poll(tcp_listener);

      /* Poll all clients */
      for (client_iter=client_list_head; client_iter; client_iter=client_iter->next)
	client_poll(client_iter);

    }
  }
  return 0;
}


int main() {
  return main_loop();
}

/* The End */
