/*
 * client.c - Main file for the Input Pipe client. It forwards
 *            events and metadata from a Linux event device to
 *            the inputpipe server.
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/select.h>
#include <dirent.h>
#include <linux/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <getopt.h>
#include <linux/input.h>

#include "inputpipe.h"

/* Linux 2.4 compatibility */
#ifndef EV_SYN
struct input_absinfo {
  __s32 value;
  __s32 minimum;
  __s32 maximum;
  __s32 fuzz;
  __s32 flat;
};
#endif

#define test_bit(nr, addr) \
        (((1UL << ((nr) & 31)) & (((const unsigned int *) addr)[(nr) >> 5])) != 0)

struct buffer {
  unsigned char data[4096];
  int remaining;
  unsigned char *current;
};

struct server {
  int tcp_fd;
  FILE *tcp_file;
  struct buffer tcp_buffer;

  char *host;
  int port;

  /* Header and flag used for tracking receive state. stdio
   * buffering will ensure we don't get a partial header or
   * content, but we need to be able to receive a header but
   * wait before getting content.
   */
  struct inputpipe_packet tcp_packet;
  int received_header;
};

/* One input device and one server struct. We have one connection
 * for each opened input device we're forwarding.
 */
struct connection {
  char *evdev_path;
  int evdev_fd;
  struct server* server;

  int in_connection_list;
  struct connection *prev, *next;
};

/* Global linked list of connections */
static struct connection *connection_list_head = NULL;
static struct connection *connection_list_tail = NULL;

/* Configuration, set via the command line */
static int config_verbose = 1;
static char *config_host_and_port = NULL;
static int config_hotplug_polling = 0;
static int config_detect_joystick = 0;
static int config_detect_mouse = 0;
static int config_detect_keyboard = 0;
static int config_detect_all = 0;
static char *config_input_path = "/dev/input";
static int config_connect_led_number = -1;
static int config_connect_led_polarity = 1;

#define connection_message(self, fmt, ...) do { \
    if (config_verbose) \
        fprintf(stderr, "[Device %s] " fmt "\n", self->evdev_path, ## __VA_ARGS__); \
  } while (0);

static struct server*     server_new          (const char *host_and_port);
static void               server_delete       (struct server *self);

static void               server_write        (struct server *self,
					       int packet_type,
					       int length,
					       void *content);
static int                server_read         (struct server *self,
					       int *length,
					       void **content);
static void               server_flush        (struct server *self);

static int                evdev_new           (const char *path);
static void               evdev_delete        (int evdev);
static int                evdev_send_metadata (int evdev, struct server *svr);
static int                evdev_send_event    (int evdev, struct server *svr);

static struct connection* connection_new      (const char *evdev_path,
					       const char *host_and_port);
static void               connection_delete   (struct connection *self);
static void               connection_add_fds  (struct connection *self,
					       int *fd_max,
					       fd_set *fd_read);
static int                connection_poll     (struct connection *self,
					       fd_set *fd_read);

static void               connection_set_status_led (struct connection *self,
						     int connected);

static void               connection_list_add_fds   (int *fd_max,
						     fd_set *fd_read);
static void               connection_list_poll      (fd_set *fd_read);
static void               connection_list_append    (struct connection *c);
static void               connection_list_remove    (struct connection *c);
static struct connection* connection_list_find_path (const char *path);

static int                event_loop          (void);
static void               hotplug_poll        (void);
static int                hotplug_detect      (int fd);

static void               event_from_server   (struct server *svr,
					       int evdev);
static void               usage               (char *progname);

static void               repack_bits         (unsigned long *src,
					       unsigned char *dest,
					       int len);
static int                led_name_to_number  (unsigned char *name);


/***********************************************************************/
/*************************************************** Server Interface **/
/***********************************************************************/

/* Create a new 'server' object and connect it to the given host and/or port */
static struct server* server_new(const char *host_and_port)
{
  struct server *self;
  char *p;
  struct sockaddr_in in_addr;
  struct hostent* host;
  int opt;

  /* Allocate the new server object */
  self = malloc(sizeof(struct server));
  assert(self != NULL);
  memset(self, 0, sizeof(struct server));

