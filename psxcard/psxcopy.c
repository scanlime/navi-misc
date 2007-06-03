/*
 * Simple host application to copy memory blocks to and from a PSX
 * memory card, via the USB card reader implemented in psxcard-fw.
 *
 * --Micah Dowty <micah@navi.cx>
 */

#include <usb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "psxcard-proto.h"

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
typedef unsigned char uint8;

/*
 * PSX memory card header block data structures.
 */

typedef struct {
    uint8 flags;
    uint8 is_reserved[3];
    uint8 link_usage[4];
    uint8 link_order[2];

    uint8 country_code[2];
    uint8 product_code[10];
    uint8 identifier[8];

    uint8 blank[97];
    uint8 xor;
} PSXCardBlockInfo;

typedef struct {
    uint8 signature[2];               /* "MC" */
    uint8 frame0_blank[0x80 - 3];     /* All zero */
    uint8 frame0_xor;               /* 0x0E */

    PSXCardBlockInfo blocks[15];
} PSXCardHeader;

#define PSX_SIGNATURE        "MC"
#define PSX_FRAME0_XOR       0x0E

#define PSX_BLOCK_ALLOC_MASK 0xF0
#define PSX_BLOCK_AVAILABLE  0xA0
#define PSX_BLOCK_USED       0x50
#define PSX_BLOCK_UNUSABLE   0xF0

#define PSX_BLOCK_LINK_MASK  0x0F
#define PSX_BLOCK_NOLINK     0x00
#define PSX_BLOCK_LINKABLE   0x01
#define PSX_BLOCK_MIDDLE     0x02
#define PSX_BLOCK_END        0x03
     

/********************************************************************/
/************************************************* USB Interface ****/
/********************************************************************/

#define USB_TIMEOUT 1000  /* 1 second */

/*
 * Open the psxcard device. On error, writes an error message and
 * returns NULL.
 */
