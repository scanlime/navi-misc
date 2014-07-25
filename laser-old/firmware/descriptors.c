/*
 * USB descriptors for the laser projector device
 */

#include <usb_driver.h>

const static struct usb_device_descriptor dev_descript = {
  /* bLength            */  sizeof(struct usb_device_descriptor),
  /* bDescriptorType    */  USB_DT_DEVICE,
  /* bcdUSB             */  0x0110,
  /* bDeviceClass       */  USB_CLASS_PER_INTERFACE,
  /* bDeviceSubClass    */  0,
  /* bDeviceProtocol    */  0,
  /* bMaxPacketSize0    */  8,
  /* idVendor           */  0xE461,
  /* idProduct          */  0x0006,
  /* bcdDevice          */  0x0100,
  /* iManufacturer      */  1,
  /* iProduct           */  2,
  /* iSerialNumber      */  0,
  /* bNumConfigurations */  1,
};

const static struct {
  struct usb_config_descriptor config;
  struct usb_interface_descriptor if0;
  struct usb_endpoint_descriptor if0_ep0;
} config_descript = {
  /* Configuration */
  {
    /* bLength             */  sizeof(struct usb_config_descriptor),
    /* bDescriptorType     */  USB_DT_CONFIG,
    /* wTotalLength        */  sizeof(config_descript),
    /* bNumInterfaces      */  1,
    /* bConfigurationValue */  1,
    /* iConfiguration      */  0,
    /* bmAttributes        */  0x80,
    /* MaxPower            */  50,
  },
  /* First interface */
  {
    /* bLength             */  sizeof(struct usb_interface_descriptor),
    /* bDescriptorType     */  USB_DT_INTERFACE,
    /* bInterfaceNumber    */  0,
    /* bAlternateSetting   */  0,
    /* bNumEndpoints       */  1,
    /* bInterfaceClass     */  USB_CLASS_VENDOR_SPEC,
    /* bInterfaceSubClass  */  USB_CLASS_VENDOR_SPEC,
    /* bInterfaceProtocol  */  USB_CLASS_VENDOR_SPEC,
    /* iInterface          */  0,
  },
  /* First interface's first endpoint */
  {
    /* bLength             */  sizeof(struct usb_endpoint_descriptor),
    /* bDescriptorType     */  USB_DT_ENDPOINT,
    /* bEndpointAddress    */  1 | USB_DIR_OUT,
    /* bmAttributes        */  USB_ENDPOINT_XFER_BULK,
    /* wMaxPacketSize      */  4,
    /* bInterval           */  0,
  },
};

const static unsigned char str_lang_table[] = {
  sizeof(str_lang_table), USB_DT_STRING,
  0x09, 0x04, /* English */
};

const struct usb_descriptor_entry usb_descriptors[] = {
  {USB_DT_DEVICE, 0, &dev_descript,    sizeof(dev_descript), 0},
  {USB_DT_CONFIG, 0, &config_descript, sizeof(config_descript), 0},
  {USB_DT_STRING, 0, str_lang_table,   sizeof(str_lang_table), 0},
  {USB_DT_STRING, 1, "Micah Dowty <micah@navi.cx>", 0, 0},
  {USB_DT_STRING, 2, "Laser Projector", 0, 0},
  {0},
};

/* The End */
