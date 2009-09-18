/*
 * hw_patch.c - Client program for the RAM patching hardware.
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
#include <string.h>
#include <assert.h>

#include "hw_patch.h"


/*
 * HWPatch_Init --
 *
 *    Initialize a patch to its default state. All registers will
 *    have dummy values.
 */

void
HWPatch_Init(HWPatch *patch)
{
   int i;

   memset(patch, 0, sizeof *patch);

   /*
    * Set all patches addresses to all ones.
    *
    * XXX: Right now the hardware will actually interpret this
    *      as a patch to the last word of memory, but in the future
    *      we might make this value special.
    */
   for (i = 0; i < PATCH_NUM_BLOCKS; i++) {
      patch->camAddrs[i] = 0xFFFFFFFF;
   }
}


/*
 * HWPatch_AllocRegion --
 *
 *    Allocate a contiguous regions of bytes in a HWPatch, splitting the region
 *    into as many CAM blocks as necessary. Returns a pointer where 'size' bytes
 *    of content may be written.
 *
 *    'size' will be automatically rounded up to a multiple of 2 bytes.
 */

uint8_t *
HWPatch_AllocRegion(HWPatch *patch, uint32_t baseAddr, uint32_t size)
{
   int contentBase;
   uint8_t *contentPtr;
   uint16_t blockOffset;

   /*
    * Allocate a new contiguous block of content memory
    */

   size = (size + 1) & ~1;
   assert((patch->contentSize & 1) == 0);
   contentBase = patch->contentSize;
   blockOffset = (uint16_t) ((contentBase - baseAddr) >> 1);
   patch->contentSize += size;

   if (patch->contentSize > PATCH_CONTENT_SIZE) {
      fprintf(stderr, "PATCH: Out of space for content (Allocated %d of %d bytes)\n",
              patch->contentSize, PATCH_CONTENT_SIZE);
      exit(1);
   }

   /*
    * Iteratively emit new patch blocks. For each block, find the
    * largest power of two that will fit our constraints, then
    * move on to the next portion of the patch region.
    *
    * Each block must be:
    *   - A power of two in size
    *   - Aligned on a multiple of its size
    *   - Contained within the allocated patch region
    */

   while (size) {
      uint32_t blockMask = 0x7fffffff;

      if (patch->numBlocks >= PATCH_NUM_BLOCKS) {
         fprintf(stderr, "PATCH: Out of blocks (patch region too complex)\n");
         exit(1);
      }

      while (blockMask >= size || (blockMask & baseAddr)) {
         assert(blockMask != 0);
         blockMask >>= 1;
      }

      fprintf(stderr, "PATCH: Block at %08x mask=%08x offset=%04x\n",
              baseAddr, blockMask, blockOffset);

      patch->camAddrs[patch->numBlocks] = baseAddr >> 1;
      patch->camMasks[patch->numBlocks] = blockMask >> 1;
      patch->blockOffsets[patch->numBlocks] = blockOffset;

      baseAddr += blockMask + 1;
      size -= blockMask + 1;
      patch->numBlocks++;
   }

   return patch->content + contentBase;
}


/*
 * HWPatch_Load --
 *
 *    Load the contents of a HWPatch into the hardware.
 *
 *    This loads the patch content, patch offsets, and
 *    CAM (Content Addressable Memory) in that order.
 */

void
HW_LoadPatch(FTDIDevice *dev, HWPatch *patch)
{
   const int numRegs = (PATCH_NUM_BLOCKS * 5 +    // CAM registers
                        PATCH_NUM_BLOCKS +        // Offset registers
                        PATCH_CONTENT_SIZE / 2);  // Content memory;
   uint16_t regAddr[numRegs];
   uint16_t regData[numRegs];
   int i, reg = 0;

   fprintf(stderr, "PATCH: Loading hardware with %d block%s, %d bytes of content\n",
           patch->numBlocks, patch->numBlocks == 1 ? "" : "s", patch->contentSize);

   for (i = 0; i < PATCH_CONTENT_SIZE/2; i++) {
      regAddr[reg] = REG_PATCH_CONTENT + i;
      regData[reg] = patch->content[i << 1] + (patch->content[1 + (i << 1)] << 8);
      reg++;
   }

   for (i = 0; i < PATCH_NUM_BLOCKS; i++) {
      regAddr[reg] = REG_PATCH_OFFSETS + i;
      regData[reg] = patch->blockOffsets[i];
      reg++;
   }

   for (i = 0; i < PATCH_NUM_BLOCKS; i++) {
      regAddr[reg] = REG_CAM_ADDR_LOW;
      regData[reg] = patch->camAddrs[i];
      reg++;

      regAddr[reg] = REG_CAM_ADDR_HIGH;
      regData[reg] = patch->camAddrs[i] >> 16;
      reg++;

      regAddr[reg] = REG_CAM_MASK_LOW;
      regData[reg] = patch->camMasks[i];
      reg++;

      regAddr[reg] = REG_CAM_MASK_HIGH;
      regData[reg] = patch->camMasks[i] >> 16;
      reg++;

      regAddr[reg] = REG_CAM_INDEX;
      regData[reg] = i;
      reg++;
   }

   assert(reg == numRegs);
   HW_ConfigWriteMultiple(dev, regAddr, regData, numRegs);
}