usb_dev_handle *psxcard_open(void)
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
      if (dev->descriptor.idVendor == PSXCARD_VENDOR_ID &&
	  dev->descriptor.idProduct == PSXCARD_PRODUCT_ID) {

	  usb_dev_handle *dh = usb_open(dev);
	  if (!dh) {
	      perror("usb_open");
	      return NULL;
	  }

	  if (usb_claim_interface(dh, 0)) {
	      perror("usb_claim_interface");
	      return NULL;
	  }

	  return dh;
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

/*
 * Check the psxcard device's error counters.
 * Soft errors are logged to stdout, hard errors
 * are fatal.
 */
void psxcard_error_check(usb_dev_handle *dev)
{
    PsxcardErrors errors;

    if (usb_control_msg(dev, USB_TYPE_VENDOR | 0x80,
			PSXCARD_REQ_ERR_CHECK,
			0, 0, (void*) &errors, sizeof errors,
			USB_TIMEOUT) < 0) {
	perror("PSXCARD_REQ_ERR_CHECK");
	exit(1);
    }

    if (errors.hard) {
	fprintf(stderr, "Unrecoverable card error.\n");
	exit(1);

	/*
	 * NB: Don't print soft errors if we have any hard
	 *     errors, since we invariably tried to retry
	 *     several times before deeming this a hard error.
	 */
    }

    if (errors.soft) {
	fprintf(stderr, "(%d recoverable error%s occurred)\n",
		errors.soft, errors.soft == 1 ? "" : "s");
    }
}

/*
 * Reset all device read/write state
 */
void psxcard_init(usb_dev_handle *dev)
{
    if (usb_control_msg(dev, USB_TYPE_VENDOR,
			PSXCARD_REQ_INIT,
			0, 0, NULL, 0,
			USB_TIMEOUT) < 0) {
	perror("PSXCARD_REQ_INIT");
	exit(1);
    }
}

/*
 * Sequential read or write, starting at the specified frame,
 * with a single large buffer in RAM. If 'verbose' is TRUE,
 * display progress info to stderr.
 *
 * 'request' should be either PSXCARD_REQ_READ or PSXCARD_REQ_WRITE.
 */
void psxcard_transfer(usb_dev_handle *dev, int request, int frame,
		      uint8 *buffer, size_t size, int verbose)
{
    size_t transferred = 0;
    int retval;

    psxcard_init(dev);

    if (usb_control_msg(dev, USB_TYPE_VENDOR, request,
			frame, 0, NULL, 0, USB_TIMEOUT) < 0) {
	perror("psxcard_transfer[request]");
	exit(1);
    }

    psxcard_error_check(dev);

    while (transferred < size) {
	size_t blocksize = MIN(512, size - transferred);
	
	switch (request) {

	case PSXCARD_REQ_READ:
	    retval = usb_bulk_read(dev, PSXCARD_BULK_IN_EP,
				   buffer + transferred, blocksize,
				   USB_TIMEOUT);
	    break;

	case PSXCARD_REQ_WRITE:
	    retval = usb_bulk_write(dev, PSXCARD_BULK_OUT_EP,
				    buffer + transferred, blocksize,
				    USB_TIMEOUT);
	    break;

	default:
	    assert(0);
	}

	if (retval <= 0) {
	    perror("psxcard_transfer[bulk]");
	    exit(1);
	}

	transferred += retval;

	if (verbose) {
	    fprintf(stderr, "  %d / %d bytes\r", transferred, size);
	}
    }
    if (verbose) {
	fprintf(stderr, "\n");
    }

    psxcard_error_check(dev);
}


/********************************************************************/
/******************************************** Generic File Layer ****/
/********************************************************************/

typedef struct {
    usb_dev_handle *card;
    int fd;
    int block;
    size_t size;
} GenericFile;


/*
 * Parse a specifier of the form (<filename>|card)[:block],
 * and open either the real file or the card it refers to.
 *
 * If we're opening a local file, the provided open() flags will be
 * used.  If we're opening a memory card, the provided usb_dev_handle
 * will be used if it's non-NULL. Otherwise we'll open the device
 * ourselves.
 *
 * On error, prints an appropriate message and exits.
 */
void generic_file_open(GenericFile *file, const char *specifier,
		       int openFlags, usb_dev_handle *card)
{
    char *buf = strdup(specifier);
    char *colon;
    assert(buf != NULL);

    memset(file, 0, sizeof *file);

    colon = strchr(buf, ':');
    if (colon) {
	/*
	 * Truncate the string here, and interpret the rest
	 * as a block number.
	 */
	file->block = atoi(colon+1);
	file->size = PSXCARD_BLOCK_SIZE;
	*colon = '\0';
    } else {
	file->size = PSXCARD_CARD_SIZE;
    }

    if (!strcmp(buf, "card")) {
	if (!card) {
	    card = psxcard_open();
	    if (!card) {
		exit(1);
	    }
	}
	file->card = card;
	return;
    }

    file->fd = open(buf, openFlags, 0666);
    if (file->fd < 0) {
	perror(buf);
	exit(1);
    }
    lseek(file->fd, PSXCARD_BLOCK_SIZE * file->block, SEEK_SET);

    free(buf);
}

/*
 * Read from a generic file into a new allocated buffer.
 * On error, prints an appropriate message and exits.
 */
void *generic_file_read(GenericFile *file,
			int blockOffset,
			int frameOffset,
			int size)
{
    void *buffer = malloc(size);
    assert(buffer != NULL);

    if (file->card) {
	fprintf(stderr, "Reading...\n");
	psxcard_transfer(file->card, PSXCARD_REQ_READ,
			 (blockOffset + file->block) * PSXCARD_FRAMES_PER_BLOCK +
			 frameOffset, buffer, size, 1);
    } else {
	if (read(file->fd, buffer, size) != size) {
	    fprintf(stderr, "File read error\n");
	    exit(1);
	}
    }

    return buffer;
}

/*
 * Write to a generic file.
 * On error, prints an appropriate message and exits.
 */
void generic_file_write(GenericFile *file,
			void *buffer, int size)
{
    if (file->card) {
	void *verified;

	if (size % PSXCARD_FRAME_SIZE) {
	    fprintf(stderr, "Write to card must be a multiple of the frame size.\n");
	    exit(1);
	}

	fprintf(stderr, "Writing...\n");
	psxcard_transfer(file->card, PSXCARD_REQ_WRITE,
			 file->block * PSXCARD_FRAMES_PER_BLOCK,
			 buffer, size, 1);

	fprintf(stderr, "Verifying...\n");
	verified = generic_file_read(file, 0, 0, size);
	if (memcmp(buffer, verified, size)) {
	    fprintf(stderr, "Verification FAILED\n");
	} else {
	    fprintf(stderr, "Write successful.\n");
	}
	
    } else {
	if (write(file->fd, buffer, size) != size) {
	    fprintf(stderr, "File write error\n");
	    exit(1);
	}
    }
}


/********************************************************************/
/************************************************** Command Line ****/
/********************************************************************/

int psxcardheader_validate(PSXCardHeader *header)
{
    int i;

    if (memcmp(header->signature, PSX_SIGNATURE, sizeof header->signature)) {
	fprintf(stderr, "Invalid memory card signature.\n");
	return 0;
    }

    for (i = 0; i < sizeof header->frame0_blank; i++) {
	if (header->frame0_blank[i] != 0x00) {
	    fprintf(stderr, "Warning, Frame 0 contains unexpected data in reserved area.\n");
	    /* Don't bother validating further, but let us pass */
	    return 1;
	}
    }

    if (header->frame0_xor != PSX_FRAME0_XOR) {
	fprintf(stderr, "Warning, Frame 0 has unexpected XOR check.\n");
    }

    return 1;
}

void ascii_dump(uint8 *buffer, size_t size)
{
    while (size-- > 0) {
	if (isspace(*buffer) || !*buffer) {
	    putchar(' ');
	} else if (isprint(*buffer)) {
	    putchar(*buffer);
	} else {
	    putchar('.');
	}
	buffer++;
    }
}

void hex_dump(uint8 *buffer, size_t size)
{
    while (size-- > 0) {
	printf("%02x", *(buffer++));
    }
}

void psxcardheader_print(PSXCardHeader *header)
{
    int block;

    printf(" Block Ctry / Product  Ident    Reserv LnkUsage Ordr Flags\n"
	   " ---------------------------------------------------------------------\n");

    for (block = 1; block < 16; block++) {
	PSXCardBlockInfo *bi = &header->blocks[block-1];

	printf(" %-5d ", block);
	ascii_dump(bi->country_code, sizeof bi->country_code);
	printf(" / ");
	ascii_dump(bi->product_code, sizeof bi->product_code);
	printf(" ");
	ascii_dump(bi->identifier, sizeof bi->identifier);

	printf(" ");
	hex_dump(bi->is_reserved, sizeof bi->is_reserved);
	printf(" ");
	hex_dump(bi->link_usage, sizeof bi->link_usage);
	printf(" ");
	hex_dump(bi->link_order, sizeof bi->link_order);

	switch (bi->flags & PSX_BLOCK_ALLOC_MASK) {
	case PSX_BLOCK_AVAILABLE:
	    printf(" Available");
	    break;
	case PSX_BLOCK_USED:
	    printf(" In-Use");
	    break;
	case PSX_BLOCK_UNUSABLE:
	    printf(" Unusable");
	    break;
	default:
	    printf(" (%02x)", bi->flags & PSX_BLOCK_ALLOC_MASK);
	    break;
	}

	switch (bi->flags & PSX_BLOCK_LINK_MASK) {
	case PSX_BLOCK_NOLINK:
	    printf(" No-Link");
	    break;
	case PSX_BLOCK_LINKABLE:
	    printf(" Linkable");
	    break;
	case PSX_BLOCK_MIDDLE:
	    printf(" Middle");
	    break;
	case PSX_BLOCK_END:
	    printf(" End");
	    break;
	default:
	    printf(" (%02x)", bi->flags & PSX_BLOCK_LINK_MASK);
	    break;
	}

	printf("\n");
    }
}

int cmd_list(const char *filename)
{
    GenericFile f;
    PSXCardHeader *header;

    generic_file_open(&f, filename, O_RDONLY, NULL);
    header = generic_file_read(&f, 0, 0, sizeof *header);

    if (psxcardheader_validate(header)) {
	psxcardheader_print(header);
    }
    
    free(header);
    return 0;
}

int cmd_copy(const char *fromSpec, const char *toSpec)
{
    GenericFile fromFile, toFile;
    void *buffer;

    generic_file_open(&fromFile, fromSpec, O_RDONLY, NULL);
    generic_file_open(&toFile, toSpec, O_RDWR | O_CREAT, NULL);

    buffer = generic_file_read(&fromFile, 0, 0, fromFile.size);
    generic_file_write(&toFile, buffer, MIN(fromFile.size, toFile.size));
    free(buffer);

    return 0;
}

int cmd_usage(void)
{
    fprintf(stderr, "\n"
	    "PSX Memory Card Interface\n"
	    "Micah Dowty <micah@navi.cx>\n"
	    "\n"
	    "usage: psxcopy -l [<filename>]\n"
	    "\n"
	    "       List a card's table of contents. By default, this will operate\n"
	    "       on a physical PSX card. It can optionally be used on raw card\n"
	    "       images from disk.\n"
	    "\n"
	    "usage: psxcopy (<filename>|card)[:<block>] (<filename>|card)[:<block>]\n"
	    "\n"
	    "       Copy data between two files. 'card' is a special filename\n"
            "       that refers to the entire raw memory card. Optional suffixes\n"
	    "       can be added to select an individual block in a file or in the\n"
	    "       memory card. See the card's table of contents to locate the\n"
	    "       block number of the save file you're intrested in.\n"
	    "\n"
	    "       Note that all block copies are raw, and the table of contents\n"
	    "       will not automatically be updated.\n"
	    "\n");
    return 1;
}

int main(int argc, char **argv)
{
    if ((argc == 2 || argc == 3) && !strcmp(argv[1], "-l")) {
	return cmd_list(argc == 3 ? argv[2] : "card");
    }

    if (argc == 3) {
	return cmd_copy(argv[1], argv[2]);
    }

    return cmd_usage();
}