  /* Reset the write buffer */
  self->tcp_buffer.remaining = sizeof(self->tcp_buffer.data);
  self->tcp_buffer.current = self->tcp_buffer.data;

  /* Parse the host:port string */
  self->host = strdup(host_and_port);
  self->port = IPIPE_DEFAULT_PORT;
  p = strchr(self->host, ':');
  if (p) {
    *p = '\0';
    self->port = atoi(p+1);
  }

  /* New socket */
  self->tcp_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (self->tcp_fd < 0) {
    perror("socket");
    server_delete(self);
    return NULL;
  }

  /* Try disabling the "Nagle algorithm" or "tinygram prevention".
   * This will keep our latency low, and we do our own write buffering.
   */
  opt = 1;
  if (setsockopt(self->tcp_fd, 6 /*PROTO_TCP*/, TCP_NODELAY, (void *)&opt, sizeof(opt))) {
    perror("Setting TCP_NODELAY");
    server_delete(self);
    return NULL;
  }

  /* Connect the socket to our parsed address */
  host = gethostbyname(self->host);
  if (!host) {
    fprintf(stderr, "Unknown host '%s'\n", self->host);
    server_delete(self);
    return NULL;
  }
  memset(&in_addr, 0, sizeof(in_addr));
  in_addr.sin_family = AF_INET;
  memcpy(&in_addr.sin_addr.s_addr, host->h_addr_list[0], sizeof(in_addr.sin_addr.s_addr));
  in_addr.sin_port = htons(self->port);
  if (connect(self->tcp_fd, (struct sockaddr*) &in_addr, sizeof(in_addr))) {
    perror("Connecting to inputpipe-server");
    server_delete(self);
    return NULL;
  }

  /* Use nonblocking I/O */
  fcntl(self->tcp_fd, F_SETFL, fcntl(self->tcp_fd, F_GETFL, 0) | O_NONBLOCK);

  /* Buffer reads using a stdio file. We do our own write buffering,
   * as fflush() is a bit finicky on sockets.
   */
  self->tcp_file = fdopen(self->tcp_fd, "r");
  assert(self->tcp_file != NULL);

  return self;
}

static void server_delete(struct server *self)
{
  if (self->host) {
    free(self->host);
  }

  if (self->tcp_file) {
    fclose(self->tcp_file);
  }
  else if (self->tcp_fd) {
    close(self->tcp_fd);
  }

  free(self);
}

static void server_write(struct server *self, int packet_type,
			int length, void *content)
{
  struct inputpipe_packet pkt;
  assert(length < 0x10000);
  assert(length + sizeof(pkt) < sizeof(self->tcp_buffer.data));

  pkt.type = htons(packet_type);
  pkt.length = htons(length);

  if (length + sizeof(pkt) > self->tcp_buffer.remaining)
    server_flush(self);

  self->tcp_buffer.remaining -= length + sizeof(pkt);
  memcpy(self->tcp_buffer.current, &pkt, sizeof(pkt));
  self->tcp_buffer.current += sizeof(pkt);
  memcpy(self->tcp_buffer.current, content, length);
  self->tcp_buffer.current += length;
}

/* Flush our write buffer. This should happen after
 * initialization, and whenever we get a sync event. That
 * will have the effect of combining our protocol's packets
 * into larger TCP packets and ethernet frames. Ideally, one group
 * of updates (for each of the device's modified axes and buttons)
 * will always correspond to one frame in the underlying transport.
 */
static void server_flush(struct server *self)
{
  int size = self->tcp_buffer.current - self->tcp_buffer.data;
  if (size == 0)
    return;
  write(self->tcp_fd, self->tcp_buffer.data, size);
  self->tcp_buffer.remaining = sizeof(self->tcp_buffer.data);
  self->tcp_buffer.current = self->tcp_buffer.data;
}

/* If we can receive a packet, this returns its type and puts its
 * length and content in the provided addresses. If not, returns 0.
 * The caller must free(content) if this function returns nonzero.
 */
