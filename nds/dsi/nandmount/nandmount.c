/*
 * nandmount -- FUSE filesystem for transparent encryption/decryption
 *              and partition table support for DSi NAND flash images
 *              or physical flash block devices.
 *
 *   The DSi's NAND flash is encrypted with AES in CTR mode, but
 *   otherwise it's a normal DOS-style partition with an MBR and
 *   multiple FAT partitions. This tool doesn't perform the actual
 *   AES encryption, but it does allow transparent access to a
 *   NAND flash image if we're given a precalculated 'pad' file
 *   with the AES-encrypted values that are XOR'ed with each byte
 *   of the flash memory.
 *
 *   This tool can operate on a physical block device (the actual
 *   flash chip mounted with a USB MMC card reader, for example)
 *   or on a plain file (a memory dump).
 *
 *   The output is a filesystem with one file for each plaintext
 *   partition in the NAND flash. These files can be inspected
 *   at the block level, or they can be mounted as FAT filesystems
 *   using the loopback device.
 *
 *   XXX: This is kind of a quick hack. Little error handling, and
 *        it isn't designed to be portable.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define FUSE_USE_VERSION  26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

#define BLOCK_SIZE      512
#define NUM_PARTITIONS  4
#define BUFFER_SIZE     (128 * 1024)

#define MIN(a, b)  ((a) > (b) ? (b) : (a))
#define PACKED     __attribute__ ((packed))

int xorFd, nandFd;

/*
 * Partition table
 */

static struct {
   uint8_t code[440];
   uint8_t disksig[4];
   uint8_t nul[2];
   struct {
      uint8_t status;
      uint8_t chsFirst[3];
      uint8_t type;
      uint8_t chsLast[3];
      uint32_t lbaFirst;
      uint32_t lbaSize;
   } PACKED partitions[NUM_PARTITIONS];
   uint8_t mbrsig[2];
} PACKED MBR;


/*******************************************************************
 * Block-level XOR layer
 */

static void
block_xor(const uint8_t *in1, const uint8_t *in2, uint8_t *out, size_t size)
{
   while (size--)
      *(out++) = *(in1++) ^ *(in2++);
}

static int
xor_read(uint8_t *buffer, size_t size, off_t offset)
{
   int result;
   uint8_t bufXor[BUFFER_SIZE];
   uint8_t bufNand[BUFFER_SIZE];

   size = MIN(size, BUFFER_SIZE);

   result = pread(nandFd, bufNand, size, offset);
   pread(xorFd, bufXor, size, offset);

   if (result > 0)
      block_xor(bufXor, bufNand, buffer, size);

   return result;
}

static int
xor_write(const uint8_t *buffer, size_t size, off_t offset)
{
   int result;
   uint8_t bufXor[BUFFER_SIZE];
   uint8_t bufNand[BUFFER_SIZE];

   size = MIN(size, BUFFER_SIZE);

   pread(xorFd, bufXor, size, offset);
   block_xor(bufXor, buffer, bufNand, size);

   return pwrite(nandFd, bufNand, size, offset);
}


/*******************************************************************
 * FUSE operations
 */

/*
 * Format the path to a partition. returns true if the partition is
 * valid, false if it has no file.
 */
static bool
format_partition_path(int index, char *buf, size_t bufSize, bool absolute)
{
   snprintf(buf, bufSize, "%spart%d_type%02x",
            absolute ? "/" : "", index, MBR.partitions[index].type);
   buf[bufSize - 1] = '\0';
   return MBR.partitions[index].lbaSize != 0;
}

static int
fs_getattr(const char *path, struct stat *stbuf)
{
   int i;

   memset(stbuf, 0, sizeof *stbuf);
   stbuf->st_uid = getuid();
   stbuf->st_gid = getgid();

   if (!strcmp(path, "/")) {
      // Root is our only directory
      stbuf->st_mode = S_IFDIR | 0755;
      stbuf->st_nlink = 2;
      return 0;
   }

   // Is it a partition file?
   for (i = 0; i < NUM_PARTITIONS; i++) {
      char partName[PATH_MAX];
      if (format_partition_path(i, partName, sizeof partName, true)
          && !strcmp(path, partName)) {

         stbuf->st_mode = S_IFREG | 0600;
         stbuf->st_nlink = 1;
         stbuf->st_size = MBR.partitions[i].lbaSize * BLOCK_SIZE;
         return 0;
      }
   }

   return -ENOENT;
}

