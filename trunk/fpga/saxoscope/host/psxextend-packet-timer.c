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
unsigned long long clock;

#define CLOCK_HZ 12000000


int next_sample(void)
{
    if (buffer_index == sizeof(buffer)) {
	if (fread(buffer, sizeof buffer, 1, stdin) != 1) {
	    exit(0);
	}
	buffer_index = 0;
    }

    clock++;
    return (buffer[buffer_index++] & probe_mask) != 0;
}

int average_samples(int n_samples)
{
    int sum = 0;
    int i;

    for (i=0; i<n_samples; i++) {
	sum += next_sample();
    }

    return sum >= (n_samples/2);
}


unsigned long long
filter(unsigned long long x)
{
    static unsigned long long acc;
    static unsigned long long state[128];
    static int head;

    acc -= state[head];
    state[head] = x;
    acc += state[head];
   
    head = (head + 1) % 128;
    return acc / 128;
}
    

int
main(int argc, char **argv)
{
    int divisor = atoi(argv[1]);
    probe_mask = 1 << atoi(argv[2]);

    /* Byte loop */
    while (1) {
	unsigned int byte = 0;
	int bit_count;

	/* Wait for start bit */
	while (next_sample() == 1);
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
	    if (byte == 0x51) {
		static unsigned long long prev_clk;
		printf("%.04f Hz\n", CLOCK_HZ / (double)filter(clock - prev_clk));
		prev_clk = clock;
	    }
	}
    }

    return 0;
}
