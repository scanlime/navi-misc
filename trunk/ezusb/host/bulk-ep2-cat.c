#include <linux/ioctl.h>
#include <linux/usbdevice_fs.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv) {
   int fd = open(argv[1], O_RDWR);
   struct usbdevfs_bulktransfer bulk;
   struct usbdevfs_setinterface seti;
   unsigned char buffer[4096];
   int i, j;

   if (fd < 0) {
      perror("open");
      return 1;
   }

   seti.interface = 0;
   seti.altsetting = 1;

   i = ioctl(fd, USBDEVFS_SETINTERFACE, &seti);
   if (i < 0) {
      perror("USBDEVFS_SETINTERFACE");
      return 1;
   }

   while (1) {
      bulk.ep = 0x82;
      bulk.len = sizeof(buffer);
      bulk.timeout = 100;
      bulk.data = buffer;

      i = ioctl(fd, USBDEVFS_BULK, &bulk);
      if (i > 0) {
         printf("[%d]", i);

         for (j=0; j<i; j++)
            printf(" %02X", buffer[j]);

         printf(" [");
         for (j=0; j<i; j++)
            if (isprint(buffer[j]))
               printf("%c", buffer[j]);
            else
               printf(".");
         printf("]\n");
      }
   }

   return 0;
}
