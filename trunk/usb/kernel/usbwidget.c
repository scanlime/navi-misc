#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>

#define DEBUG
#include <linux/usb.h>

#define USB_WIDGET_VENDOR_ID	0xe461
#define USB_WIDGET_PRODUCT_ID	0x0001

#define DRIVER_VERSION "v0.0"
#define DRIVER_AUTHOR "The Unknown Tribble"
#define DRIVER_DESC "USB HID Boot Protocol widget driver"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

struct usb_widget {
	signed char data[8];
	char name[128];
	struct usb_device *usbdev;
	struct input_dev dev;
	struct urb irq;
	int open;
};

static void usb_widget_irq(struct urb *urb)
{
	struct usb_widget *widget = urb->context;
	unsigned char *data = widget->data;
	struct input_dev *dev = &widget->dev;

	if (urb->status) return;

	dbg("Received %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);

	/*
	input_report_key(dev, BTN_LEFT,   data[0] & 0x01);
	input_report_key(dev, BTN_RIGHT,  data[0] & 0x02);
	input_report_key(dev, BTN_MIDDLE, data[0] & 0x04);
	input_report_key(dev, BTN_SIDE,   data[0] & 0x08);
	input_report_key(dev, BTN_EXTRA,  data[0] & 0x10);

	input_report_rel(dev, REL_X,     data[1]);
	input_report_rel(dev, REL_Y,     data[2]);
	input_report_rel(dev, REL_WHEEL, data[3]);
	*/
}

static int usb_widget_open(struct input_dev *dev)
{
	struct usb_widget *widget = dev->private;

	if (widget->open++)
		return 0;

	widget->irq.dev = widget->usbdev;
	if (usb_submit_urb(&widget->irq))
		return -EIO;

	return 0;
}

static void usb_widget_close(struct input_dev *dev)
{
	struct usb_widget *widget = dev->private;

	if (!--widget->open)
		usb_unlink_urb(&widget->irq);
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

	widget->dev.evbit[0] = BIT(EV_KEY) | BIT(EV_REL);
	widget->dev.keybit[LONG(BTN_MOUSE)] = BIT(BTN_LEFT) | BIT(BTN_RIGHT) | BIT(BTN_MIDDLE);
	widget->dev.relbit[0] = BIT(REL_X) | BIT(REL_Y);
	widget->dev.keybit[LONG(BTN_MOUSE)] |= BIT(BTN_SIDE) | BIT(BTN_EXTRA);
	widget->dev.relbit[0] |= BIT(REL_WHEEL);

	widget->dev.private = widget;
	widget->dev.open = usb_widget_open;
	widget->dev.close = usb_widget_close;

	widget->dev.name = widget->name;
	widget->dev.idbus = BUS_USB;
	widget->dev.idvendor = dev->descriptor.idVendor;
	widget->dev.idproduct = dev->descriptor.idProduct;
	widget->dev.idversion = dev->descriptor.bcdDevice;

	if (!(buf = kmalloc(63, GFP_KERNEL))) {
		kfree(widget);
		return NULL;
	}

	if (dev->descriptor.iManufacturer &&
		usb_string(dev, dev->descriptor.iManufacturer, buf, 63) > 0)
			strcat(widget->name, buf);
	if (dev->descriptor.iProduct &&
		usb_string(dev, dev->descriptor.iProduct, buf, 63) > 0)
			sprintf(widget->name, "%s %s", widget->name, buf);

	if (!strlen(widget->name))
		sprintf(widget->name, "USB HIDBP Widget %04x:%04x",
			widget->dev.idvendor, widget->dev.idproduct);

	kfree(buf);

	FILL_INT_URB(&widget->irq, dev, pipe, widget->data, maxp > 8 ? 8 : maxp,
		usb_widget_irq, widget, endpoint->bInterval);

	input_register_device(&widget->dev);

	printk(KERN_INFO "input%d: %s on usb%d:%d.%d\n",
		 widget->dev.number, widget->name, dev->bus->busnum, dev->devnum, ifnum);

	return widget;
}

static void usb_widget_disconnect(struct usb_device *dev, void *ptr)
{
	struct usb_widget *widget = ptr;
	usb_unlink_urb(&widget->irq);
	input_unregister_device(&widget->dev);
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