static int
fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
           off_t offset, struct fuse_file_info *fi)
{
   int i;

   if (strcmp(path, "/")) {
      // Root is our only directory
      return -ENOENT;
   }

   filler(buf, ".", NULL, 0);
   filler(buf, "..", NULL, 0);

   for (i = 0; i < NUM_PARTITIONS; i++) {
      char partName[PATH_MAX];
      if (format_partition_path(i, partName, sizeof partName, false)) {
         filler(buf, partName, NULL, 0);
      }
   }

   return 0;
}

static int
fs_open(const char *path, struct fuse_file_info *fi)
{
   int i;

   for (i = 0; i < NUM_PARTITIONS; i++) {
      char partName[PATH_MAX];
      if (format_partition_path(i, partName, sizeof partName, true)
          && !strcmp(path, partName)) {
         return 0;
      }
   }

   return -ENOENT;
}

static int
fs_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
   int i;

   for (i = 0; i < NUM_PARTITIONS; i++) {
      char partName[PATH_MAX];

      if (format_partition_path(i, partName, sizeof partName, true)
          && !strcmp(path, partName)) {

         uint32_t partsize = MBR.partitions[i].lbaSize * BLOCK_SIZE;
         size = MIN(size, partsize - offset);
         offset += MBR.partitions[i].lbaFirst * BLOCK_SIZE;

         return xor_read(buf, size, offset);
      }
   }

   return -ENOENT;
}

static int
fs_write(const char *path, const char *buf, size_t size, off_t offset,
         struct fuse_file_info *fi)
{
   int i;

   for (i = 0; i < NUM_PARTITIONS; i++) {
      char partName[PATH_MAX];

      if (format_partition_path(i, partName, sizeof partName, true)
          && !strcmp(path, partName)) {

         uint32_t partsize = MBR.partitions[i].lbaSize * BLOCK_SIZE;
         size = MIN(size, partsize - offset);
         offset += MBR.partitions[i].lbaFirst * BLOCK_SIZE;

         return xor_write(buf, size, offset);
      }
   }

   return -ENOENT;
}

static const struct fuse_operations fsops = {
   .getattr   = fs_getattr,
   .readdir = fs_readdir,
   .open   = fs_open,
   .read   = fs_read,
   .write  = fs_write,
};


/*******************************************************************
 * Main Program
 */

int
main(int argc, char **argv)
{
   struct fuse_args args = FUSE_ARGS_INIT(0, NULL);
   int argNum = 0;
   int i;

   for (i = 0; i < argc; i++) {
      if (i > 0 && argNum < 2 && argv[i][0] != '-') {
         /* This is one of our new options- eat it. */

         if (argNum == 0) {
            xorFd = open(argv[i], O_RDONLY);
            if (xorFd < 0) {
               perror("xor pad file");
               return 1;
            }
         } else if (argNum == 1) {
            nandFd = open(argv[i], O_RDWR);
            if (nandFd < 0) {
               perror("NAND image file");
               return 1;
            }
         }

         argNum++;

      } else {
         /* FUSE argument */
         fuse_opt_add_arg(&args, argv[i]);
      }
   }

   // Allow root to access our files, otherwise loopback mounts won't work
   fuse_opt_add_arg(&args, "-o");
   fuse_opt_add_arg(&args, "allow_root");

   if (argNum != 2) {
      fprintf(stderr,
              "Usage: %s [FUSE options] <xor pad> <nand image/device> <mount point>\n"
              "  Mounts a NAND image as a filesystem in which each decrypted partition\n"
              "  is a separate file, individually mountable via the loopback device.\n",
              argv[0]);
      return 1;
   }

   assert(sizeof MBR == BLOCK_SIZE);
   if (xor_read((uint8_t*) &MBR, sizeof MBR, 0) != sizeof MBR) {
      fprintf(stderr, "Error reading MBR sector. Is file empty?\n");
      return 1;
   }

   if (MBR.mbrsig[0] != 0x55 || MBR.mbrsig[1] != 0xAA) {
      fprintf(stderr, "Error: MBR signature invalid. Is XOR pad correct?\n");
      return 1;
   }

   return fuse_main(args.argc, args.argv, &fsops, NULL);
}
