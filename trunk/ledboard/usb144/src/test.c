#include <usb.h>
#include <stdio.h>
#include <math.h>
#include <ledboard_protocol.h>


usb_dev_handle *open_ledboard(void) {
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
      if (dev->descriptor.idVendor == LEDBOARD_VENDOR_ID &&
	  dev->descriptor.idProduct == LEDBOARD_PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

void control_write(usb_dev_handle *d, int request, int value, int index) {
  if (usb_control_msg(d, USB_TYPE_VENDOR, request,
		      value, index, NULL, 0, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
}

int control_read_byte(usb_dev_handle *d, int request, int value, int index) {
  unsigned char reply;
  if (usb_control_msg(d, USB_TYPE_VENDOR | USB_ENDPOINT_IN, request,
		      value, index, &reply, 1, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
  return reply;
}

int brightness_fn(int x, int y) {
  static int o = 0;
  if (x==0 && y==0) o++;
  return (x+y+o) & 0x0F;
}

int main(int argc, char **argv) {
  usb_dev_handle *d;
  int x, y;

  if (!(d = open_ledboard())) {
    printf("Error opening device\n");
    return 1;
  }

  printf("%.02fV LED power supply\n",
	 control_read_byte(d, LEDBOARD_CTRL_GET_LED_VOLTAGE, 0, 0) / 255.0 * 10);

  control_write(d, LEDBOARD_CTRL_STATUS_INTENSITY, 0x1000, 0);
  control_write(d, LEDBOARD_CTRL_SET_PWM_CYCLES, 1, 0);
  control_write(d, LEDBOARD_CTRL_SET_SCAN_RATE, 0xFD00, 0);

  for (x=0; x<16; x++) {
    int pwmValue = pow(x/15.0, 2.5) * 20;
    printf("pwm table: %d -> %d\n", x, pwmValue);
    control_write(d, LEDBOARD_CTRL_SET_PWM_ENTRY,
		  pwmValue, x);
  }

  control_write(d, LEDBOARD_CTRL_FLIP, 0, 0);
  while (1) {
    for (y=0; y<9; y++)
      for (x=0; x<16; x+=8)
	control_write(d, LEDBOARD_CTRL_SEQ_WRITE4,
		      (brightness_fn(x+0, y) << 4) |
		      (brightness_fn(x+1, y) << 0) |
		      (brightness_fn(x+2, y) << 12) |
		      (brightness_fn(x+3, y) << 8),
		      (brightness_fn(x+4, y) << 4) |
		      (brightness_fn(x+5, y) << 0) |
		      (brightness_fn(x+6, y) << 12) |
		      (brightness_fn(x+7, y) << 8));

    control_write(d, LEDBOARD_CTRL_FLIP, 0, 0);
  }


  return 0;
}
