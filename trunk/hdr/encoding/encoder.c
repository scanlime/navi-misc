#include "pix.h"

/* Encoding:
 * 4x4 pixel blocks, 16 bits usable storage per block.
 * Red channel holds content, one bit per pixel in LSB.
 * Green channel LSB is always (red ^ blue ^ 1).
 * Blue is used for alignment- the top-left pixel has
 * its LSB high, all others have their LSB low.
 */

#define BLOCKSIZE 4

void encode_block(unsigned char *data, int stride, int content) {
  int x, y;

  for (y=0; y<BLOCKSIZE; y++)
    for (x=0; x<BLOCKSIZE; x++) {
      unsigned char *pixel = data + x*3 + y*stride;
      int r,g,b;

      r = (content & 0x8000) ? 1 : 0;
      content <<= 1;

      b = (x==0 && y==0) ? 1 : 0;
      g = r ^ b ^ 1;

      pixel[0] = (pixel[0] & 0xFE) | r;
      pixel[1] = (pixel[1] & 0xFE) | g;
      pixel[2] = (pixel[2] & 0xFE) | b;
    }
}

int main() {
  struct pixmap* p = read_pixmap("original.ppm");
  struct pixmap* subband = read_pixmap("original_subband.ppm");
  int x, y;
  int content;

  for (y=0; y<=p->height-BLOCKSIZE; y+=BLOCKSIZE)
    for (x=0; x<=p->width-BLOCKSIZE; x+=BLOCKSIZE) {

      /* Extract a sample from our subband image, for the top-left corner of this block.
       * We can store 16-bit data, but for testing it's easier to use 8-bit.
       */
      content = subband->data[x*3 + y*subband->width*3 + 1];

      encode_block(p->data + (x*3 + y*p->width*3), p->width*3, content);
    }

  write_pixmap("encoded.ppm", p);
  return 0;
}
