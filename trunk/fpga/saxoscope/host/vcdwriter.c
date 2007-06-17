/*
 * Reads raw FIFO data from stdin, writes a Verilog VCD waveform file to stdout.
 * By default, names the channels 'probe_0' through 'probe_7'. Optionally, new
 * names can be specified on the command line starting with the LSB.
 *
 * Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>

#define CLOCK_HZ       12000000
#define PROBE_BITS     8

int
main(int argc, char **argv)
{
    unsigned long long timestep = 0;
    int previous_value = 0;
    int channel;

    /* Header */
    printf("$version SaxoScope $end\n");
    printf("$timescale 1 ns $end\n");

    /* Variable definitions */
    printf("$scope module saxo $end\n");
    for (channel=0; channel<PROBE_BITS; channel++) {
	printf("$var wire 1 %c ", '!'+channel);
	if (channel+1 < argc) {
	    printf("%s", argv[channel+1]);
	} else {
	    printf("probe_%d", channel);
	}
	printf(" $end\n");
    }
    printf("$upscope $end\n");
    printf("$enddefinitions $end\n");

    /* Initial values */
    printf("#0\n$dumpvars\n");
    for (channel=0; channel<PROBE_BITS; channel++) {
	printf("0%c\n", '!'+channel);
    }
    printf("$end\n");

    while (1) {
	unsigned char buffer[4096];
	int byte;

	if (fread(buffer, sizeof buffer, 1, stdin) != 1) {
	    break;
	}

	for (byte=0; byte<sizeof(buffer); byte++) {
	    int next_value = buffer[byte];
	    if (next_value != previous_value) {

		/* Convert clock ticks to nanoseconds */
		printf("#%lld\n", (timestep * 1000000000) / CLOCK_HZ);

		for (channel=0; channel<PROBE_BITS; channel++) {
		    int mask = 1 << channel;

		    if ((previous_value & mask) != (next_value & mask)) {
			printf("%c%c\n", (next_value & mask) ? '1' : '0', '!'+channel);
		    }
		}

		previous_value = next_value;
	    }
	    timestep++;
	}
    }

    /* All values are unknown at end of file */
    printf("$dumpoff\n");
    for (channel=0; channel<PROBE_BITS; channel++) {
	printf("x%c\n", '!'+channel);
    }
    printf(" $end\n");

    return 0;
}
