/*
 * packet.c - Shared communications code, for assembling and disassembling
 *            the command packets used by the inputpipe protocol.
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
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/select.h>
#include <linux/tcp.h>
#include <errno.h>

#include "packet.h"

struct packet_socket*    packet_socket_new    (int                   fd)
{
  struct packet_socket *self;
  int opt;

  /* Try disabling the "Nagle algorithm" or "tinygram prevention".
   * This will keep our latency low, and we do our own write buffering.
   */
  opt = 1;
  setsockopt(fd, 6 /*PROTO_TCP*/, TCP_NODELAY, (void *)&opt, sizeof(opt));

  /* Use nonblocking I/O */
  if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) < 0) {
    perror("Setting O_NONBLOCK");
  }

  self = malloc(sizeof(struct packet_socket));
  assert(self);
  memset(self, 0, sizeof(struct packet_socket));

  self->fd = fd;
  self->file = fdopen(fd, "rb");
  assert(self->file);

  /* Reset the write buffer */
  self->buffer.remaining = sizeof(self->buffer.data);
  self->buffer.current = self->buffer.data;

  return self;
}

void                     packet_socket_delete (struct packet_socket* self)
{
  if (self->file)
    fclose(self->file);
  else if (self->fd > 0)
    close(self->fd);
  free(self);
}

void                     packet_socket_write  (struct packet_socket* self,
					       int                   packet_type,
					       int                   length,
					       void*                 content)
{
  struct inputpipe_packet pkt;
  assert(length < 0x10000);
  assert(length + sizeof(pkt) < sizeof(self->buffer.data));

  pkt.type = htons(packet_type);
  pkt.length = htons(length);

  if (length + sizeof(pkt) > self->buffer.remaining)
    packet_socket_flush(self);

  self->buffer.remaining -= length + sizeof(pkt);
  memcpy(self->buffer.current, &pkt, sizeof(pkt));
  self->buffer.current += sizeof(pkt);
  memcpy(self->buffer.current, content, length);
  self->buffer.current += length;
}

void                     packet_socket_flush  (struct packet_socket* self)
{
  int size = self->buffer.current - self->buffer.data;
  if (size == 0)
    return;
  write(self->fd, self->buffer.data, size);
  self->buffer.remaining = sizeof(self->buffer.data);
  self->buffer.current = self->buffer.data;
}

int                      packet_socket_read   (struct packet_socket* self,
					       int*                  length,
					       void**                content)
{
  while (1) {
    /* Already received a packet header? Try to get the content */
    if (self->received_header) {
      int type = ntohs(self->packet.type);
      *length = ntohs(self->packet.length);

      if (*length > 0) {
	/* We have content to receive... */
	*content = malloc(*length);
	assert(*content != NULL);

	if (fread(*content, *length, 1, self->file)) {
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
    if (fread(&self->packet, sizeof(self->packet), 1, self->file)) {
      /* Yep. Next we'll try to get the content. */
      self->received_header = 1;
    }
    else
      return 0;
  }
}

/* The End */
