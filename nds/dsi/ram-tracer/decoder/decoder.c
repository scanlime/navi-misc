/*
 * decoder.c - Simple command-line decoder for memory trace logs.
 *             (Only for the new 32-bit log format)
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include "memtrace.h"


int
main(int argc, char **argv)
{
   static MemTraceState state;
   MemTraceResult result;
   MemOp op;
   const char *memImageFile = NULL;
   bool quiet = false;
   bool limit = false;
   double limit_time = 0.0;

   /*
    * Command line gook...
    */

   if (argc < 2 || argc > 4) {
      fprintf(stderr,
              "\n"
              "RAM Trace Decoder, for new 32-bit trace logs.\n"
              "-- Micah Dowty <micah@navi.cx>\n"
              "\n"
              "usage: %s <trace.raw> [<mem-image.bin>  [limit_time] ]\n"
              "\n", argv[0]);
      return 1;
   }
   if (argc >= 3) {
      memImageFile = argv[2];
      quiet = true;
   }

   if (argc >= 4) {
      limit = true;
      errno = 0;
      limit_time = strtod(argv[3], NULL);
      if(errno != 0) {
         perror("strtod");
         return -1;
      }
   }

   if (!MemTrace_Open(&state, argv[1])) {
      perror("open");
      return 1;
   }

   /*
    * Main loop- ask MemTrace to fetch us one burst at a time.
    */

   while ((result = MemTrace_Next(&state, &op)) != MEMTR_EOF) {
      if (result != MEMTR_SUCCESS) {
	 fprintf(stderr, "*** Error at offset %llx: %s\n", state.fileOffset,
		 MemTrace_ErrorString(result));
         continue;
      }

      if(limit && state.timestamp.seconds > limit_time) {
         fprintf(stderr, "Exiting per user request before entry @ %11.06f\n", state.timestamp.seconds);
         result = MEMTR_EOF;
         break;
      }

      if (!quiet) {
         const char *label;
         int i;

         assert(op.type == MEMOP_READ || op.type == MEMOP_WRITE);
         label = op.type == MEMOP_WRITE ? "WRITE" : "read";

         printf("%11.06fs %-5s [%2d] %08x: ", state.timestamp.seconds,
                label, op.length, op.addr);

         for (i = 0; i < op.length || i < 32; i++) {
            const char *pad = (i & 1) ? "" : " ";
            if (i < op.length)
               printf("%s%02x", pad, state.memory[op.addr + i]);
            else
               printf("%s  ", pad);
         }

         printf("  ");

         for (i = 0; i < op.length || i < 32; i++) {
            char c = i < op.length ? (char)state.memory[op.addr + i] : ' ';
            printf("%c", isprint(c) ? c : '.');
         }

         printf("\n");
      }
   }

   /*
    * Finished successfully. Write out a memory image, if we were asked to.
    */

   if (memImageFile) {
      FILE *img = fopen(memImageFile, "wb");

      if (!img) {
         perror("open");
         return 1;
      }

      if (fwrite(state.memory, sizeof state.memory, 1, img) != 1) {
         perror("write");
         return 1;
      }

      fclose(img);
   }

   return 0;
}
