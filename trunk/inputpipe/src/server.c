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
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/select.h>
#include <linux/tcp.h>

#include <linux/input.h>
#include <uinput.h>
#include "inputpipe.h"
#include "packet.h"


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

  struct sockaddr_in addr;
  struct packet_socket *socket;

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
static int   config_tcp_port    = IPIPE_DEFAULT_PORT;
static int   config_verbose     = 1;
static int   config_inetd_mode  = 0;

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

static struct client*   client_new             (int tcp_fd,
						struct sockaddr_in *addr);
static void             client_delete          (struct client* self);
static void             client_poll            (struct client* self);
static const char*      client_format_addr     (struct client* self);
static void             client_received_packet (struct client* self,
						int type,
						int length,
						void* content);
static void             client_received_event  (struct client* self,
						struct input_event *ev);

static void             client_list_remove     (struct client* client);
static void             client_list_insert     (struct client* client);

static struct listener* listener_new           (int sock_type,
						int port);
static void             listener_delete        (struct listener* self);
static void             listener_poll          (struct listener* self);

static int              main_loop              (void);

/* Variadic macros are a gcc extension, but this by definition
 * requires linux so why not require gcc too...
 */
#define client_message(self, fmt, ...) do { \
    if (config_verbose) \
        fprintf(stderr, "[Client %s] " fmt "\n", client_format_addr(self), ## __VA_ARGS__); \
  } while (0);


/***********************************************************************/
/******************************************************* Client ********/
/***********************************************************************/

/* Accept a connection from the given socket fd, returning
 * a new client for that connection.
 */
static struct client* client_new(int tcp_fd, struct sockaddr_in *addr)
{
  struct client *self;

  /* Create and init the client itself */
  self = malloc(sizeof(struct client));
  assert(self != NULL);
  memset(self, 0, sizeof(struct client));

  /* If we got the address of our client, save that */
  if (addr)
    memcpy(&self->addr, addr, sizeof(*addr));

  self->socket = packet_socket_new(tcp_fd);
  assert(self->socket);

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
  FD_SET(self->socket->fd, &fd_request_read);
  if (self->socket->fd >= fd_count)
    fd_count = self->socket->fd + 1;

  return self;
}

static void client_delete(struct client* self)
{
  if (self->socket) {
    FD_CLR(self->socket->fd, &fd_request_read);
    packet_socket_delete(self->socket);
  }
  if (self->uinput_fd > 0) {
    FD_CLR(self->uinput_fd, &fd_request_read);
    close(self->uinput_fd);
  }
  free(self);
}

static const char* client_format_addr (struct client* self)
{
  if (self->socket->fd == fileno(stdin)) {
    return "stdin";
  }
  else {
    static char buffer[25];
    unsigned char *ip = (unsigned char*) &self->addr.sin_addr.s_addr;
    int port = ntohs(self->addr.sin_port);
    sprintf(buffer, "%d.%d.%d.%d:%d",
	    ip[0], ip[1], ip[2], ip[3], port);
    return buffer;
  }
}

