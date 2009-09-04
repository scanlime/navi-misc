/*
 * Old decoder for the 3-byte packet format.
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

const int RL = 4;   // Read latency, set in the CR on startup
const int WL = 3;   // Write latency, automatically RL-1.


int main(void)
{
   uint32_t addr = 0;
   int burst_cnt = 0;
   int skip_cycles = 0;

   FILE *memcontent = fopen("mem-content.bin", "wb");
   if (!memcontent) {
      perror("open");
      return 1;
   }

   while (!feof(stdin)) {
      uint8_t packet[3];

      if (fread(packet, sizeof packet, 1, stdin) != 1) {
         perror("read");
         return 1;
      }

      if (packet[0] & 0x80) {
         /* Data packet */

         if (skip_cycles) {
            /* Skip data cycles that fall within the Write Latency. */
            skip_cycles--;
         } else {

            uint16_t data = (packet[1] << 8) | packet[2];
            const char *label;
            int width;
            uint32_t va = addr << 1;

            switch (packet[0]) {

            case 0xA0:
               label = "w_word";
               width = 2;
               break;

            case 0xA1:
               label = "w_upper";
               va++;
               width = 1;
               break;

            case 0xA2:
               label = "w_lower";
               width = 1;
               break;

            case 0xA3:
               label = "w_none";
               width = 0;
               break;

            default:
               label = "w_ERROR";
               width = 0;
               break;
            }

            printf("0x%08x = 0x%04x %c%c %s b=%d\n", addr, data,
                   isprint(data & 0xFF) ? data & 0xFF : '.',
                   isprint(data >> 8) ? data >> 8 : '.',
                   label, burst_cnt);

            fseek(memcontent, va, SEEK_SET);
            fwrite(&data, width, 1, memcontent);

            addr++;
            burst_cnt++;
         }

      } else {
         /* Address packet */
         addr = (packet[0] << 16) | (packet[1] << 8) | packet[2];
         burst_cnt = 0;
         skip_cycles = WL-1;
      }
    }

    return 0;
}
