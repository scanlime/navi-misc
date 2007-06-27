/*
 * Reads raw SaxoScope FIFO data from stdin, decodes Playstation
 * (PSX/PS2) serial packets to stdout. These packets are found on both
 * the controller and memory card busses of the Playstation and PS2.
 *
 * Input signals should be wired as follows: (With 1.5k series resistors)
 *
 *    SaxoScope Name   Saxo pin #   PSX Name  PSX Pin #
 *    -------------------------------------------------
 *    probe_0          1            ACK       9
 *    probe_1          3            CLK       7
 *    probe_2          5            SEL       6
 *    probe_3          7            CMD       2
 *    probe_4          9            DAT       1
 *
 * Output format:
 * 
 *    Each line of text corresponds to one SEL pulse. The
 *    line begins when SEL goes low, and ends when it goes
 *    high. A SEL with no data would yield a blank line.
 *
 *    Each line begins with a timestamp indicating when SEL
 *    went low, and contains:
 *
 *    - TT/RR
 *      Indicates one byte of command/response data. Playstation
 *      devices always transmit and receive simultaneously. 'TT'
 *      represents a hexadecimal byte of data transmitted from
 *      playstation to device, 'RR' is the simultaneously received
 *      byte.
 *
 *    - "_"
 *      Indicates a high->low transition on the ACK pin. Usually
 *      occurs after every TT/RR pair other than the last one
 *      in a packet.
 *
 * -- Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>

#define ACK_MASK  0x01
#define CLK_MASK  0x02
#define SEL_MASK  0x04
#define CMD_MASK  0x08
#define DAT_MASK  0x10

#define CLOCK_HZ  12000000

int
main(int argc, char **argv)
{
    unsigned char prev_value = 0xFF;
    int bit_count = 0;
    unsigned char cmd_byte = 0, dat_byte = 0;
    unsigned long long clock_ticks = 0;

    while (1) {
	unsigned char buffer[4096];
	int index;

	if (fread(buffer, sizeof buffer, 1, stdin) != 1) {
	    break;
	}

	for (index=0; index<sizeof(buffer); index++) {
	    unsigned char next_value = buffer[index];

	    if (next_value & SEL_MASK) {
		/*
		 * SEL is still high. The only thing we can do in this
		 * state is wait for SEL to go low. If SEL just went
		 * high, end the previous line.
		 */
		if (!(prev_value & SEL_MASK)) {
		    printf("\n");
		}
	    } else {
		/* SEL is low */

		if (prev_value & SEL_MASK) {
		    /*
		     * SEL just went low. Reset our state,
		     * and print a timestamp.
		     */
		    bit_count = 0;
		    cmd_byte = 0;
		    dat_byte = 0;

		    printf("[% 4d.%03d %03d]",
			   (int) (clock_ticks / CLOCK_HZ),
			   (int) ((clock_ticks / (CLOCK_HZ / 1000)) % 1000),
			   (int) ((clock_ticks / (CLOCK_HZ / 1000000)) % 1000));
		}

		/* Did we just get an ACK? */
		if ((prev_value & ACK_MASK) && !(next_value & ACK_MASK)) {
		    printf("_");
		}

		/* Sample data on the rising edge of CLK */
		if ((next_value & CLK_MASK) && !(prev_value & CLK_MASK)) {
		    cmd_byte = (cmd_byte >> 1) | ((next_value & CMD_MASK) ? 0x80 : 0x00);
		    dat_byte = (dat_byte >> 1) | ((next_value & DAT_MASK) ? 0x80 : 0x00);
		    bit_count++;

		    if (bit_count == 8) {
			printf(" %02x/%02x", cmd_byte, dat_byte);

			bit_count = 0;
			cmd_byte = 0;
			dat_byte = 0;
		    }
		}
	    }

	    prev_value = next_value;
	    clock_ticks++;
	}
    }

    return 0;
}