static int server_read(struct server *self, int *length, void **content)
{
  while (1) {
    /* Already received a packet header? Try to get the content */
    if (self->received_header) {
      int type = ntohs(self->tcp_packet.type);
      *length = ntohs(self->tcp_packet.length);

      if (*length > 0) {
	/* We have content to receive... */
	*content = malloc(*length);
	assert(*content != NULL);

	if (fread(*content, *length, 1, self->tcp_file)) {
	  /* Yay, got a whole packet to process */
	  self->received_header = 0;
	  return type;
	}

	/* Can't do anything else until we get the content */
	free(*content);
	return 0;
      }
      else {
	/* This packet included no content, we're done */
	self->received_header = 0;
	*content = NULL;
	return type;
      }
    }

    /* See if we can get another header */
    if (fread(&self->tcp_packet, sizeof(self->tcp_packet), 1, self->tcp_file)) {
      /* Yep. Next we'll try to get the content. */
      self->received_header = 1;
    }
    else
      return 0;
  }
}


/***********************************************************************/
/********************************************* Event device interface **/
/***********************************************************************/

static int evdev_new(const char *path)
{
  int fd;
  fd = open(path, O_RDWR | O_NONBLOCK);
  if (fd < 0)
    perror("Opening event device");
  return fd;
}

static void evdev_delete(int evdev)
{
  close(evdev);
}

/* Our bits come from the kernel packed in longs, but for portability
 * on the network we want them packed in bytes. This copies 'len'
 * bit mask bytes, rearranging them as necessary.
 */
static void repack_bits(unsigned long *src, unsigned char *dest, int len) {
  int i;
  unsigned long word;
  while (len >= sizeof(long)) {
    word = *src;
    for (i=0; i<sizeof(long); i++) {
      *(dest++) = word;
      word >>= 8;
    }
    src++;
    len -= sizeof(long);
  }
}

/* Send all metadata from our event device to the server,
 * and create a corresponding device on the server side.
 */
static int evdev_send_metadata(int evdev, struct server *svr)
{
  unsigned char buffer[512];
  unsigned char buffer2[512];
  short id[4];
  struct ipipe_input_id ip_id;
  uint32_t i32;
  int i, axis;

  /* Send the device name */
  buffer[0] = '\0';
  ioctl(evdev, EVIOCGNAME(sizeof(buffer)), buffer);
  buffer[sizeof(buffer)-1] = '\0';
  server_write(svr, IPIPE_DEVICE_NAME, strlen(buffer), buffer);

  /* Send device ID */
  ioctl(evdev, EVIOCGID, id);
  ip_id.bustype = htons(id[0]);
  ip_id.vendor  = htons(id[1]);
  ip_id.product = htons(id[2]);
  ip_id.version = htons(id[3]);
  server_write(svr, IPIPE_DEVICE_ID, sizeof(ip_id), &ip_id);

  /* Send bits */
  for (i=0; i<EV_MAX; i++) {
    /* Read these bits, leaving room for the EV_* code at the beginning */
    int len = ioctl(evdev, EVIOCGBIT(i, sizeof(buffer) - sizeof(uint16_t)), buffer);
    if (len <= 0)
      continue;

    repack_bits((unsigned long*) buffer, buffer2 + sizeof(uint16_t), len);
    *(uint16_t*)buffer2 = htons(i);
    server_write(svr, IPIPE_DEVICE_BITS, len + sizeof(uint16_t), buffer2);

    /* If we just grabbed the EV_ABS bits, look for absolute axes
     * we need to send IPIPE_DEVICE_ABSINFO packets for.
     */
    if (i == EV_ABS) {
      for (axis=0; axis < len*8; axis++) {
	/* This ugly mess tests a bit in our bitfield. We have
	 * to be careful to do this the same way the kernel and
	 * repack_bits do it, to be portable.
	 */
	if (((unsigned long*)buffer)[ axis / (sizeof(long)*8) ] &
	    (1 << (axis % (sizeof(long)*8)))) {

	  /* We found an axis, get and repackage its input_absinfo struct */
	  struct input_absinfo absinfo;
	  struct ipipe_absinfo ip_abs;
	  ioctl(evdev, EVIOCGABS(axis), &absinfo);

	  ip_abs.axis = htonl(axis);
	  ip_abs.max = htonl(absinfo.maximum);
	  ip_abs.min = htonl(absinfo.minimum);
	  ip_abs.fuzz = htonl(absinfo.fuzz);
	  ip_abs.flat = htonl(absinfo.flat);
	  server_write(svr, IPIPE_DEVICE_ABSINFO, sizeof(ip_abs), &ip_abs);
	}
      }
    }
  }

  /* Send the number of maximum concurrent force-feedback effects */
  ioctl(evdev, EVIOCGEFFECTS, &i);
  i32 = htonl(i);
  server_write(svr, IPIPE_DEVICE_FF_EFFECTS_MAX, sizeof(i32), &i32);

  /* Create the device and flush all this to the server */
  server_write(svr, IPIPE_CREATE, 0, NULL);
  server_flush(svr);
  return 0;
}

