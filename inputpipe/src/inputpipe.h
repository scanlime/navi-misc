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

#define IPIPE_DEFAULT_PORT    7192

/* Every input event or configuration request is packaged into
 * a packet. The header simply identifies the type of packet and
 * its size. Unknown packets types are typically ignored.
 */
struct inputpipe_packet {
  uint16_t type;
  uint16_t length;
};

/* Packets that can be sent after a device is created. Input
 * events can be sent in either direction between client and server,
 * but most devices only generate events, not accept them. Sending
 * events to devices is required to set LED status, or control
 * force-feedback effects.
 */
#define IPIPE_EVENT                  0x0101    /* struct ipipe_event */

/* Packets to set device characteristics before one is created.
 * All of these are optional, but the device name is recommended.
 * These must be sent from client to server before IPIPE_CREATE.
 */
#define IPIPE_DEVICE_NAME            0x0201    /* string */
#define IPIPE_DEVICE_ID              0x0202    /* struct ipipe_input_id */
#define IPIPE_DEVICE_FF_EFFECTS_MAX  0x0203    /* uint32_t */
#define IPIPE_DEVICE_ABSINFO         0x0204    /* struct ipipe_absinfo */
#define IPIPE_DEVICE_BITS            0x0205    /* uint16_t + bit map */

/* After all the IPIPE_DEVICE_* packets you wish to send,
 * this actually creates a new input device on the server machine.
 * Always sent from client to server.
 */
#define IPIPE_CREATE                 0x0301    /* none */

/* These packets allow force-feedback over inputpipe. Effects are started
 * and stopped using normal input events, but the input subsystem defines
 * upload_effect and erase_effect callbacks that the device implements
 * to set up effects.
 *
 * The server defines versions of these callbacks that send a corresponding
 * packet to the client, with a unique request ID, then blocks until it
 * receives a return value packet with a corresponding ID.
 *
 * IPIPE_UPLOAD_EFFECT and IPIPE_ERASE_EFFECT always flow from server to
 * client, IPIPE_RETURN_INT is sent from client to server in response.
 * Note that if the client does not set the EV_FF bit, these packets will
 * never be sent. If the EV_FF bit is set, however, the client must implement
 * these packets or any apps trying to use force feedback will hang while
 * uploading effects.
 */
#define IPIPE_UPLOAD_EFFECT          0x0401    /* struct ipipe_upload_effect */
#define IPIPE_ERASE_EFFECT           0x0402    /* struct ipipe_erase_effect */
#define IPIPE_RETURN_INT             0x0403    /* struct ipipe_return_int */


struct ipipe_event {
  uint32_t tv_sec;
  uint32_t tv_usec;
  int32_t  value;
  uint16_t type;
  uint16_t code;
};

struct ipipe_input_id {
  uint16_t bustype;
  uint16_t vendor;
  uint16_t product;
  uint16_t version;
};

struct ipipe_absinfo {
  uint32_t axis;
  int32_t max;
  int32_t min;
  int32_t fuzz;
  int32_t flat;
};

struct ipipe_upload_effect {
  uint32_t request_id;

  /* ff_effect */
  uint16_t effect_type;
  int16_t effect_id;
  uint16_t effect_direction;

  /* ff_trigger */
  uint16_t trigger_button;
  uint16_t trigger_interval;

  /* ff_replay */
  uint16_t replay_length;
  uint16_t replay_delay;

};

struct ipipe_erase_effect {
  uint32_t request_id;
  int16_t effect_id;
};

struct ipipe_return_int {
  uint32_t request_id;
  int32_t retval;
};

#endif /* __H_INPUTPIPE */

/* The End */
