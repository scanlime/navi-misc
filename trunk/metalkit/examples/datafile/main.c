/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: How to include compressed data files,
 * and decompress them using the 'puff' module.
 */

#include "types.h"
#include "vgatext.h"
#include "datafile.h"
#include "intr.h"

DECLARE_DATAFILE(myFile, sample_txt_z);

static uint8 output_buffer[64*1024];

int
main(void)
{
   uint32 len;

   Intr_Init();
   Intr_SetFaultHandlers(VGAText_DefaultFaultHandler);

   VGAText_Init();

   len = DataFile_Decompress(myFile, output_buffer, sizeof output_buffer);
   output_buffer[len] = '\0';

   VGAText_WriteString(output_buffer);

   return 0;
}
