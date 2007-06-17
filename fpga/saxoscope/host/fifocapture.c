/*
 * Reads continuously from Saxo's FIFO4, writes raw data to stdout.
 *
 * Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <linux/ioctl.h>
#include <linux/usbdevice_fs.h>

#define USB_CONFIG     1
#define USB_IFACE      0
#define USB_ALTSETTING 1
#define USB_ENDPOINT   0x86

/* Yes, this buffer has to be giant to stream at full USB 2.0 speed without interruption */
#define NUM_URBS       512
#define BUFFER_SIZE    4096

int
main(int argc, char **argv)
{
    int i, device_fd;
    struct usbdevfs_setinterface si;
    struct usbdevfs_urb urbs[NUM_URBS];
    unsigned char buffers[NUM_URBS][BUFFER_SIZE];

    if (argc != 2) {
	fprintf(stderr, "usage: %s <device>\n", argv[0]);
	return 1;
    }

    device_fd = open(argv[1], O_RDWR);
    if (device_fd <= 0) {
	perror("device");
	return 1;
    }

    i = USB_CONFIG;
    if (ioctl(device_fd, USBDEVFS_SETCONFIGURATION, &i) < 0) {
	perror("USBDEVFS_SETCONFIGURATION");
	return 1;
    }


    i = USB_IFACE;
    if (ioctl(device_fd, USBDEVFS_CLAIMINTERFACE, &i) < 0) {
	perror("USBDEVFS_CLAIMINTERFACE");
	return 1;
    }

    si.interface = USB_IFACE;
    si.altsetting = USB_ALTSETTING;
    if (ioctl(device_fd, USBDEVFS_SETINTERFACE, &si) < 0) {
	perror("USBDEVFS_SETINTERFACE");
	return 1;
    }

    /* Send out initial URBs */
    memset(urbs, 0, sizeof urbs);
    for (i=0; i<NUM_URBS; i++) {
	urbs[i].type = USBDEVFS_URB_TYPE_BULK;
	urbs[i].endpoint = USB_ENDPOINT;
	urbs[i].buffer = buffers[i];
	urbs[i].buffer_length = BUFFER_SIZE;
	urbs[i].actual_length = BUFFER_SIZE;

	if (ioctl(device_fd, USBDEVFS_SUBMITURB, &urbs[i]) < 0) {
	    perror("USBDEVFS_SUBMITURB");
	    return 1;
	}
    }

    /* Wait for completions */
    while (1) {
	struct usbdevfs_urb *urb;

	if (ioctl(device_fd, USBDEVFS_REAPURB, &urb) < 0) {
	    perror("USBDEVFS_REAPURB");
	    continue;
	}

	fwrite(urb->buffer, urb->actual_length, 1, stdout);
	
	/* Resubmit URB */
	if (ioctl(device_fd, USBDEVFS_SUBMITURB, urb) < 0) {
	    perror("USBDEVFS_SUBMITURB");
	    return 1;
	}
    }

    return 0;
}