/* Read the next event from the event device and send to the server. */
static int evdev_send_event(int evdev, struct server *svr)
{
  struct input_event ev;
  struct ipipe_event ip_ev;
  int retval;

  retval = read(evdev, &ev, sizeof(ev));
  if (retval < 0) {
    if (errno == EAGAIN) {
      return 0;
    }
    perror("read from evdev");
    return 1;
  }
  else if (retval == 0) {
    fprintf(stderr, "Event device EOF\n");
    return 1;
  }
  else if (retval < sizeof(ev)) {
    return 0;
  }

  /* Translate and send this event */
  ip_ev.tv_sec = htonl(ev.time.tv_sec);
  ip_ev.tv_usec = htonl(ev.time.tv_usec);
  ip_ev.value = htonl(ev.value);
  ip_ev.type = htons(ev.type);
  ip_ev.code = htons(ev.code);
  server_write(svr, IPIPE_EVENT, sizeof(ip_ev), &ip_ev);

#ifdef EV_SYN
  /* If this was a synchronization event, flush our buffers.
   * This will group together the individual events for each axis and button
   * into one frame in the underlying network transport hopefully.
   */
  if (ev.type == EV_SYN)
    server_flush(svr);

#else
  /* Oh no, we're running on linux 2.4, where there were no sync
   * events! This is going to suck, but we'll have to generate
   * a sync then flush for every event.
   */
  ip_ev.value = 0;
  ip_ev.type = 0;
  ip_ev.code = 0;
  server_write(svr, IPIPE_EVENT, sizeof(ip_ev), &ip_ev);
  server_flush(svr);
#endif

  return 0;
}


/***********************************************************************/
/******************************************************* Connections ***/
/***********************************************************************/

static struct connection* connection_new(const char *evdev_path,
					 const char *host_and_port)
{
  struct connection* self;

  /* Allocate the new server object */
  self = malloc(sizeof(struct connection));
  assert(self != NULL);
  memset(self, 0, sizeof(struct connection));

  self->evdev_path = strdup(evdev_path);
  assert(self->evdev_path != NULL);

  connection_message(self, "Connecting to '%s'...", host_and_port);

  self->evdev_fd = evdev_new(evdev_path);
  if (self->evdev_fd <= 0) {
    connection_delete(self);
    return NULL;
  }

  connection_set_status_led(self, 0);

  self->server = server_new(host_and_port);
  if (!self->server) {
    connection_delete(self);
    return NULL;
  }

  /* Tell the server about our device */
  if (evdev_send_metadata(self->evdev_fd, self->server)) {
    connection_delete(self);
    return NULL;
  }

  connection_list_append(self);
  connection_message(self, "Connected");
  connection_set_status_led(self, 1);

  return self;
}

static void connection_delete(struct connection *self)
{
  connection_message(self, "Disconnected");
  connection_list_remove(self);

  if (self->evdev_fd > 0) {
    connection_set_status_led(self, 0);
    evdev_delete(self->evdev_fd);
  }
  if (self->server)
    server_delete(self->server);
  free(self);
}

