/*
 * Simple R/C light dimmer.
 *
 * This connects two brightness-controlled LEDs to a single R/C
 * servo output. From minimum pulse width to maximum pulse width,
 * we sweep through a few different modes:
 *
 *   - At minimum, all lights are off
 *   - Light 1 begins to fade on
 *   - Light 1 remains on, Light 2 begins to fade on
 *   - Lights 1 and 2 are both on at full brightness
 *   - At maximum, Light 1 remains on and Light 2 begins to strobe
 *
 * In my helicopter model, I use Light 1 as a general
 * cockpit/navigation light and Light 2 as a high-power searchlight.
 *
 * Wiring:
 *   - PINB0, servo pulse input
 *   - PINB3, Light 1 output
 *   - PINB4, Light 2 output
 *
 * This is designed to run on an 8-pin ATtiny, with the internal 8 MHz
 * RC oscillator. It was tested with an ATtiny85.
 *
 * The light outputs may be connected directly to small (40 mA or
 * less) LEDs. Larger LED loads should be driven by a MOSFET with
 * a pull-down resistor on the gate.
 *
 * -- Micah Dowty <micah@navi.cx>
 *
 */

#include <avr/io.h>

#define F_CPU         8000000UL                      // 8 MHz
#define SERVO_MS      (F_CPU / 6000UL)               // Ticks per millisecond
#define SERVO_MIN     ((uint16_t)(1.1 * SERVO_MS))
#define SERVO_CENTER  ((uint16_t)(1.5 * SERVO_MS))
#define SERVO_MAX     ((uint16_t)(1.9 * SERVO_MS))
#define SERVO_PIN     PINB0

#define LIGHTMODE_OFF      0       // All lights off
#define LIGHTMODE_FADE_1   1       // Light 1 fading, Light 2 off
#define LIGHTMODE_FADE_2   2       // Light 1 on, Light 2 fading
#define LIGHTMODE_STROBE   3       // Light 1 on, Light 2 flashing

uint8_t pulse_counter      = 0;    // Global servo pulse counter, for low-frequency timing
#define STROBE_DIVISOR     _BV(1)  // Bit mask for dividing pulse_counter into our strobe frequency


/*
 * Wait indefinitely for a servo pulse. When the pulse arrives, time
 * it.  Returns a value in the same units as SERVO_MS.
 */

static uint16_t
read_pulse(void)
{
    register uint16_t result asm("r24");
    uint8_t temp;

    asm volatile (

	/* Zero result */
	"clr %A0 \n\t"
	"clr %B0 \n\t"

	/* Wait for SERVO_PIN to go high (Accurate to 4 clock cycles) */
	"L_1%=: \n\t"
	"in %1, %2 \n\t"
        "andi %1, %3 \n\t"
	"breq L_1%= \n\t"

	/*
	 * Until it goes low, incrment result.  As long as SERVO_PIN
	 * is high, this increments 'result' every 6 clock cycles.
	 */
	"L_2%=: \n\t"
	"in %1, %2 \n\t"         // 1 clock
	"adiw %A0, 1 \n\t"       // 2 clocks
        "andi %1, %3 \n\t"       // 1 clock
	"brne L_2%= \n\t"        // 2 clocks

	/* Outputs */
        : "=r" (result),
	  "=d" (temp)

	/* Inputs */
        : "I" (_SFR_IO_ADDR(PINB)),
	  "I" (_BV(SERVO_PIN))
        );

    pulse_counter++;
    return result;
}


/*
 * Set up outputs for a particular light mode and fade step.
 *
 * We actually control both lights with Timer1's PWM module, so we can
 * only have a single light fading at once and we need to pull some
 * tricks to ensure the non-fading light is in the correct state.
 *
 * It would have been simpler to use separate PWM compare outputs for
 * each LED, but this configuration makes in-circuit programming
 * easier :)
 */

static void
set_light_state(uint8_t mode, uint8_t fade_step)
{
    switch (mode) {
	
    case LIGHTMODE_OFF:
    default:
	/*
	 * Float the light output pins
	 */
	PORTB = 0;
	DDRB = 0;
	GTCCR = 0;
	break;
	
    case LIGHTMODE_STROBE:
	/*
	 * Drive both lights, toggle Light 2
	 */
	DDRB = _BV(PINB3) | _BV(PINB4);
	GTCCR = 0;
	if (pulse_counter & STROBE_DIVISOR)
	    PORTB = _BV(PINB3) | _BV(PINB4);
	else
	    PORTB = _BV(PINB3);
	break;

    case LIGHTMODE_FADE_1:
	/*
	 * Disable Light 2 (OC1B) and fade Light 1 (!OC1B).
	 * We can't do this using the Timer1 PWM mode
	 * alone, so we need to float Light 2 using
	 * DDRB.
	 */
	DDRB = _BV(PINB3);
	GTCCR = _BV(PWM1B) | _BV(COM1B0);
	OCR1C = 0xFF;
	OCR1B = 0xFF - fade_step;
	break;

    case LIGHTMODE_FADE_2:
	/*
	 * Turn Light 1 (!OC1B) full-on and fade Light 2 (OC1B).  To
	 * do this, we disable !OC1B as a PWM output and set it high
	 * using PORTB.
	 */
	DDRB = _BV(PINB3) | _BV(PINB4);
	PORTB = _BV(PINB3);
	GTCCR = _BV(PWM1B) | _BV(COM1B1);
	OCR1C = 0xFF;
	OCR1B = fade_step;
	break;
    }
}


/*
 * Initialize light-related hardware.
 * This sets the Timer1 PWM frequency, and initializes I/O directions.
 */

static void
init_lights(void)
{
    TCCR1 = _BV(CTC1) | _BV(CS10);      // Use system clock with no divider. (31 kHz PWM)
    set_light_state(LIGHTMODE_OFF, 0);
}


int main(void)
{
    init_lights();

    while (1) {
	uint16_t pulse = read_pulse();

	if (pulse <= SERVO_MIN) {
	    set_light_state(LIGHTMODE_OFF, 0);
	} else if (pulse >= SERVO_MAX) {
	    set_light_state(LIGHTMODE_STROBE, 0);
	} else if (pulse <= SERVO_CENTER) {
	    set_light_state(LIGHTMODE_FADE_1, ((uint32_t) pulse - SERVO_MIN) * 0xFF / (SERVO_CENTER - SERVO_MIN));
	} else {
	    set_light_state(LIGHTMODE_FADE_2, ((uint32_t) pulse - SERVO_CENTER) * 0xFF / (SERVO_MAX - SERVO_CENTER));
	}
    }

    return 0;
}
