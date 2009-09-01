#include <stdio.h>
#include <stdint.h>

int main(void)
{
   uint32_t addr = 0;
   uint16_t data = 0;
   uint32_t sync_offset = 0;

   FILE *memcontent = fopen("mem-content.bin", "wb");
   if (!memcontent) {
      perror("open");
      return 1;
   }

   /*
    * Look for the first sync byte.
    * XXX: Huge hack!
    */

   while (!feof(stdin) && fgetc(stdin) != 0xAA)
      sync_offset++;
   fgetc(stdin); fgetc(stdin);
   fprintf(stderr, "Sync at offset %d\n", sync_offset);

   while (!feof(stdin)) {
      uint8_t packet[3];

      if (fread(packet, sizeof packet, 1, stdin) != 1) {
         perror("read");
         return 1;
      }

      if (packet[0] & 0x80) {
         /* Data packet */

         if (packet[0] != 0xaa) {
            fprintf(stderr, "Data sync byte incorrect!\n");
            return 1;
         }

         data = (packet[1] << 8) | packet[2];

         printf("0x%08x = 0x%04x\n", addr, data);

         fseek(memcontent, addr << 1, SEEK_SET);
         fwrite(&data, sizeof data, 1, memcontent);

         addr++;

      } else {
         /* Address packet */
         addr = (packet[0] << 16) | (packet[1] << 8) | packet[2];
      }
    }

    return 0;
}
