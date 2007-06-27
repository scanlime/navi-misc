/*
 * Reads raw SaxoScope FIFO data from stdin, decodes asynchronous
 * serial data (8-N-1) to stdout.
 *
 * usage: fifocapture <device> | serialdecode <divisor> <probe#>
 *
 * 'divisor' is the number of SaxoScope clock cycles per bit.
 * For example, on a 12 MHz FIFO stream, a 250 kbaud signal would
 * have a divisor of 48.
 *
 * -- Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <stdlib.h>

unsigned char buffer[4096];
int buffer_index = sizeof(buffer);
int probe_mask;
int header_filter;
int divisor;
unsigned long long clock;

int next_sample(void)
{
    if (buffer_index == sizeof(buffer)) {
	if (fread(buffer, sizeof buffer, 1, stdin) != 1) {
	    exit(0);
	}
	buffer_index = 0;
    }

    return (buffer[buffer_index++] & probe_mask) != 0;
}

int average_samples(int n_samples)
{
    int sum = 0;
    int i;

    for (i=0; i<n_samples; i++) {
	sum += next_sample();
    }

    clock++;
    return sum >= (n_samples/2);
}

int
recv_byte()
{
    /* Byte loop */
    clock = 0;

    while (1) {
	unsigned int byte = 0;
	int bit_count;

	/* Wait for start bit */
	while (next_sample() == 1) {
	    if (clock > 12000) /* 1ms */
		return -1;
	}
	if (average_samples(divisor-1) == 1) {
	    /* False start bit */
	    continue;
	}

	/* Collect data bits, LSB first */
	for (bit_count=0; bit_count<8; bit_count++) {
	    if (average_samples(divisor)) {
		byte |= 1 << bit_count;
	    }
	}

	/* Check for stop bit */
	if (average_samples(divisor) == 1) {
	    return byte;
	}
    }
}

#define STATE_CONNECTED  0x01
#define STATE_ANALOG     0x02
#define STATE_DUALSHOCK  0x04

void
recv_packet()
{
    int i;
    int length = 2;
    unsigned char buffer[32];

    for (i=0; i<length; i++) {
	int byte = recv_byte();
	if (byte < 0)
	    return;
	buffer[i] = byte;

	if (i==0 && !(byte == 0x50 || byte == 0x51))
	    return;

	if (i==1) {
	    if (byte & STATE_CONNECTED)
		length = 4;
	    if (byte & STATE_ANALOG)
		length = 8;
	    if (byte & STATE_DUALSHOCK)
		length = 20;
	}
    }

    if (buffer[0] == header_filter) {
      for (i=0; i<length; i++) {
	printf("%02x ", buffer[i]);
      }
      printf("\n");
    }
}

int
main(int argc, char **argv)
{
    divisor = atoi(argv[1]);
    probe_mask = 1 << atoi(argv[2]);
    header_filter = strtol(argv[3], NULL, 16);

    while (1) {
      recv_packet();
    }

    return 0;
}
