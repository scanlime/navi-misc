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
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#include <unicone.h>
#include "device.h"
#include "progress.h"

#ifndef EV_SYN
#define EV_SYN EV_RST
#endif

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

/* Encode one controller's gc_status struct as an 8-byte raw packet */
void gc_encode_status(struct gc_status *status, unsigned char *encoded)
{
  encoded[0] = 0x00;
  if (status->btn_start)     encoded[0] |= 0x10;
  if (status->btn_y)         encoded[0] |= 0x08;
  if (status->btn_x)         encoded[0] |= 0x04;
  if (status->btn_b)         encoded[0] |= 0x02;
  if (status->btn_a)         encoded[0] |= 0x01;

  encoded[1] = 0x80;
  if (status->btn_l)         encoded[1] |= 0x40;
  if (status->btn_r)         encoded[1] |= 0x20;
  if (status->btn_z)         encoded[1] |= 0x10;
  if (status->abs_dpad_y>0)  encoded[1] |= 0x08;
  if (status->abs_dpad_y<0)  encoded[1] |= 0x04;
  if (status->abs_dpad_x>0)  encoded[1] |= 0x02;
  if (status->abs_dpad_x<0)  encoded[1] |= 0x01;

  encoded[2] = clamp(status->abs_joystick_x, -128, 127) + 0x80;
  encoded[3] = clamp(status->abs_joystick_y, -128, 127) + 0x80;
  encoded[4] = clamp(status->abs_cstick_x,   -128, 127) + 0x80;
  encoded[5] = clamp(status->abs_cstick_y,   -128, 127) + 0x80;
  encoded[6] = clamp(status->abs_l,           0,   255);
  encoded[7] = clamp(status->abs_r,           0,   255);
}

/* Given gc_status structs for each controller, encode and combine
 * them, sending off one big I2C write with all controller data.
 */
void gc_send_update(struct unicone_device* dev, struct gc_status *controllers)
{
  unsigned char packet[32];

  gc_encode_status(&controllers[0], &packet[0]);
  gc_encode_status(&controllers[1], &packet[8]);
  gc_encode_status(&controllers[2], &packet[16]);
  gc_encode_status(&controllers[3], &packet[24]);

  if (unicone_device_i2c_write(dev, 0x40, packet, sizeof(packet)) < 0) {
    perror("unicone_device_i2c_write");
    exit(1);
  }
}

/* Translate input events into modifications to a gc_status struct */
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

/* Given file handles of open event devices for each controller port,
 * read events from those devices and forward them to the gamecube.
 * evdevs with values <= 0 are not connected.
 */
void event_loop(struct unicone_device *dev, int evdevs[4])
{
  struct gc_status controllers[4];
  struct input_event ev;
  memset(&controllers, 0, sizeof(controllers));
  fd_set rfds;
  int i;
  int fd_limit;
  int need_update;

  while (1) {

    /* Prepare rfds and fd_limit for all attached controllers */
    fd_limit = 0;
    FD_ZERO(&rfds);
    for (i=0; i<4; i++) {
      if (evdevs[i] <= 0)
	continue;
      if (evdevs[i] >= fd_limit)
	fd_limit = evdevs[i] + 1;
      FD_SET(evdevs[i], &rfds);
    }

    /* Wait until some kind of activity, we really don't care what */
    if (select(fd_limit, &rfds, NULL, NULL, NULL) < 0) {
      perror("select");
      exit(1);
    }

    /* Read all events from each device, updating status. Keep track
     * of whether any controllers are finished, so we can combine
     * updates where possible.
     */
    need_update = 0;
    for (i=0; i<4; i++) {
      if (evdevs[i] <= 0)
	continue;

      while (read(evdevs[i], &ev, sizeof(ev)) > 0) {
	if (ev.type == EV_SYN)
	  need_update = 1;
	else
	  translate_event(&ev, &controllers[i]);
      }
    }

    if (need_update)
      gc_send_update(dev, controllers);
  }
}

void scan_for_controllers(char *dev_names[4])
{
  DIR *d;
  struct dirent *dent;
  const char* dir_name = "/dev/input";
  memset(dev_names, 0, sizeof(dev_names));

  /* Look through /dev/input/event* for devices of the form "Gamecube Controller #" */
  d = opendir(dir_name);
  if (!d) {
    perror("opendir");
    exit(1);
  }

  /* Cheesy, yep */
  while ((dent = readdir(d))) {
    char full_path[NAME_MAX];
    char dev_name[128];
    int fd;

    if (strncmp(dent->d_name, "event", 5))
      continue;

    strcpy(full_path, dir_name);
    strcat(full_path, "/");
    strcat(full_path, dent->d_name);

    fd = open(full_path, O_RDONLY);
    if (fd < 0)
      continue;
    dev_name[0] = 0;
    ioctl(fd, EVIOCGNAME(sizeof(dev_name)), dev_name);
    close(fd);

    /* Extra cheesy */
    if (strncmp(dev_name, "Gamecube Controller ", 20))
      continue;
    dev_names[dev_name[20] - '1'] = strdup(full_path);
  }

  closedir(d);
}

int main(int argc, char **argv)
{
  struct unicone_device* dev;
  struct progress_reporter* progress = progress_reporter_console_new();
  int evdevs[4];
  char *dev_names[4];
  int i;

  /* Look for gamecube controllers- that's all this little hack works with now */
  scan_for_controllers(dev_names);

  /* Open as many event devices as we can */
  for (i=0; i<4; i++) {
    evdevs[i] = open(dev_names[i], O_RDONLY | O_NONBLOCK);
    if (evdevs[i] > 0) {
      printf("Opened controller %d (%s)\n", i+1, dev_names[i]);
    }
  }

  unicone_usb_init();
  dev = unicone_device_new();
  if (!dev) {
    printf("Can't open unicone device\n");
    return 1;
  }

  if (unicone_device_configure(dev, "firmware.bin", "fpga/gamecube/gamecube.bit", progress) < 0) {
    printf("Error configuring unicone device\n");
    return 1;
  }

  printf("Running.\n");
  event_loop(dev, evdevs);

  return 0;
}

/* The End */
