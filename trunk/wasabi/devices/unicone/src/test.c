#include "bit_file.h"

int main()
{
  struct bitfile* bf = bitfile_new_from_path("fpga/test/test.bit");
  if (!bf) {
    perror("bitfile_new_from_path");
    return 1;
  }

  printf("NCD file    : %s\n", bf->ncd_filename);
  printf("Part number : %s\n", bf->part_number);
  printf("Date        : %s\n", bf->date);
  printf("Time        : %s\n", bf->time);
  printf("Length      : %d bytes\n", bf->length);

  return 0;
}