static void connection_add_fds(struct connection *self,
			       int *fd_max,
			       fd_set *fd_read)
{
  if (fd_max) {
    if (*fd_max <= self->server->tcp_fd)
      *fd_max = self->server->tcp_fd + 1;
    if (*fd_max <= self->evdev_fd)
      *fd_max = self->evdev_fd + 1;
  }
  if (fd_read) {
    FD_SET(self->server->tcp_fd, fd_read);
    FD_SET(self->evdev_fd, fd_read);
  }
}

static int connection_poll(struct connection *self, fd_set *fd_read)
{
  int retval;

  /* Can we read from the server? */
  if (FD_ISSET(self->server->tcp_fd, fd_read)) {
    if (feof(self->server->tcp_file)) {
      connection_message(self, "Connection lost");
      return 1;
    }
    event_from_server(self->server, self->evdev_fd);
  }

  /* Can we read from the event device?
   * Poll it whether or not we see any read
   * activity, to detect device disconnects.
   */
  retval = evdev_send_event(self->evdev_fd, self->server);
  if (retval) {
    connection_message(self, "Error reading from event device");
    return retval;
  }

  return 0;
}

static void connection_set_status_led (struct connection *self, int connected)
{
  if (config_connect_led_number >= 0) {
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_LED;
    ev.code = config_connect_led_number;

    if (connected)
      ev.value = !!config_connect_led_polarity;
    else
      ev.value = !config_connect_led_polarity;

    write(self->evdev_fd, &ev, sizeof(ev));
  }
}

static void connection_list_append(struct connection *c)
{
  if (c->in_connection_list)
    remove;
  c->in_connection_list = 1;

  if (connection_list_tail) {
    c->prev = connection_list_tail;
    c->prev->next = c;
  }
  else {
    assert(connection_list_head == NULL);
    connection_list_head = c;
  }
  connection_list_tail = c;
}

static void connection_list_remove(struct connection *c)
{
  if (!c->in_connection_list)
    return;
  c->in_connection_list = 0;

  if (c->next) {
    c->next->prev = c->prev;
  }
  else {
    assert(connection_list_tail == c);
    connection_list_tail = c->prev;
  }
  if (c->prev) {
    c->prev->next = c->next;
  }
  else {
    assert(connection_list_head == c);
    connection_list_head = c->next;
  }
}

static void connection_list_add_fds(int *fd_max, fd_set *fd_read)
{
  struct connection *i, *next;
  i = connection_list_head;
  while (i) {
    next = i->next;
    connection_add_fds(i, fd_max, fd_read);
    i = next;
  }
}

static void connection_list_poll(fd_set *fd_read)
{
  struct connection *i, *next;
  i = connection_list_head;
  while (i) {
    next = i->next;
    if (connection_poll(i, fd_read))
      connection_delete(i);
    i = next;
  }
}

static struct connection* connection_list_find_path (const char *path)
{
  struct connection *i;

  for (i=connection_list_head; i; i=i->next) {
    if (!strcmp(i->evdev_path, path))
      return i;
  }
  return NULL;
}


/***********************************************************************/
/******************************************************* Event Loop ****/
/***********************************************************************/

/* Process incoming data from the server */
static void event_from_server(struct server *svr, int evdev)
{
  int length;
  void* content;

  /* Read packets from the server while they're available */
  while (1) {
    switch (server_read(svr, &length, &content)) {

    case 0:
      return;

    default:
      printf("Unknown packet received from server\n");
    }
    free(content);
  }
}

static int event_loop(void) {
  fd_set fd_read;
  int fd_max;
  int n;
  struct timeval timeout, remaining;

  /* The polling interval for detecting device insertion/removal */
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  remaining = timeout;

  while (1) {
    FD_ZERO(&fd_read);
    fd_max = 0;

    connection_list_add_fds(&fd_max, &fd_read);

    n = select(fd_max, &fd_read, NULL, NULL, &remaining);
    if (n<0) {
      perror("select");
      exit(1);
    }
    else if (n == 0) {
      /* Remaining time expired, poll for new devices.
       * This also polls implicitly for disconnections,
       * when we do connection_list_poll()
       */
      remaining = timeout;
      if (config_hotplug_polling)
	hotplug_poll();
    }

    connection_list_poll(&fd_read);

    /* With hotplugging disabled, exit after the last connection is gone */
    if ((!config_hotplug_polling) && (!connection_list_head))
      break;
  }
}

