/*
 * cubed.c - Gamecube daemon. This communicates with the gchub using libusb,
 *           and creates input devices corresponding with all connected
 *           controllers.
 *
 *           Requires at least libusb 0.1.9. This currently acts as an
 *           inputpipe client, rather than creating the devices using
 *           /dev/uinput or another method. Specify the inputpipe server
 *           to use on the command line. The default is localhost, on
 *           the default inputpipe port.
 *
 * The Gamecube Controller Hub project
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <usb.h>
#include <string.h>
#include <gchub_protocol.h>

typedef int bool;

typedef struct {
  int buttons;
  unsigned char joystick[2];
  unsigned char cstick[2];
  unsigned char triggers[2];
} controller_status;

typedef struct {
  /* The name of this controller, as seen by applications */
  const char* name;

  /* Nonzero if the controller is connected */
  bool attached;

  /* Status received from the hardware */
  controller_status last_sent;
  controller_status current;
  controller_status calibrated;

  /* Status we send to the hardware */
  bool rumble;
  bool red_led, green_led;

  /* The socket this controller is connected to input-pipe server with,
   * or zero if no socket connection currently exists.
   */
  int socket_fd;

  int sync_count;

} controller;

usb_dev_handle *gchub_open             (void);
void            gchub_write_status     (usb_dev_handle* self,
					controller*     ports);
void            gchub_read_status      (usb_dev_handle* self,
					controller*     ports);

void            controller_init        (controller*     self,
				        int             port_number);
void            controller_sync        (controller*     self);
void            controller_connect     (controller*     self);
void            controller_disconnect  (controller*     self);
void            controller_send_status (controller*     self);
void            controller_update_leds (controller*     self);


/***********************************************************************/
/**************************************************************** Hub **/
/***********************************************************************/

usb_dev_handle *gchub_open(void)
{
  struct usb_bus *busses;
  struct usb_bus *bus;
  struct usb_device *dev;

  /* Find the device */
  usb_init();
  usb_find_busses();
  usb_find_devices();
  busses = usb_get_busses();

  for (bus=busses; bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {
      if (dev->descriptor.idVendor == GCHUB_VENDOR_ID &&
	  dev->descriptor.idProduct == GCHUB_PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

/* Sends one status packet updating all controller ports */
void gchub_write_status(usb_dev_handle *self, controller* ports)
{
  int rumble=0, leds=0;

  if (ports[0].rumble) rumble |= GCHUB_RUMBLE_PORT0;
  if (ports[1].rumble) rumble |= GCHUB_RUMBLE_PORT1;
  if (ports[2].rumble) rumble |= GCHUB_RUMBLE_PORT2;
  if (ports[3].rumble) rumble |= GCHUB_RUMBLE_PORT3;

  if (ports[0].red_led) leds |= GCHUB_LED_PORT0_RED;
  if (ports[1].red_led) leds |= GCHUB_LED_PORT1_RED;
  if (ports[2].red_led) leds |= GCHUB_LED_PORT2_RED;
  if (ports[3].red_led) leds |= GCHUB_LED_PORT3_RED;

  if (ports[0].green_led) leds |= GCHUB_LED_PORT0_GREEN;
  if (ports[1].green_led) leds |= GCHUB_LED_PORT1_GREEN;
  if (ports[2].green_led) leds |= GCHUB_LED_PORT2_GREEN;
  if (ports[3].green_led) leds |= GCHUB_LED_PORT3_GREEN;

  if (usb_control_msg(self, USB_TYPE_VENDOR, GCHUB_CTRL_SET_STATUS,
		      rumble, leds, NULL, 0, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
}

/* Read one interrupt packet and act upon its contents */
void gchub_read_status(usb_dev_handle *self, controller* ports)
{
  unsigned char buffer[8];
  int result;
  controller *port;
  bool sync;
  int packet_type;

  result = usb_interrupt_read(self, 1, buffer, sizeof(buffer), 500);
  if (result < 0) {
    perror("usb_interrupt_read");
    exit(1);
  }
  else if (result != sizeof(buffer)) {
    printf("short read: %d bytes\n", result);
    exit(1);
  }

  /* The first byte of the packet holds the controller number,
   * packet type, and sync bit.
   */
  port = &ports[buffer[0] & 0x03];
  sync = buffer[0] & GCHUB_SYNC;
  packet_type = buffer[0] & 0x70;

  switch (packet_type) {

  case GCHUB_PACKET_DISCONNECT:
    port->attached = 0;
    break;

  case GCHUB_PACKET_ANALOG:
    port->attached = 1;
    port->current.joystick[0] = buffer[1];
    port->current.joystick[1] = buffer[2];
    port->current.cstick[0]   = buffer[3];
    port->current.cstick[1]   = buffer[4];
    port->current.triggers[0] = buffer[5];
    port->current.triggers[1] = buffer[6];
    break;

  case GCHUB_PACKET_BUTTONS:
    port->attached = 1;
    port->current.buttons = (buffer[1] << 8) | buffer[2];
    break;
  }

  if (sync)
    controller_sync(port);
}


/***********************************************************************/
/********************************************************* Controller **/
/***********************************************************************/

void controller_init(controller *self, int port_number)
{
  char buf[40];

  memset(self, 0, sizeof(*self));

  /* Set our name */
  sprintf(buf, "Gamecube Controller %d", port_number+1);
  self->name = strdup(buf);
}

void controller_sync(controller *self)
{
  if (self->attached && !self->socket_fd)
    controller_connect(self);

  if (self->socket_fd && !self->attached)
    controller_disconnect(self);

  if (self->attached)
    controller_send_status(self);

  controller_update_leds(self);

  self->sync_count += 1;
  if (!(self->sync_count & 0x0F))
    printf("%d\n", self->sync_count);
}

void controller_connect(controller* self)
{
}

void controller_disconnect(controller* self)
{
}

void controller_send_status(controller* self)
{
}

void controller_update_leds(controller* self)
{
}


/***********************************************************************/
/*********************************************************** Frontend **/
/***********************************************************************/

int main(int argc, char **argv) {
  usb_dev_handle *gchub;
  int i;
  controller ports[4];

  for (i=0; i<4; i++)
    controller_init(&ports[i], i);

  if (!(gchub = gchub_open())) {
    printf("Error opening device\n");
    return 1;
  }

  while (1) {
    /* Push writes through less often than reads-
     * it only takes one write to update everything, but we
     * could need up to 8 reads to update all controllers.
     * FIXME: make this smarter and stuff, test our latency and polling rate
     */
    gchub_write_status(gchub, ports);
    for (i=0; i<10; i++)
      gchub_read_status(gchub, ports);
  }

  return 0;
}

/* The End */
