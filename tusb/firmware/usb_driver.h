
/* The SETUP packet contents according to the USB spec,
 * but minus the silly hungarian notation.
 */
struct usb_setup {
  unsigned char request_type;
  unsigned char request;
  unsigned short value;
  unsigned short index;
  unsigned short length;
};

/* The hardware has a memory-mapped buffer where we receive incoming SETUP packets */
xdata at 0xFF00 struct usb_setup usb_setup_buffer;

/* USB directions */
#define USB_DIR_OUT    0       /* From host to device */
#define USB_DIR_IN     0x80    /* From device to host */

void usb_init();
void usb_poll();
