/*
 * server.c - Main file for the Input Pipe server. It accepts
 *            connections from clients and creates corresponding
 *            input devices on this machine via the 'uinput' device.
 *
 * Input Pipe, network transparency for the Linux input layer
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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


/* Our server's representation of one input client */
struct client {
  int tcp_fd;
  int uinput_fd;

  struct sockaddr addr;
  socklen_t addrlen;

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


static struct client*   client_new         (int socket_fd);
static void             client_delete      (struct client* self);
static void             client_poll        (struct client* self);

static void             client_list_remove (struct client* client);
static void             client_list_insert (struct client* client);

static struct listener* listener_new       (int sock_type, int port);
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

  /* Create and init the client itself */
  self = malloc(sizeof(struct client));
  assert(self != NULL);
  memset(self, 0, sizeof(struct client));

  /* Accept the client's connection, save this as our TCP socket */
  self->addrlen = sizeof(self->addr);
  self->tcp_fd = accept(socket_fd, &self->addr, &self->addrlen);
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

  return self;
}

static void client_delete(struct client* self)
{
  if (self->tcp_fd > 0) {
    FD_SET(self->tcp_fd, &fd_request_read);
    close(self->tcp_fd);
  }
  if (self->uinput_fd > 0) {
    FD_CLR(self->uinput_fd, &fd_request_read);
    close(self->uinput_fd);
  }
  free(self);
}

static void client_poll(struct client* self)
{
}


/***********************************************************************/
/******************************************************* Client List ***/
/***********************************************************************/

static void client_list_remove(struct client* client)
{
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

      listener_poll(tcp_listener);

    }
  }
  return 0;
}


int main() {
  return main_loop();
}

/* The End */