static void hotplug_poll(void)
{
  /* Scan the input path for event devices */
  DIR *dir;
  struct dirent *dent;
  int fd;
  char full_path[PATH_MAX];

  dir = opendir(config_input_path);
  if (!dir) {
    /* No point to continuing if we can't scan the directory */
    perror("Opening input directory");
    exit(1);
  }

  while ((dent = readdir(dir))) {

    /* We only care about event devices */
    if (strncmp(dent->d_name, "event", 5))
      continue;

    /* Construct a full path, we'll need it for the rest */
    strncpy(full_path, config_input_path, sizeof(full_path)-1);
    full_path[sizeof(full_path)-1] = '\0';
    strncat(full_path, "/", sizeof(full_path)-1);
    full_path[sizeof(full_path)-1] = '\0';
    strncat(full_path, dent->d_name, sizeof(full_path)-1);
    full_path[sizeof(full_path)-1] = '\0';

    /* Make sure it isn't a device we already have connected */
    if (connection_list_find_path(full_path))
      continue;

    /* Make sure we can open it- if we can, we'll need to see
     * whether it's a device we're interested in.
     */
    fd = open(full_path, O_RDWR);
    if (fd >= 0) {
      if (hotplug_detect(fd)) {
	close(fd);
	connection_new(full_path, config_host_and_port);
      }
      else {
	close(fd);
      }
    }
  }

  closedir(dir);
}

static int hotplug_detect(int fd)
{
  /* Given an opened event device, return nonzero if it's
   * one we're interested in automatically connecting.
   * This uses the config_detect_* criteria.
   */
  unsigned char evbits[64];
  unsigned char keybits[64];
  unsigned char absbits[64];
  unsigned char relbits[64];

  if (config_detect_all)
    return 1;

  if (ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), evbits) < 0)
    return 0;
  if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybits)), keybits) < 0)
    return 0;
  if (ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relbits)), relbits) < 0)
    return 0;
  if (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absbits)), absbits) < 0)
    return 0;

  if (config_detect_joystick) {
    /* Joysticks need two absolute axes and a joystickesque button */
    if (test_bit(EV_KEY, evbits) &&
	test_bit(EV_ABS, evbits) &&
	test_bit(ABS_X, absbits) &&
	test_bit(ABS_Y, absbits) &&
	(test_bit(BTN_TRIGGER, keybits) ||
	 test_bit(BTN_A, keybits) ||
	 test_bit(BTN_0, keybits) ||
	 test_bit(BTN_1, keybits)))
      return 1;
  }

  if (config_detect_mouse) {
    /* Mice have two relative axes and a mouse-like button */
    if (test_bit(EV_KEY, evbits) &&
	test_bit(EV_REL, evbits) &&
	test_bit(REL_X, relbits) &&
	test_bit(REL_Y, relbits) &&
	test_bit(BTN_MOUSE, keybits))
      return 1;
  }

  if (config_detect_keyboard) {
    /* Keyboards have keys, but no axes */
    if (test_bit(EV_KEY, evbits) &&
	(!test_bit(EV_REL, evbits)) &&
	(!test_bit(EV_ABS, evbits)))
      return 1;
  }

  return 0;
}

static int led_name_to_number(unsigned char *name)
{
  int i;
  struct {
    const char *name;
    int number;
  } table[] = {
    {"NUML",    LED_NUML},
    {"CAPSL",   LED_CAPSL},
    {"SCROLLL", LED_SCROLLL},
    {"COMPOSE", LED_COMPOSE},
    {"KANA",    LED_KANA},
    {"SLEEP",   LED_SLEEP},
    {"SUSPEND", LED_SUSPEND},
    {"MUTE",    LED_MUTE},
    {"MISC",    LED_MISC},
    {NULL,      0},
  };

  for (i=0; table[i].name; i++) {
    if (!strcasecmp(table[i].name, name))
      return table[i].number;
  }

  printf("Unknown LED name '%s'\n", name);
  exit(1);
}

