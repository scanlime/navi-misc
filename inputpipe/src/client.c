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
#include <linux/tcp.h>
#include <netdb.h>

#include <linux/input.h>
#include "inputpipe.h"

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

static struct server* server_new          (const char *host_and_port);
static void           server_delete       (struct server *self);

static void           server_write        (struct server *self,
				           int packet_type,
				           int length,
				           void *content);
static int            server_read         (struct server *self,
				           int *length,
				           void **content);
static void           server_flush        (struct server *self);

static int            evdev_new           (const char *path);
static int            evdev_send_metadata (int evdev, struct server *svr);
static int            evdev_send_event    (int evdev, struct server *svr);

static int            event_loop          (struct server *svr,
					   int evdev);
static void           event_from_server   (struct server *svr,
					   int evdev);
static void           repack_bits         (unsigned long *src,
					   unsigned char *dest,
					   int len);


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

  read(evdev, &ev, sizeof(ev));

  /* Translate and send this event */
  ip_ev.tv_sec = htonl(ev.time.tv_sec);
  ip_ev.tv_usec = htonl(ev.time.tv_usec);
  ip_ev.value = htonl(ev.value);
  ip_ev.type = htons(ev.type);
  ip_ev.code = htons(ev.code);
  server_write(svr, IPIPE_EVENT, sizeof(ip_ev), &ip_ev);

  /* If this was a synchronization event, flush our buffers.
   * This will group together the individual events for each axis and button
   * into one frame in the underlying network transport hopefully.
   */
  if (ev.type == EV_SYN)
    server_flush(svr);

  return 0;
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

/* Listen for input from the server and from the event device,
 * forwarding the received data from one to the other.
 */
static int event_loop(struct server *svr, int evdev) {
  fd_set fd_read;
  int fd_count;
  int n;

  fd_count = svr->tcp_fd + 1;
  if (evdev >= fd_count)
    fd_count = evdev + 1;

  while (1) {
    FD_ZERO(&fd_read);
    FD_SET(svr->tcp_fd, &fd_read);
    FD_SET(evdev, &fd_read);

    n = select(fd_count, &fd_read, NULL, NULL, NULL);
    if (n<0) {
      perror("select");
      return 1;
    }
    else if (n>0) {

      /* Can we read from the server? */
      if (FD_ISSET(svr->tcp_fd, &fd_read)) {
	if (feof(svr->tcp_file)) {
	  fprintf(stderr, "Connection lost\n");
	  return 1;
	}
	event_from_server(svr, evdev);
      }

      /* Can we read from the event device? */
      if (FD_ISSET(evdev, &fd_read)) {
	n = evdev_send_event(evdev, svr);
	if (n)
	  return n;
      }

    }
  }
  return 0;
}


int main(int argc, char **argv) {
  struct server *svr;
  int evdev;

  if (argc != 3 || argv[1][0]=='-' || argv[2][0]=='-') {
    printf("Usage: %s server[:port] /dev/input/eventN\n\n"
	   "Export any device registered with the Linux input system to\n"
	   "a remote machine running inputpipe-server.\n",
	   argv[0]);
    return 1;
  }

  svr = server_new(argv[1]);
  if (!svr)
    return 1;

  evdev = evdev_new(argv[2]);
  if (evdev < 0)
    return 1;

  /* Tell the server about our device, then just start forwarding data */
  if (evdev_send_metadata(evdev, svr))
    return 1;
  return event_loop(svr, evdev);
}

/* The End */
