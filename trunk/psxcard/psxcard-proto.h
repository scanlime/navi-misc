/*
 * psxcard-proto.h - USB protocol definitions for the PSX Memory Card Reader
 *
 * --Micah Dowty <micah@navi.cx>
 */

#ifndef _PSXCARD_PROTO_H_
#define _PSXCARD_PROTO_H_

/*
 * The PSX card's equivalent of a disk sector is a
 * 'frame', equal to 128 bytes. Each 'block' is 8K.
 */
#define PSXCARD_FRAME_SIZE        128
#define PSXCARD_BLOCK_SIZE        (8 * 1024)
#define PSXCARD_CARD_SIZE         (128 * 1024)
#define PSXCARD_FRAMES_PER_BLOCK  (PSXCARD_BLOCK_SIZE / PSXCARD_FRAME_SIZE)

#define PSXCARD_VENDOR_ID    0xE461
#define PSXCARD_PRODUCT_ID   0x0013
#define PSXCARD_REVISION     0x0100

/*
 * The bulk IN/OUT endpoints are used to perform sequential
 * reads/writes after initiating a read/write using the proper
 * vendor-specific control request.
 */
#define PSXCARD_BULK_IN_EP   0x81
#define PSXCARD_BULK_OUT_EP  0x01

/*
 * Vendor specific EP0 device requests
 */

/*
 * Reset the error counters, stop any pending read/write,
 * and check for the presence of a memory card. Returns
 * one byte, '1' if a memory card is present, '0' if not.
 */
#define PSXCARD_REQ_INIT       0x01

/*
 * Returns 2 bytes:
 *
 *  1. Number of soft errors (Failed ACKs and bad checksums that were
 *     recovered from after retry)
 *
 *  2. Number of hard errors (data is incorrect!)
 *
 * Both counters are reset on INIT or on ERR_CHECK. They don't roll
 * over- on overflow, they saturate to 0xFF.
 */
#define PSXCARD_REQ_ERR_CHECK  0x02

typedef struct {
  unsigned char soft;
  unsigned char hard;
} PsxcardErrors;

/*
 * Begin a sequential read, at frame 'wValue'.
 */
#define PSXCARD_REQ_READ       0x03

/*
 * Begin a sequential write, at frame 'wValue'. Note that a frame is
 * only written once it has been completely received from the host.
 * Partial frames transmitted over USB will not be written.
 */
#define PSXCARD_REQ_WRITE      0x04

#endif

/* The End */
