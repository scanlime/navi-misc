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

void                   ntoh_ff_envelope     (struct ff_envelope*       host,
					     struct ipipe_ff_envelope* net)
{
  host->attack_length = ntohs(net->attack_length);
  host->attack_level = ntohs(net->attack_level);
  host->fade_length = ntohs(net->fade_length);
  host->fade_level = ntohs(net->fade_level);
}

void                   hton_ff_envelope     (struct ipipe_ff_envelope* net,
					     struct ff_envelope*       host)
{
  net->attack_length = htons(host->attack_length);
  net->attack_level = htons(host->attack_level);
  net->fade_length = htons(host->fade_length);
  net->fade_level = htons(host->fade_level);
}

void                   ntoh_ff_effect       (struct ff_effect*         host,
					     struct ipipe_ff_effect*   net)
{
  int i;

  host->type = ntohs(net->type);
  host->id = ntohs(net->id);
  host->direction = ntohs(net->direction);
  host->trigger.button = ntohs(net->trigger_button);
  host->trigger.interval = ntohs(net->trigger_interval);
  host->replay.length = ntohs(net->replay_length);
  host->replay.delay = ntohs(net->replay_delay);

  switch (host->type) {

  case FF_CONSTANT:
    host->u.constant.level = ntohs(net->u.constant.level);
    ntoh_ff_envelope(&host->u.constant.envelope, &net->u.constant.envelope);
    break;

  case FF_RAMP:
    host->u.ramp.start_level = ntohs(net->u.ramp.start_level);
    host->u.ramp.end_level = ntohs(net->u.ramp.end_level);
    ntoh_ff_envelope(&host->u.ramp.envelope, &net->u.ramp.envelope);
    break;

  case FF_PERIODIC:
    host->u.periodic.waveform = ntohs(net->u.periodic.waveform);
    host->u.periodic.period = ntohs(net->u.periodic.period);
    host->u.periodic.magnitude = ntohs(net->u.periodic.magnitude);
    host->u.periodic.offset = ntohs(net->u.periodic.offset);
    host->u.periodic.phase = ntohs(net->u.periodic.phase);
    ntoh_ff_envelope(&host->u.periodic.envelope, &net->u.periodic.envelope);
    break;


  case FF_SPRING:
  case FF_FRICTION:
    for (i=0; i<2; i++) {
      host->u.condition[i].right_saturation = ntohs(net->u.condition[i].right_saturation);
      host->u.condition[i].left_saturation = ntohs(net->u.condition[i].left_saturation);
      host->u.condition[i].right_coeff = ntohs(net->u.condition[i].right_coeff);
      host->u.condition[i].left_coeff = ntohs(net->u.condition[i].left_coeff);
      host->u.condition[i].deadband = ntohs(net->u.condition[i].deadband);
      host->u.condition[i].center = ntohs(net->u.condition[i].center);
    }
    break;

  case FF_RUMBLE:
    host->u.rumble.strong_magnitude = ntohs(net->u.rumble.strong_magnitude);
    host->u.rumble.weak_magnitude = ntohs(net->u.rumble.weak_magnitude);
    break;
  }
}

void                   hton_ff_effect       (struct ipipe_ff_effect*   net,
					     struct ff_effect*         host)
{
  int i;

  net->type = htons(host->type);
  net->id = htons(host->id);
  net->direction = htons(host->direction);
  net->trigger_button = htons(host->trigger.button);
  net->trigger_interval = htons(host->trigger.interval);
  net->replay_length = htons(host->replay.length);
  net->replay_delay = htons(host->replay.delay);

  switch (host->type) {

  case FF_CONSTANT:
    net->u.constant.level = htons(host->u.constant.level);
    hton_ff_envelope(&net->u.constant.envelope, &host->u.constant.envelope);
    break;

  case FF_RAMP:
    net->u.ramp.start_level = htons(host->u.ramp.start_level);
    net->u.ramp.end_level = htons(host->u.ramp.end_level);
    hton_ff_envelope(&net->u.ramp.envelope, &host->u.ramp.envelope);
    break;

  case FF_PERIODIC:
    net->u.periodic.waveform = htons(host->u.periodic.waveform);
    net->u.periodic.period = htons(host->u.periodic.period);
    net->u.periodic.magnitude = htons(host->u.periodic.magnitude);
    net->u.periodic.offset = htons(host->u.periodic.offset);
    net->u.periodic.phase = htons(host->u.periodic.phase);
    hton_ff_envelope(&net->u.periodic.envelope, &host->u.periodic.envelope);
    break;


  case FF_SPRING:
  case FF_FRICTION:
    for (i=0; i<2; i++) {
      net->u.condition[i].right_saturation = htons(host->u.condition[i].right_saturation);
      net->u.condition[i].left_saturation = htons(host->u.condition[i].left_saturation);
      net->u.condition[i].right_coeff = htons(host->u.condition[i].right_coeff);
      net->u.condition[i].left_coeff = htons(host->u.condition[i].left_coeff);
      net->u.condition[i].deadband = htons(host->u.condition[i].deadband);
      net->u.condition[i].center = htons(host->u.condition[i].center);
    }
    break;

  case FF_RUMBLE:
    net->u.rumble.strong_magnitude = htons(host->u.rumble.strong_magnitude);
    net->u.rumble.weak_magnitude = htons(host->u.rumble.weak_magnitude);
    break;
  }
}

void                   ntoh_input_event     (struct input_event*       host,
					     struct ipipe_event*       net)
{
  host->time.tv_sec = ntohl(net->tv_sec);
  host->time.tv_usec = ntohl(net->tv_usec);
  host->type = ntohs(net->type);
  host->code = ntohs(net->code);
  host->value = ntohl(net->value);
}

void                   hton_input_event     (struct ipipe_event*       net,
					     struct input_event*       host)
{
  net->tv_sec = htonl(host->time.tv_sec);
  net->tv_usec = htonl(host->time.tv_usec);
  net->value = htonl(host->value);
  net->type = htons(host->type);
  net->code = htons(host->code);
}

void                   ntoh_input_id        (struct input_id*          host,
					     struct ipipe_input_id*    net)
{
  host->bustype = ntohs(net->bustype);
  host->vendor = ntohs(net->vendor);
  host->product = ntohs(net->product);
  host->version = ntohs(net->version);
}

void                   hton_input_id        (struct ipipe_input_id*    net,
					     short                     host[4])
{
  net->bustype = htons(host[0]);
  net->vendor  = htons(host[1]);
  net->product = htons(host[2]);
  net->version = htons(host[3]);
}

void                   hton_input_absinfo   (struct ipipe_absinfo*     net,
					     struct input_absinfo*     host,
					     int                       axis)
{
  net->axis = htonl(axis);
  net->max = htonl(host->maximum);
  net->min = htonl(host->minimum);
  net->fuzz = htonl(host->fuzz);
  net->flat = htonl(host->flat);
}

/* The End */
