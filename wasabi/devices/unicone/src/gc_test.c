/*
 * Simple demonstration of gamecube controller emulation.
 *
 * This opens an event device specified on the command line,
 * translates to a gamecube controller status packet using
 * a mapping appropriate for the gchub, then sends the
 * mapped controller status on to the Unicone.
 */

#include <usb.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

#include <unicone.h>
#include "device.h"

struct gc_status {
  int btn_start;
  int btn_y;
  int btn_x;
  int btn_b;
  int btn_a;
  int btn_l;
  int btn_r;
  int btn_z;
  int abs_dpad_x;
  int abs_dpad_y;
  int abs_joystick_x;   /* Clamped to [-128, 127] */
  int abs_joystick_y;   /* Clamped to [-128, 127] */
  int abs_cstick_x;     /* Clamped to [-128, 127] */
  int abs_cstick_y;     /* Clamped to [-128, 127] */
  int abs_l;            /* Clamped to [0,255]     */
  int abs_r;            /* Clamped to [0,255]     */
};

int clamp(int x, int lower, int upper)
{
  if (x < lower)
    return lower;
  if (x > upper)
    return upper;
  return x;
}

void gc_send_update(struct unicone_device* dev, int controller_num,
		    struct gc_status *status)
{
  /* Pack these into an 8-byte packet that gets sent as-is to the gamecube */
  unsigned char packet[8];
  int i2c_address = 0x40 + controller_num;

  packet[0] = 0x00;
  if (status->btn_start)     packet[0] |= 0x10;
  if (status->btn_y)         packet[0] |= 0x08;
  if (status->btn_x)         packet[0] |= 0x04;
  if (status->btn_b)         packet[0] |= 0x02;
  if (status->btn_a)         packet[0] |= 0x01;

  packet[1] = 0x80;
  if (status->btn_l)         packet[1] |= 0x40;
  if (status->btn_r)         packet[1] |= 0x20;
  if (status->btn_z)         packet[1] |= 0x10;
  if (status->abs_dpad_y>0)  packet[1] |= 0x08;
  if (status->abs_dpad_y<0)  packet[1] |= 0x04;
  if (status->abs_dpad_x>0)  packet[1] |= 0x02;
  if (status->abs_dpad_x<0)  packet[1] |= 0x01;

  packet[2] = clamp(status->abs_joystick_x, -128, 127) + 0x80;
  packet[3] = clamp(status->abs_joystick_y, -128, 127) + 0x80;
  packet[4] = clamp(status->abs_cstick_x,   -128, 127) + 0x80;
  packet[5] = clamp(status->abs_cstick_y,   -128, 127) + 0x80;
  packet[6] = clamp(status->abs_l,           0,   255);
  packet[7] = clamp(status->abs_r,           0,   255);

  printf("%02X %02X %02X %02X %02X %02X %02X %02X\n",
	 packet[0], packet[1], packet[2], packet[3],
	 packet[4], packet[5], packet[6], packet[7]);

  if (unicone_device_i2c_write(dev, i2c_address, packet, sizeof(packet)) < 0) {
    perror("unicone_device_i2c_write");
    exit(1);
  }
}

void translate_event(struct input_event *ev, struct gc_status *status)
{
  switch (ev->type) {

  case EV_KEY:
    switch (ev->code) {
    case BTN_A:      status->btn_a     = ev->value; break;
    case BTN_B:      status->btn_b     = ev->value; break;
    case BTN_X:      status->btn_x     = ev->value; break;
    case BTN_Y:      status->btn_y     = ev->value; break;
    case BTN_Z:      status->btn_z     = ev->value; break;
    case BTN_TL:     status->btn_l     = ev->value; break;
    case BTN_TR:     status->btn_r     = ev->value; break;
    case BTN_START:  status->btn_start = ev->value; break;
    }
    break;

  case EV_ABS:
    /* FIXME: scale axes */
    switch (ev->code) {
    case ABS_X:      status->abs_joystick_x =  ev->value; break;
    case ABS_Y:      status->abs_joystick_y = -ev->value; break;
    case ABS_RX:     status->abs_cstick_x   =  ev->value; break;
    case ABS_RY:     status->abs_cstick_y   = -ev->value; break;
    case ABS_HAT0X:  status->abs_dpad_x     =  ev->value; break;
    case ABS_HAT0Y:  status->abs_dpad_y     = -ev->value; break;
    case ABS_Z:      status->abs_l          =  ev->value; break;
    case ABS_RZ:     status->abs_r          =  ev->value; break;
    }
  }
}

void event_loop(struct unicone_device *dev, int evdev)
{
  struct gc_status status;
  struct input_event ev;
  memset(&status, 0, sizeof(status));

  while (read(evdev, &ev, sizeof(ev)) > 0) {
    if (ev.type == EV_SYN)
      gc_send_update(dev, 0, &status);
    else
      translate_event(&ev, &status);
  }
}

int main(int argc, char **argv)
{
  struct unicone_device* dev;
  int evdev;

  if (!argv[1]) {
    printf("Specify an event device on the command line\n");
    return 1;
  }
  evdev = open(argv[1], O_RDONLY);
  if (evdev < 0) {
    perror("Can't open event device");
    return 1;
  }

  unicone_usb_init();
  dev = unicone_device_new();
  if (!dev)
    return 1;
  if (!(dev->fpga_configured && dev->firmware_installed)) {
    printf("The Unicone device must have firmware and FPGA configuration installed");
    return 1;
  }

  event_loop(dev, evdev);

  return 0;
}

/* The End */
