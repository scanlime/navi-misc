/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: How to include compressed data files,
 * and decompress them using the 'puff' module.
 */

#include "types.h"
#include "vgatext.h"
#include "puff.h"
#include "intr.h"

extern uint8 _binary_sample_txt_z_start[];
extern uint8 _binary_sample_txt_z_end[];
extern uint8 _binary_sample_txt_z_size[];

static uint8 output_buffer[64*1024];
uint32 output_size;

int
main(void)
{
   Intr_Init();
   Intr_SetFaultHandlers(VGAText_DefaultFaultHandler);

   VGAText_Init();

   unsigned long destlen = sizeof output_buffer;
   unsigned long sourcelen = (uint32) _binary_sample_txt_z_size;

   if (puff(output_buffer, &destlen, _binary_sample_txt_z_start, &sourcelen)) {
      VGAText_Panic("Decompression error!");
   }

   output_buffer[destlen] = '\0';

   VGAText_WriteString(output_buffer);

   return 0;
}
