/*
 * an arduino sketch to interface with a ps/2 keyboard.
 * Also uses serial protocol to talk back to the host
 * and report what it finds. Used the ps2 library.
 */

#include <ps2.h>

/*
 * Pin 5 is the ps2 data pin, pin 6 is the clock pin
 * Feel free to use whatever pins are convenient.
 */

PS2 kbd(6, 5);

void kbd_init()
{
  char ack;

  kbd.write(0xff);  // send reset code
  ack = kbd.read();  // byte, kbd does self test
  ack = kbd.read();  // another ack when self test is done
}

void setup()
{
  Serial.begin(9600);
  kbd_init();
}

/*
 * get a keycode from the kbd and report it back to the
 * host via the serial line.
 */
void loop()
{
  unsigned char code;
  
  for (;;) { /* ever */  
    /* read a keycode */
    code = kbd.read();
    /* send the data back up */
    Serial.println(code, HEX);
   // delay(20);  /* twiddle */
  }
}
