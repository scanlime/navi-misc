/*
 * Comodore 64 USB Interface
 * Copyright (C) 2005 Micah Dowty
 *
 * This program is based in part on the IEC-ATA project,
 * Copyright (C) 2002 Asbjørn Djupdal, also released
 * under the GNU General Public License.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include "regezusb.h"

typedef unsigned char bool;

/*********************************************/

void led_init() {
   PORTCCFG = 0;
   OUTC = 0x00;
   OEC = 0x08;
}

void led_set(bit on) {
   if (on)
      OUTC = 0x00;
   else
      OUTC = 0x08;
}

/*********************************************/

/*
 * Normally we never reset the timer, we just remember
 * where we left off. This means we end up ignoring
 * all time that passes outside a Timer2_Elapse method.
 */
unsigned short timer2_mark;

/* Ticks are 0.5us, since tmr2 is set up at 2 MHz */
#define US *2

void timer2_reset() {
   /* Run at 2 MHz (default compatibility-mode CLK24/12) */
   CKCON = 0x00;

   /* Stop */
   T2CON = 0x00;

   /* Reset everything while stopped */
   RCAP2H = 0;
   RCAP2L = 0;
   TH2 = 0;
   TL2 = 0;
   timer2_mark = 0;

   /* Start */
   T2CON = 0x04;
}

unsigned short timer2_read() {
   unsigned char high, low;

   /*
    * Retry if it rolled over while
    * we were reading it.
    */
   do {
      high = TH2;
      low = TL2;
   } while (high != TH2);

   return (high << 8) | low;
}

void timer2_elapse_ticks(unsigned short ticks) {
   timer2_mark += ticks;
   while ((timer2_mark - timer2_read()) < 0x8000);
}

void timer2_elapse_ms(unsigned int milliseconds) {
   while (milliseconds--) {
      timer2_elapse_ticks(1000 US);
   }
}

/*********************************************/

#define IEC_PORT_PINS PINSA
#define IEC_PORT_OE   OEA

#define IEC_RESET  0x10
#define IEC_DATA   0x20
#define IEC_CLK    0x40
#define IEC_ATN    0x80

#define IEC_CMD_LISTEN    0x20
#define IEC_CMD_UNLISTEN  0x30
#define IEC_CMD_TALK      0x40
#define IEC_CMD_UNTALK    0x50
#define IEC_CMD_DATA      0x60
#define IEC_CMD_CLOSE     0xE0
#define IEC_CMD_OPEN      0xF0

#define iec_pin_is_high(mask)         ((IEC_PORT_PINS & (mask)) != 0)
#define iec_pin_is_low(mask)          ((IEC_PORT_PINS & (mask)) == 0)
#define iec_pin_release(mask)         do { IEC_PORT_OE &= ~mask; } while (0)
#define iec_pin_pull_down(mask)       do { IEC_PORT_OE |= mask; } while (0)

struct iec_command_t {
   /* Unpacked version of the data sent during ATN */
   unsigned char device, secondary_address;
   unsigned char command, secondary_command;

   /* Should this command have TALK and/or LISTEN phases? */
   bool need_listen, need_talk;
};

void iec_init() {
   PORTACFG = 0;
   OUTA = 0x00;
   OEA = 0x00;
}

bool iec_pin_wait(unsigned char mask, bit high) {
   timer2_reset();
   while (iec_pin_is_high(mask) != high) {
      if (timer2_read() > 10000 US)
         return 0;
   }
   return 1;
}

void iec_delay() {
   /* C64 timing: 60 microseconds */
   timer2_reset();
   timer2_elapse_ticks(80 US);
}

bool iec_read_byte(char *result) {
   bit eoi = 1;
   unsigned char i, byte;

   /* We're ready, wait until everyone is */
   iec_pin_release(IEC_DATA);
   if (!iec_pin_wait(IEC_DATA, 1))
      goto error;

   /* Wait for the talker to pull CLK low. Normally
    * this happens within 200us. If it doesn't, this
    * signals EOI (End Of Indicator) and we time out.
    */
   timer2_reset();
   while (timer2_read() < 200 US) {
      if (iec_pin_is_low(IEC_CLK)) {
         eoi = 0;
         break;
      }
   }

   /* For EOI, we have to acknowledge by pulsing DATA
    * then going back to waiting for CLK to go low.
    */
   if (eoi) {
      iec_pin_pull_down(IEC_DATA);
      iec_delay();
      iec_pin_release(IEC_DATA);
      if (!iec_pin_wait(IEC_CLK, 0))
         goto error;
   }

   byte = 0;
   for (i=8; i; i--) {
      byte >>= 1;

      /* Receive bits on CLK's rising edge */
      if (!iec_pin_wait(IEC_CLK, 1))
         goto error;
      if (iec_pin_is_high(IEC_DATA))
         byte |= 0x80;
      if (!iec_pin_wait(IEC_CLK, 0))
         goto error;
   }
   *result = byte;

   /* Acknowledge the byte */
   iec_pin_pull_down(IEC_DATA);

   return eoi;

 error:
   *result = 0;
   return 1;
}

