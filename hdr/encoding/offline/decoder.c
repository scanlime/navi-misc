#include "pix.h"

#define BLOCKSIZE 4

/* Returns the 16-bit unsigned content if a valid
 * block was found, or -1 if it couldn't be decoded.
 */
int decode_block(unsigned char *data, int stride) {
  int x, y;
  int content = 0;

  for (y=0; y<BLOCKSIZE; y++)
    for (x=0; x<BLOCKSIZE; x++) {
      unsigned char *pixel = data + x*3 + y*stride;
      int r,g,b;

      r = pixel[0] & 1;
      g = pixel[1] & 1;
      b = pixel[2] & 1;

      if (b != (x==0 && y==0) ? 1 : 0)
	return -1;

      if (g != r^b^1)
	return -1;

      content <<= 1;
      content |= r;
    }
  return content;
}

void fill_block(unsigned char *data, int stride, int r, int g, int b) {
  int x, y;

  for (y=0; y<BLOCKSIZE; y++)
    for (x=0; x<BLOCKSIZE; x++) {
      unsigned char *pixel = data + x*3 + y*stride;
      pixel[0] = r;
      pixel[1] = g;
      pixel[2] = b;
    }
}

int main() {
  struct pixmap* p = read_pixmap("sshot.ppm");
  struct pixmap* output = new_pixmap(p->width, p->height);
  unsigned char *pixel = p->data;
  unsigned char *outpixel;
  int r,g,b, x,y;

  outpixel = output->data;
  for (y=p->height; y; y--)
    for (x=p->width; x; x--) {
      outpixel[0] = 127;
      outpixel[1] = 0;
      outpixel[2] = 0;
      outpixel += 3;
    }

  outpixel = output->data;
  for (y=p->height-4; y; y--) {
    for (x=p->width; x; x--) {
      int content = decode_block(pixel, p->width*3);

      if (content >= 0) {
	int gray = content;
	fill_block(outpixel, p->width*3, gray, gray, gray);
      }

      pixel += 3;
      outpixel += 3;
    }
}

  write_pixmap("lsb_map.ppm", output);
  return 0;
}
