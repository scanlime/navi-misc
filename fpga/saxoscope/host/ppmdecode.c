/*
 * Reads raw SaxoScope FIFO data from stdin, decodes PPM
 * (Pulse Position Modulated) R/C aircraft pulses. Outputs
 * each frame as text on stdout, with pulse lengths in
 * milliseconds.
 *
 * usage: fifocapture <device> | ppmdecode <probe#>
 *
 * -- Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <stdlib.h>

#define FIFO_KHZ        12000
#define RESET_PULSE_MS  5

int
main(int argc, char **argv)
{
    int probe_mask = 1 << atoi(argv[1]);
    unsigned long pulse_clocks;

    while (1) {
	unsigned char buffer[4096];
	int index;

	if (fread(buffer, sizeof buffer, 1, stdin) != 1) {
	    break;
	}

	for (index=0; index<sizeof(buffer); index++) {
	    unsigned char next_value = buffer[index];

	    if (next_value & probe_mask) {
		pulse_clocks++;
	    } else if (pulse_clocks > RESET_PULSE_MS * FIFO_KHZ) {
		printf("\n");
		pulse_clocks = 0;
	    } else if (pulse_clocks > 0) {
		printf("%.05f ", pulse_clocks / (float)FIFO_KHZ);
		pulse_clocks = 0;
	    }
	}
    }

    return 0;
}
