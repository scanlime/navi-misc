#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>

#define DEBUG
#include <linux/usb.h>

#define USB_WIDGET_VENDOR_ID	0xe461
#define USB_WIDGET_PRODUCT_ID	0x0001

#define RECV_BUFFER_SIZE 128

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "The Unknown Tribble"
#define DRIVER_DESC "USB Widget Driver"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

struct usb_widget {
	signed char data[RECV_BUFFER_SIZE];
	char name[128];
	struct usb_device *usbdev;
	struct urb irq;
	int open;
};

static void usb_widget_irq(struct urb *urb)
{
	struct usb_widget *widget = urb->context;
	unsigned char *data = widget->data;

	if (urb->status) return;

	dbg("Received %d bytes - %02X %02X %02X %02X", urb->actual_length, data[0], data[1], data[2], data[3]);
}

static void *usb_widget_probe(struct usb_device *dev, unsigned int ifnum,
			     const struct usb_device_id *id)
{
	struct usb_interface *iface;
	struct usb_interface_descriptor *interface;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_widget *widget;
	int pipe, maxp;
	char *buf;

	if ((dev->descriptor.idVendor != USB_WIDGET_VENDOR_ID) ||
	    (dev->descriptor.idProduct != USB_WIDGET_PRODUCT_ID))
		return NULL;

	iface = &dev->actconfig->interface[ifnum];
	interface = &iface->altsetting[iface->act_altsetting];

	dbg("bNumEndpoints = %d\n", interface->bNumEndpoints);

	if (interface->bNumEndpoints < 1) return NULL;

	endpoint = interface->endpoint + 0;

	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
	maxp = usb_maxpacket(dev, pipe, usb_pipeout(pipe));

	usb_set_idle(dev, interface->bInterfaceNumber, 0, 0);

	if (!(widget = kmalloc(sizeof(struct usb_widget), GFP_KERNEL))) return NULL;
	memset(widget, 0, sizeof(struct usb_widget));

	widget->usbdev = dev;

	FILL_INT_URB(&widget->irq, dev, pipe, widget->data, min(maxp, RECV_BUFFER_SIZE),
		usb_widget_irq, widget, endpoint->bInterval);

	widget->irq.dev = widget->usbdev;
	if (usb_submit_urb(&widget->irq))
		return NULL;

	return widget;
}

static void usb_widget_disconnect(struct usb_device *dev, void *ptr)
{
	struct usb_widget *widget = ptr;
	usb_unlink_urb(&widget->irq);
	kfree(widget);
}

static struct usb_device_id usb_widget_id_table [] = {
	{ USB_DEVICE(USB_WIDGET_VENDOR_ID, USB_WIDGET_PRODUCT_ID) },
	{ }	/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, usb_widget_id_table);

static struct usb_driver usb_widget_driver = {
	name:		"usb_widget",
	probe:		usb_widget_probe,
	disconnect:	usb_widget_disconnect,
	id_table:	usb_widget_id_table,
};

static int __init usb_widget_init(void)
{
	usb_register(&usb_widget_driver);
	info(DRIVER_VERSION ":" DRIVER_DESC);
	dbg("Poing!");
	return 0;
}

static void __exit usb_widget_exit(void)
{
	usb_deregister(&usb_widget_driver);
}

module_init(usb_widget_init);
module_exit(usb_widget_exit);
