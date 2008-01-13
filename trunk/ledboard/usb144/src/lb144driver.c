/*
 * Userspace LED board USB/144 driver.
 * Micah Dowty <micah@navi.cx>
 *
 * This is a fairly dead-simple driver program which reads frames, one
 * per line with one hexadecimal digit per pixel, and spits those
 * frames out over USB. We do it as fast as possible given the bugs
 * and limitations of the device's firmware.
 *
 */

#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <ledboard_protocol.h>

#define MAX_ASYNC_TRANSFERS  32
#define PAGE_FLIP_DELAY_US   10000
#define DISPLAY_WIDTH        16
#define DISPLAY_HEIGHT       9

#define FALSE  0
#define TRUE   1

void control_write(int fd, int request, int value, int index, int sync)
{
  static int pending_urb_count = 0;
  struct usbdevfs_urb *urb;
  struct usbdevfs_ctrltransfer *setup;

  urb = calloc(sizeof *urb, 1);
  setup = calloc(sizeof *setup, 1);

  urb->type = USBDEVFS_URB_TYPE_CONTROL;
  urb->endpoint = 0;
  urb->buffer = setup;
  urb->buffer_length = 8;

  setup->bRequestType = 0x40;
  setup->bRequest = request;
  setup->wValue = value;
  setup->wIndex = index;
  setup->wLength = 0;

  pending_urb_count++;
  if (ioctl(fd, USBDEVFS_SUBMITURB, urb) < 0) {
    perror("submit_urb");
    exit(1);
  }

  while ((sync && pending_urb_count > 0) ||
	 pending_urb_count > MAX_ASYNC_TRANSFERS) {

      if (ioctl(fd, USBDEVFS_REAPURB, &urb) < 0) {
	perror("reap_urb");
	exit(1);
      }

      free(urb->buffer);
      free(urb);
      pending_urb_count--;
  }
}

int hexdigit_to_int(char digit)
{
  if (digit >= '0' && digit <= '9')
    return digit - '0';

  if (digit >= 'a' && digit <= 'f')
    return digit - 'a' + 10;

  if (digit >= 'A' && digit <= 'F')
    return digit - 'A' + 10;

  return 0;
}

int main(int argc, char **argv) {
  int fd;
  int x, y;
  char framebuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT] = { 0 };

  if (argc != 2) {
    printf("usage: %s /proc/bus/usb/<bus>/<dev>\n", argv[0]);
    return 1;
  }

  fd = open(argv[1], O_RDWR);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  control_write(fd, LEDBOARD_CTRL_STATUS_INTENSITY, 0x1000, 0, FALSE);
  control_write(fd, LEDBOARD_CTRL_SET_PWM_CYCLES, 20, 0, FALSE);

  /*
   * This number is extremely sensitive and magical. Setting it
   * too low will crash the microcontroller!
   */
  control_write(fd, LEDBOARD_CTRL_SET_SCAN_RATE, 0xFE00, 0, FALSE);

  /*
   * Set up some really basic gamma correction using the
   * PWM lookup table. This makes better use of our 4-bit
   * brightness range than a linear duty cycle mapping would.
   */
  for (x=0; x<16; x++) {
    int pwmValue = pow(x/15.0, 1.1) * 20;
    control_write(fd, LEDBOARD_CTRL_SET_PWM_ENTRY,
		  pwmValue, x, FALSE);
  }

  while (1) {
    for (y = 0; y < DISPLAY_HEIGHT; y++)
      for (x=0; x < DISPLAY_WIDTH; x+=8) {
	const char *line = framebuffer + (y * DISPLAY_WIDTH);

	/*
	 * The microcontroller firmware is buggy. It can't handle
	 * control transfers with a data stage, so the fastest we can
	 * write is 4 bytes per transfer. To mitigate the awfulness,
	 * use asynchronous writes so we're hopefully guaranteed to
	 * have a control transfer on every USB frame.
	 */
	control_write(fd, LEDBOARD_CTRL_SEQ_WRITE4,
		      (hexdigit_to_int(line[x + 0]) << 4) |
		      (hexdigit_to_int(line[x + 1]) << 0) |
		      (hexdigit_to_int(line[x + 2]) << 12) |
		      (hexdigit_to_int(line[x + 3]) << 8),
		      (hexdigit_to_int(line[x + 4]) << 4) |
		      (hexdigit_to_int(line[x + 5]) << 0) |
		      (hexdigit_to_int(line[x + 6]) << 12) |
		      (hexdigit_to_int(line[x + 7]) << 8),
		      FALSE);
      }

    /*
     * Work around another microcontroller bug: Any USB traffic
     * during the page flip itself will cause flickering on the
     * display. We have to request the flip synchronously,
     * then sleep a bit to ensure the microcontroller has time
     * to complete the flip.
     */
    control_write(fd, LEDBOARD_CTRL_FLIP, 0, 0, TRUE);
    usleep(PAGE_FLIP_DELAY_US);

    /* Read the next frame */
    fgets(framebuffer, sizeof framebuffer, stdin);
  }

  return 0;
}
