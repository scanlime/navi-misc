/*
 * An example for using the features of the rcpod-485 board:
 * This program will send packets over RS-485 in the 'micronet' format
 * used by the Citizen Explorer satellite. It will then display all
 * the data received, and an indication of whether it was a valid packet.
 *
 * Anatomy of a micronet packet:
 *   - First byte is a destination device ID
 *   - Second byte is a source device ID
 *   - Third byte is the number of following data bytes
 *   - Data bytes
 *   - An 8-bit checksum byte such that all bytes in the packet add up to 255, modulo 256
 *
 * -- Micah Dowty <micah@picogui.org>
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <rcpod.h>

int mnetsend(int source, int destination, const unsigned char *data, int dataBytes, int timeout);
void usage(void);


int main(int argc, char **argv) {
  int i;
  int source = 10;      /* default source */
  int destination = 2;  /* default destination (critical decoder) */
  int timeout = 1;      /* defualt timeout */
  unsigned char data[64];  /* packet data */
  int dataBytes = 0;

  /* Scan for command line options using getopt(3) */
  while (1) {
    i = getopt(argc, argv, "hs:d:t:");
    if (i==-1)
      break;
    switch (i) {

    case 's':
      source = atoi(optarg);
      break;

    case 'd':
      destination = atoi(optarg);
      break;

    case 't':
      timeout = atoi(optarg);
      break;

    default:
      usage();
      return 1;
    }
  }

  /* Process non-option arguments as data bytes */
  for (i=optind; i<argc; i++) {
    data[dataBytes++] = atoi(argv[i]);
    if (dataBytes >= sizeof(data)) {
      fprintf(stderr, "Too much packet data\n");
      return 1;
    }
  }

  /* We need at least one byte of data */
  if (!dataBytes) {
    usage();
    return 1;
  }

  return mnetsend(source, destination, data, dataBytes, timeout);
}

void usage(void) {
  fprintf(stderr,
	  "usage:\tmnetsend [-h] [-s source] [-d destination] [-t timeout] data\n"
	  "\n"
	  "Transmits a micronet packet with the supplied data, source, and\n"
	  "destination, and a calculated checksum. Displays the received response.\n"
	  "\n"
	  " -h             : This message\n"
	  " -t timeout     : Number of seconds to wait for a response, defaults to 1\n"
	  " -s source      : Specifies the source device address, defaults to 10\n"
	  " -d destination : Specifies the destination device address, defaults to 2\n"
	  "                  (the critical decoder)\n"
	  " data           : Data bytes for this packet, as decimal integers\n");
}


int mnetsend(int source, int destination, const unsigned char *data, int dataBytes, int timeout) {
  unsigned char packet[RCPOD_SCRATCHPAD_SIZE];
  unsigned char csum;
  int packetBytes;
  int i;
  rcpod_dev* rcpod;

  /* Show our input, to verify we're doing what the user really meant */
  printf("Sending data from device %d to device %d:", source, destination);
  for (i=0; i<dataBytes; i++)
    printf(" %d", data[i]);
  printf("\n");

  /* Assemble the packet, according to the format described at the top of this file */
  packet[0] = destination;
  packet[1] = source;
  packet[2] = dataBytes;
  memcpy(&packet[3], data, dataBytes);
  packetBytes = dataBytes + 4;

  /* Calculate the checksum by initially storing 255, then subtracting each other byte in the packet */
  csum = 255;
  for (i=0; i<packetBytes-1; i++)
    csum -= packet[i];
  packet[packetBytes-1] = csum;

  /* Show the completed packet */
  printf("Assembled packet, in hex:");
  for (i=0; i<packetBytes; i++)
    printf(" %02X", packet[i]);
  printf("\n");

  rcpod = rcpod_InitSimple();                                 /* Initialize the rcpod */
  rcpod_SerialInit(rcpod, 9600);                              /* Turn on the UART, set it to 9600 bps */
  rcpod_GpioAssert(rcpod, RCPOD_OUTPUT(RCPOD485_RS485_TXE));  /* Make the transmit enable pin an output */
  rcpod_SerialSetTxEnable(rcpod, RCPOD485_RS485_TXE);         /* Set the transmit enable pin */

  /* Send our packet and start receiving a response */
  printf("Sending...\n");
  rcpod_SerialTxRxStart(rcpod, packet, packetBytes, sizeof(packet));

  /* Wait for a response. The rcpod will be receiving it to its
   * scratchpad RAM. We then pick up whatever it has to offer us into our own buffer.
   */
  printf("Waiting %d seconds for a response...\n", timeout);
  sleep(timeout);
  packetBytes = rcpod_SerialRxFinish(rcpod, packet, sizeof(packet));

  /* Show the raw received packet */
  printf("Received %d byte%s:", packetBytes, packetBytes==1 ? "" : "s");
  for (i=0; i<packetBytes; i++)
    printf(" %02X", packet[i]);
  printf("\n");

  return 0;
}

/* The End */