bool iec_write_byte(char byte, bool eoi) {
   unsigned char i;

   if (eoi) {
      /* Wait for the listener to acknoledge EOI */
      if (!iec_pin_wait(IEC_DATA, 0))
         return 0;
      if (!iec_pin_wait(IEC_DATA, 1))
         return 0;
   }

   iec_delay();
   iec_pin_pull_down(IEC_CLK);

   for (i=8; i; i--) {
      iec_delay();

      if (byte & 1)
         iec_pin_release(IEC_DATA);
      else
         iec_pin_pull_down(IEC_DATA);

      iec_pin_release(IEC_CLK);
      iec_delay();
      iec_pin_pull_down(IEC_CLK);
      iec_pin_release(IEC_DATA);

      byte >>= 1;
   }

   /* Wait for the listener to acknowledge */
   if (!iec_pin_wait(IEC_DATA, 0))
      return 0;

   return 1;
}

void iec_next_command(struct iec_command_t *cmd_info) {
   unsigned char length = 0;
   char buffer[2];

   /* End the previous transaction */
   iec_pin_release(IEC_DATA);

   /* Wait for ATN to be asserted by the host.
    * Use the LED as a 'ready' indicator.
    */
   led_set(1);
   while (iec_pin_is_high(IEC_ATN));
   led_set(0);

   /* Acknowledge ATN */
   iec_delay();
   iec_pin_release(IEC_CLK);
   iec_pin_pull_down(IEC_DATA);

   /* As long as ATN stays down, receive a byte each time CLK is released */
   while (iec_pin_is_low(IEC_ATN)) {
      if (iec_pin_is_high(IEC_CLK) && length < sizeof(buffer)) {
         iec_read_byte(buffer + length);
         length++;
      }
   }

   /* Parse the ATN packet we just got */
   cmd_info->command = buffer[0] & 0xF0;
   cmd_info->device = buffer[0] & 0x0F;
   cmd_info->secondary_address = buffer[1] & 0x0F;
   cmd_info->secondary_command = buffer[1] & 0xF0;

   cmd_info->need_listen = 0;
   cmd_info->need_talk = 0;

   switch (cmd_info->secondary_command) {
   case IEC_CMD_OPEN:
   case IEC_CMD_DATA:
      if (cmd_info->command == IEC_CMD_LISTEN)
         cmd_info->need_listen = 1;
      else if (cmd_info->command == IEC_CMD_TALK)
         cmd_info->need_talk = 1;
      break;
   }

   if (cmd_info->need_talk) {
      /* Turn-around sequence, switch from listening to talking */
      iec_pin_wait(IEC_CLK, 1);
      iec_pin_release(IEC_DATA);
      iec_delay();
      iec_pin_pull_down(IEC_CLK);
      iec_delay();
   }
}

unsigned char iec_listen(char *buffer, unsigned char bufferLen) {
   unsigned char eoi = 0, length = 0;

   while (length < bufferLen && eoi == 0) {
      if (!iec_pin_wait(IEC_CLK, 1))
         break;

      eoi = iec_read_byte(buffer + length);
      length++;
   }

   return length;
}

unsigned char iec_talk(char *buffer, unsigned char length, bool eoi) {
   unsigned char written = 0;
   bit last = 0;

   while (!last) {
      last = written+1 >= length;

      /* Ready to send */
      iec_pin_release(IEC_CLK);

      /* Wait for the listener */
      if (!iec_pin_wait(IEC_CLK, 1))
         break;

      if (!iec_write_byte(buffer[written], eoi && last))
         break;
      written++;
   }

   if (eoi) {
      iec_delay();
      iec_pin_release(IEC_CLK);
   }

   return written;
}

/*********************************************/

struct usb_response_t {
   struct iec_command_t cmd;
   unsigned char length;
};

void main()
{
   led_init();
   iec_init();

   while (1) {
      struct usb_response_t *response = IN2BUF;

      iec_next_command(&response->cmd);
      response->length = 0;

      if (response->cmd.need_listen)
         response->length = iec_listen(((char*)response) +
                                       sizeof(struct usb_response_t), 32);

      if (response->cmd.need_talk) {
         static const unsigned char program[] = {
            0x01, 0x08,

            0x00, 0x00,

            /*
            0x11, 0x08, 0x0A, 0x00,
            'H', 'E', 'L', 'L', 'O', ' ', 'Y', 'O', 'S', 'H', 'I',
            0x00, 0x00, 0x00,
            */
         };
         response->length = iec_talk(program, sizeof(program), 1);
      }

      IN2BC = sizeof(struct usb_response_t) +
         (response->cmd.need_listen ? response->length : 0);
      while (IN2CS & 0x02);
   }
}

/* The End */