static void client_poll(struct client* self)
{
  /* Is there data waiting on our uinput device? */
  if (FD_ISSET(self->uinput_fd, &fd_read)) {
    struct input_event ev;
    if (read(self->uinput_fd, &ev, sizeof(ev)) == sizeof(ev))
      client_received_event(self, &ev);
  }

  /* Is our TCP socket ready? */
  if (FD_ISSET(self->socket->fd, &fd_read)) {
    int length, type;
    void* content;

    while (1) {
      type = packet_socket_read(self->socket, &length, &content);
      if (!type)
	break;
      client_received_packet(self, type, length, content);
      free(content);
    }

    if (feof(self->socket->file)) {
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

  case IPIPE_EVENT:
    {
      struct ipipe_event* ip_ev = (struct ipipe_event*) content;
      struct input_event ev;
      if (length != sizeof(struct ipipe_event)) {
	client_message(self, "Received IPIPE_EVENT with incorrect length");
	break;
      }
      if (!self->device_created) {
	client_message(self, "Received IPIPE_EVENT before IPIPE_CREATE");
	break;
      }
      ntoh_input_event(&ev, ip_ev);
      write(self->uinput_fd, &ev, sizeof(ev));
    }
    break;

  case IPIPE_DEVICE_NAME:
    /* Truncate to uinput's limit and copy to our dev_info */
    if (length >= UINPUT_MAX_NAME_SIZE)
      length = UINPUT_MAX_NAME_SIZE-1;
    memcpy(self->dev_info.name, content, length);
    self->dev_info.name[length] = 0;
    break;

  case IPIPE_DEVICE_PHYS:
    /* The client's given us their original physical path. We use
     * this to build a full path of the form ipipe://client:port/path
     */
    {
      char buffer[1024];
      int prefix_len, copy_len;
      prefix_len = snprintf(buffer, sizeof(buffer), IPIPE_PHYS_PREFIX "%s/",
			    client_format_addr(self));
      copy_len = sizeof(buffer) - prefix_len - 1;
      if (copy_len > length)
	copy_len = length;
      memcpy(buffer+prefix_len, content, copy_len);
      buffer[prefix_len + copy_len] = '\0';
      if (ioctl(self->uinput_fd, UI_SET_PHYS, buffer) < 0)
	perror("ioctl UI_SET_PHYS");
    }
    break;

  case IPIPE_DEVICE_ID:
    {
      struct ipipe_input_id* ip_id = (struct ipipe_input_id*) content;
      if (length != sizeof(struct ipipe_input_id)) {
	client_message(self, "Received IPIPE_DEVICE_ID with incorrect length");
	break;
      }
      ntoh_input_id(&self->dev_info.id, ip_id);
    }
    break;

  case IPIPE_DEVICE_FF_EFFECTS_MAX:
    if (length != sizeof(uint32_t)) {
      client_message(self, "Received a IPIPE_DEVICE_FF_EFFECTS_MAX with incorrect length");
      return;
    }
    self->dev_info.ff_effects_max = ntohl(*(uint32_t*)content);
    break;

  case IPIPE_DEVICE_ABSINFO:
    {
      struct ipipe_absinfo* ip_abs = (struct ipipe_absinfo*) content;
      int axis;
      if (length != sizeof(struct ipipe_absinfo)) {
	client_message(self, "Received IPIPE_DEVICE_ABSINFO with incorrect length");
	break;
      }
      axis = ntohl(ip_abs->axis);
      if (axis > ABS_MAX) {
	client_message(self, "Received IPIPE_DEVICE_ABSINFO with out-of-range axis (%d)", axis);
	break;
      }
      self->dev_info.absmax[axis] = ntohl(ip_abs->max);
      self->dev_info.absmin[axis] = ntohl(ip_abs->min);
      self->dev_info.absfuzz[axis] = ntohl(ip_abs->fuzz);
      self->dev_info.absflat[axis] = ntohl(ip_abs->flat);

      /* Validate the absinfo. If we don't catch problems here and fix
       * them, (with a warning) the device creation will fail completely.
       */
      if (self->dev_info.absmax[axis] < self->dev_info.absmin[axis]+2) {
	client_message(self, "Warning, axis %d has invalid min/max", axis);
	self->dev_info.absmax[axis] = self->dev_info.absmin[axis]+2;
      }
      if (self->dev_info.absflat[axis] <= self->dev_info.absmin[axis]) {
	client_message(self, "Warning, axis %d absflat is too low", axis);
	self->dev_info.absflat[axis] = self->dev_info.absmin[axis]+1;
      }
      if (self->dev_info.absflat[axis] >= self->dev_info.absmax[axis]) {
	client_message(self, "Warning, axis %d absflat is too high", axis);
	self->dev_info.absflat[axis] = self->dev_info.absmax[axis]-1;
      }
    }
    break;

  case IPIPE_DEVICE_BITS:
    {
      int bit, ev, ioc, i, j;
      unsigned char *p;
      unsigned char byte;

      if (length < sizeof(uint16_t)) {
	client_message(self, "Received IPIPE_DEVICE_BITS with incorrect length");
	return;
      }

      /* Our packet starts with the EV_* type code */
      ev = ntohs(*(uint16_t*)content);

      /* Convert the type code into a uinput ioctl */
      switch (ev) {
      case      0: ioc = UI_SET_EVBIT;  break;
      case EV_KEY: ioc = UI_SET_KEYBIT; break;
      case EV_REL: ioc = UI_SET_RELBIT; break;
      case EV_ABS: ioc = UI_SET_ABSBIT; break;
      case EV_MSC: ioc = UI_SET_MSCBIT; break;
      case EV_LED: ioc = UI_SET_LEDBIT; break;
      case EV_SND: ioc = UI_SET_SNDBIT; break;
      case EV_FF:  ioc = UI_SET_FFBIT;  break;
      default:
	ioc = 0;
      }

      if (!ioc) {
	client_message(self, "Received bits for unknown event type %d", ev);
	break;
      }

      /* Start unpacking the bits and setting them in ioctl()s */
      p = ((unsigned char *)content)+sizeof(uint16_t);
      bit = 0;
      for (i=0; i < (length-sizeof(uint16_t)); i++) {
	byte = p[i];
	for (j=0; j<8; j++) {
	  if (byte & 1) {
	    ioctl(self->uinput_fd, ioc, bit);
	  }
	  byte >>= 1;
	  bit++;
	}
      }
    }
    break;

  case IPIPE_CREATE:
    {
      /* Yay, send the uinput_user_dev and actually create our device */
      int write_res, ioctl_res;
      if (self->device_created) {
	client_message(self, "Duplicate IPIPE_CREATE received");
	break;
      }
      write_res = write(self->uinput_fd, &self->dev_info, sizeof(self->dev_info));
      ioctl_res = ioctl(self->uinput_fd, UI_DEV_CREATE, 0);
      if (write_res < 0 || ioctl_res < 0) {
	client_message(self, "Failed to create new device \"%s\"", self->dev_info.name);
	break;
      }
      self->device_created = 1;
      client_message(self, "Created new device \"%s\"", self->dev_info.name);

      /* Now we can safely register our uinput device for select() */
      FD_SET(self->uinput_fd, &fd_request_read);
      if (self->uinput_fd >= fd_count)
	fd_count = self->uinput_fd + 1;
    }
    break;

  case IPIPE_UPLOAD_EFFECT_RESPONSE:
    {
      struct ipipe_upload_effect_response* ipipe_up = (struct ipipe_upload_effect_response*) content;
      struct uinput_ff_upload ff_up;
      if (length != sizeof(struct ipipe_upload_effect_response)) {
	client_message(self, "Received IPIPE_UPLOAD_EFFECT_RESPONSE with incorrect length");
	break;
      }
      ff_up.request_id = ntohl(ipipe_up->request_id);
      ff_up.retval = ntohl(ipipe_up->retval);
      ntoh_ff_effect(&ff_up.effect, &ipipe_up->effect);
      if (ioctl(self->uinput_fd, UI_END_FF_UPLOAD, &ff_up) < 0)
	perror("ioctl UI_END_FF_UPLOAD");
    }
    break;

  case IPIPE_ERASE_EFFECT_RESPONSE:
    {
      struct ipipe_erase_effect_response* ipipe_erase = (struct ipipe_erase_effect_response*) content;
      struct uinput_ff_erase ff_erase;
      if (length != sizeof(struct ipipe_erase_effect_response)) {
	client_message(self, "Received IPIPE_ERASE_EFFECT_RESPONSE with incorrect length");
	break;
      }
      ff_erase.request_id = ntohl(ipipe_erase->request_id);
      ff_erase.retval = ntohl(ipipe_erase->retval);
      ff_erase.effect_id = -1;
      if (ioctl(self->uinput_fd, UI_END_FF_ERASE, &ff_erase) < 0)
	perror("ioctl UI_END_FF_ERASE");
    }
    break;

  default:
    client_message(self, "Received unknown packet type 0x%04X", type);
  }
}

static void client_received_event(struct client* self,
				  struct input_event *ev)
{
  /* We got an app to device input event from uinput. In most
   * cases we forward these to the corresponding client, but
   * we process EV_UINPUT events here. For EV_UINPUT events,
   * we issue an ioctl to get the rest of the necessary
   * data, then we form a packet with that.
   */

  struct uinput_ff_upload ff_up;
  struct uinput_ff_erase ff_erase;
  struct ipipe_upload_effect ipipe_up;
  struct ipipe_erase_effect ipipe_erase;

  if (ev->type == EV_UINPUT) {
    switch (ev->code) {

    case UI_FF_UPLOAD:
      /* Upload a force feedback effect */
      ff_up.request_id = ev->value;
      if (ioctl(self->uinput_fd, UI_BEGIN_FF_UPLOAD, &ff_up) < 0) {
	perror("ioctl UI_BEGIN_FF_UPLOAD");
	return;
      }
      ipipe_up.request_id = htonl(ff_up.request_id);
      hton_ff_effect(&ipipe_up.effect, &ff_up.effect);
      packet_socket_write(self->socket, IPIPE_UPLOAD_EFFECT, sizeof(ipipe_up), &ipipe_up);
      break;

    case UI_FF_ERASE:
      /* Erase a force feedback effect */
      ff_erase.request_id = ev->value;
      if (ioctl(self->uinput_fd, UI_BEGIN_FF_ERASE, &ff_erase) < 0) {
	perror("ioctl UI_BEGIN_FF_ERASE");
	return;
      }
      ipipe_erase.request_id = htonl(ff_erase.request_id);
      ipipe_erase.effect_id = htons(ff_erase.effect_id);
      packet_socket_write(self->socket, IPIPE_ERASE_EFFECT, sizeof(ipipe_erase), &ipipe_erase);
      break;

    }
  }
  else {
    /* Send a normal event to the client */
    struct ipipe_event ip_ev;
    hton_input_event(&ip_ev, ev);
    packet_socket_write(self->socket, IPIPE_EVENT, sizeof(ip_ev), &ip_ev);
  }

  packet_socket_flush(self->socket);
}


/***********************************************************************/
/******************************************************* Client List ***/
/***********************************************************************/

static void client_list_remove(struct client* client)
{
  client_message(client, "Removed");
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
  client_message(client, "Connected");
  assert(client->prev == NULL);
  assert(client->next == NULL);
  if (client_list_tail) {
    client->prev = client_list_tail;
    client_list_tail->next = client;
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
    perror("Setting SO_REUSEADDR");
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

  FD_SET(self->fd, &fd_request_read);
  if (self->fd >= fd_count)
    fd_count = self->fd + 1;

  if (config_verbose) {
    fprintf(stderr, "Listening on port %d\n", port);
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
  socklen_t addrlen = sizeof(struct sockaddr_in);
  struct sockaddr_in in_addr;

  if (FD_ISSET(self->fd, &fd_read)) {
    /* Accept the client's connection, save this as our TCP socket */
    int fd = accept(self->fd, (struct sockaddr*) &in_addr, &addrlen);
    if (fd < 0) {
      perror("Accepting client");
    }
    else {
      c = client_new(fd, &in_addr);
      if (c)
	client_list_insert(c);
    }
  }
}


/***********************************************************************/
/******************************************************* Main Loop *****/
/***********************************************************************/

static int main_loop(void) {
  struct listener *tcp_listener;
  struct client *client_iter, *next_client;
  int n;

  FD_ZERO(&fd_request_read);
  FD_ZERO(&fd_request_write);
  FD_ZERO(&fd_request_except);

  if (config_inetd_mode) {
    /* In inetd mode, we only have one client connected to stdin.
     * Otherwise, we create a listener that generates new clients for us.
     */
    struct client *c;
    tcp_listener = NULL;
    c = client_new(fileno(stdin), NULL);
    if (c == NULL)
      return 1;
    client_list_insert(c);
  }
  else {
    tcp_listener = listener_new(SOCK_STREAM, config_tcp_port);
    if (!tcp_listener)
      return 1;
  }

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
      if (tcp_listener)
	listener_poll(tcp_listener);

      /* Poll all clients */
      client_iter = client_list_head;
      while (client_iter) {
	/* Save the next client first, as this one might remove itself */
	next_client = client_iter->next;
	client_poll(client_iter);
	client_iter = next_client;
      }
    }

    /* In inetd mode, die after our client disconnects */
    if (config_inetd_mode && !client_list_head)
      break;
  }
  return 0;
}

void usage(char *progname) {
  fprintf(stderr,
	  "Usage: %s [options]\n"
	  "\n"
	  "Wait for inputpipe-client processes to connect. Each client process\n"
	  "can create and control one input device on this system, via the Linux\n"
	  "'uinput' device.\n"
	  "\n"
	  "  -h, --help                     This text\n"
	  "  -d PATH, --uinput-device=PATH  Set the location of the uinput device node.\n"
	  "                                 [%s]\n"
	  "  -p PORT, --port=PORT           Set the port number to listen on [%d]\n"
	  "  -i, --inetd                    For running from inetd\n"
	  "  -q, --quiet                    Suppress normal log output\n",
	  progname, config_uinput_path, config_tcp_port);
}

int main(int argc, char **argv) {
  int c;

  while (1) {
    static struct option long_options[] = {
      {"help",          0, 0, 'h'},
      {"uinput-device", 1, 0, 'd'},
      {"port",          1, 0, 'p'},
      {"quiet",         0, 0, 'q'},
      {"inetd",         0, 0, 'i'},
      {0},
    };

    c = getopt_long(argc, argv, "d:p:qi",
		    long_options, NULL);
    if (c == -1)
      break;
    switch (c) {

    case 'd':
      config_uinput_path = strdup(optarg);
      break;

    case 'p':
      config_tcp_port = atoi(optarg);
      break;

    case 'q':
      config_verbose = 0;
      break;

    case 'i':
      config_inetd_mode = 1;
      break;

    case 'h':
    default:
      usage(argv[0]);
      return 1;
    }
  }

  if (optind < argc) {
    /* Extra arguments */
    usage(argv[0]);
    return 1;
  }

  return main_loop();
}

/* The End */