static void usage(char *progname)
{
  fprintf(stderr,
	  "Usage: %s [options] server[:port] [devices...]\n"
	  "\n"
	  "Export any device registered with the Linux input system to\n"
	  "a remote machine running inputpipe-server. Any event devices\n"
	  "(/dev/input/eventN) given on the command line will be sent to\n"
	  "the indicated server. In addition to or instead of the explicit\n"
	  "devices given on the command line, this can automatically detect\n"
	  "categories of devices to send to the server using the --hotplug-*\n"
	  "command line options.\n"
	  "\n"
	  "  -h, --help                     This text\n"
	  "  -q, --quiet                    Suppress normal log output\n"
	  "  -p PATH, --input-path PATH     Set the path to scan for hotpluggable\n"
	  "                                 input devices [%s]\n"
	  "  -l LED, --connect-led LED      Use the given LED on a device, if\n"
	  "                                 present, to indicate server connection.\n"
	  "                                 The LED name is one defined by the kernel\n"
	  "                                 (numl, capsl, scroll, sleep, misc...)\n"
	  "                                 and may be preceeded with a '~' to make\n"
	  "                                 it active-low.\n"
	  "  -j, --hotplug-js               Detect and export joystick devices\n"
	  "  -m, --hotplug-mice             Detect and export mouse devices\n"
	  "  -k, --hotplug-kb               Detect and export keyboard devices\n"
	  "  -a, --hotplug-all              Detect and export all input devices\n"
	  "\n"
	  "WARNING: Do not use --hotplug-kb or --hotplug-all if this system has\n"
	  "         keyboards attached that may be used to input passwords or other\n"
	  "         sensitive information\n",
	  progname, config_input_path);
}

int main(int argc, char **argv)
{
  int c;

  while (1) {
    static struct option long_options[] = {
      {"help",         0, 0, 'h'},
      {"quiet",        0, 0, 'q'},
      {"input-path",   1, 0, 'p'},
      {"hotplug-js",   0, 0, 'j'},
      {"hotplug-mice", 0, 0, 'm'},
      {"hotplug-kb",   0, 0, 'k'},
      {"hotplug-all",  0, 0, 'a'},
      {"connect-led",  1, 0, 'l'},
      {0},
    };

    c = getopt_long(argc, argv, "hqp:jmkal:",
		    long_options, NULL);
    if (c == -1)
      break;
    switch (c) {

    case 'q':
      config_verbose = 0;
      break;

    case 'p':
      config_input_path = optarg;
      break;

    case 'j':
      config_detect_joystick = 1;
      config_hotplug_polling = 1;
      break;

    case 'm':
      config_detect_mouse = 1;
      config_hotplug_polling = 1;
      break;

    case 'k':
      config_detect_keyboard = 1;
      config_hotplug_polling = 1;
      break;

    case 'a':
      config_detect_all = 1;
      config_hotplug_polling = 1;
      break;

    case 'l':
      if (optarg[0] == '~') {
	config_connect_led_number = led_name_to_number(optarg+1);
	config_connect_led_polarity = 0;
      }
      else {
	config_connect_led_number = led_name_to_number(optarg);
	config_connect_led_polarity = 1;
      }
      break;

    case 'h':
    default:
      usage(argv[0]);
      return 1;
    }
  }

  /* We require at least a server name */
  if (!argv[optind]) {
    usage(argv[0]);
    return 1;
  }
  config_host_and_port = argv[optind++];

  /* Let the user know they're doing something silly if we aren't in hotplug
   * polling mode and there aren't any explicitly specified devices.
   */
  if ((!config_hotplug_polling) && !argv[optind]) {
    printf("Nothing to do; give at least one hotplug option or device name\n\n");
    usage(argv[0]);
    return 1;
  }

  /* Open all the explicitly mentioned devices */
  while (argv[optind]) {
    if (!connection_new(argv[optind], config_host_and_port))
      return 1;
    optind++;
  }

  return event_loop();
}

/* The End */
