/*
 * inputpipe.h - This header defines the input pipe protocol,
 *               used for sending events and device metadata between
 *               the client and server. This closely follows the kernel's
 *               event structures, but doesn't refer to them to help increase
 *               portability between kernel versions. All structures in this
 *               file are in network byte order.
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

#ifndef __H_INPUTPIPE
#define __H_INPUTPIPE

#include <netinet/in.h>

/* Every input event or configuration request is packaged into
 * a packet. The header simply identifies the type of packet and
 * its size. Unknown packets types are typically ignored.
 */
struct inputpipe_packet {
  uint16_t type;
  uint16_t length;
};

#define IPIPE_MAX_PACKET_SIZE 32

/* Packets that can be sent after a device is created */
#define IPIPE_EVENT                  0x0101    /* struct ipipe_event */

/* Packets to set device characteristics before one is created.
 * All of these are optional, but the device name is recommended.
 */
#define IPIPE_DEVICE_NAME            0x0201    /* string */
#define IPIPE_DEVICE_ID              0x0202    /* struct ipipe_input_id */
#define IPIPE_DEVICE_FF_EFFECTS_MAX  0x0203    /* uint32_t */
#define IPIPE_DEVICE_ABSINFO         0x0204    /* struct ipipe_absinfo */
#define IPIPE_DEVICE_SET_EVBIT       0x0205    /* uint32_t */
#define IPIPE_DEVICE_SET_KEYBIT      0x0206    /* uint32_t */
#define IPIPE_DEVICE_SET_RELBIT      0x0207    /* uint32_t */
#define IPIPE_DEVICE_SET_ABSBIT      0x0208    /* uint32_t */
#define IPIPE_DEVICE_SET_MSCBIT      0x0209    /* uint32_t */
#define IPIPE_DEVICE_SET_LEDBIT      0x020A    /* uint32_t */
#define IPIPE_DEVICE_SET_SNDBIT      0x020B    /* uint32_t */
#define IPIPE_DEVICE_SET_FFBIT       0x020C    /* uint32_t */

/* After all the IPIPE_DEVICE_* packets you wish to send,
 * this actually creates a new input device on the server machine.
 */
#define IPIPE_CREATE                 0x0301    /* none */


struct ipipe_event {
  uint32_t tv_secs;
  uint32_t tv_usecs;
  int32_t  value;
  uint16_t type;
  uint16_t code;
};

struct ipipe_input_id {
  uint32_t bus_type;
  uint32_t vendor;
  uint32_t product;
  uint32_t version;
};

struct ipipe_absinfo {
  uint32_t axis;
  int32_t max;
  int32_t min;
  int32_t fuzz;
  int32_t flat;
};

#endif /* __H_INPUTPIPE */

/* The End */
