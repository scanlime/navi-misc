#include <stdio.h>
#include <malloc.h>

struct pixmap {
  int width, height;
  unsigned char *data;
};

__inline__ struct pixmap* new_pixmap(int width, int height) {
  struct pixmap *pix = malloc(sizeof(struct pixmap));
  pix->width = width;
  pix->height = height;
  pix->data = malloc(pix->width * pix->height * 3);
  memset(pix->data, 0, pix->width * pix->height * 3);
  return pix;
}

__inline__ struct pixmap* read_pixmap(unsigned char *name) {
  struct pixmap *pix = malloc(sizeof(struct pixmap));
  unsigned char line[256];
  FILE* f = fopen(name, "rb");
  unsigned char *pixels;
  if (!f) {
    printf("Error opening file %s\n", name);
    exit(1);
  }
  fgets(line, sizeof(line), f);
  do {
    fgets(line, sizeof(line), f);
  } while (line[0] == '#');
  sscanf(line, "%d %d", &pix->width, &pix->height);
  fgets(line, sizeof(line), f);
  pix->data = malloc(pix->width * pix->height * 3);
  fread(pix->data, 3, pix->width * pix->height, f);
  fclose(f);
  return pix;
}



__inline__ void write_pixmap(unsigned char *name, struct pixmap* pix) {
  FILE* f = fopen(name, "wb");
  if (!f) {
    printf("Error opening file %s\n", name);
    exit(1);
  }
  fprintf(f, "P6\n%d %d\n255\n", pix->width, pix->height);
  fwrite(pix->data, 3, pix->width * pix->height, f);
  fclose(f);
}
