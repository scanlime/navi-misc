/*
 * packet.h - Shared communications code, for assembling and disassembling
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

#ifndef __H_PACKET
#define __H_PACKET

#include "inputpipe.h"
#include <stdio.h>

struct packet_socket {
  int fd;
  FILE *file;

  struct {
    unsigned char data[4096];
    int remaining;
    unsigned char *current;
  } buffer;

  /* Header and flag used for tracking receive state. stdio
   * buffering will ensure we don't get a partial header or
   * content, but we need to be able to receive a header but
   * wait before getting content.
   */
  struct inputpipe_packet packet;
  int received_header;
};

/* Create a packet_socket wrapping a file descriptor */
struct packet_socket*  packet_socket_new    (int                   fd);
void                   packet_socket_delete (struct packet_socket* self);

/* Assemble and buffer a new packet */
void                   packet_socket_write  (struct packet_socket* self,
					     int                   packet_type,
					     int                   length,
					     void*                 content);

/* Flush our write buffer. This should happen after
 * initialization, and whenever we get a sync event. That
 * will have the effect of combining our protocol's packets
 * into larger TCP packets and ethernet frames. Ideally, one group
 * of updates (for each of the device's modified axes and buttons)
 * will always correspond to one frame in the underlying transport.
 */
void                   packet_socket_flush  (struct packet_socket* self);

/* If we can receive a packet, this returns its type and puts its
 * length and content in the provided addresses. If not, returns 0.
 * The caller must free(content) if this function returns nonzero.
 */
int                    packet_socket_read   (struct packet_socket* self,
					     int*                  length,
					     void**                content);

#endif /* __H_PACKET */

/* The End */
