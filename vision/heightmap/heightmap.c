#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <math.h>

int main(int argc, char **argv) {
  GdkPixbuf *left, *right, *output;
  guchar *lp, *rp, *out;
  float *dist_buffer;
  float dist_max = -G_MINFLOAT;
  float dist_min = G_MAXFLOAT;
  int width, height, stride;
  int filter_width;
  int x, y;

  g_type_init();

  /* Create input and output images */
  left = gdk_pixbuf_new_from_file("stereo-test-left.png", NULL);
  right = gdk_pixbuf_new_from_file("stereo-test-right.png", NULL);
  lp = gdk_pixbuf_get_pixels(left);
  rp = gdk_pixbuf_get_pixels(right);
  width = gdk_pixbuf_get_width(left);
  height = gdk_pixbuf_get_height(left);
  stride = gdk_pixbuf_get_rowstride(left);

  /* Create an integer distance buffer */
  dist_buffer = g_malloc(sizeof(float) * width * height);

  /* For each row... */
  for (y=0; y<height; y++) {
    printf("row %d\n", y);

    /* Select filter widths starting from 1 pixel and going up to the entire width of the image */
    for (filter_width=1; filter_width<=width/4; filter_width+=4) {

      /* This filter looks for similarities that are filter_width pixels apart */
      for (x=filter_width-1; x<width-filter_width+1; x++) {
	int z;
	z = dist_buffer[x + width*y];

	z += filter_width / (10+fabs(lp[x*3 + stride*y] - rp[(x + filter_width - 1)*3 + stride*y]));

	if (z < dist_min) dist_min = z;
	if (z > dist_max) dist_max = z;

	dist_buffer[x + width*y] = z;
      }
    }
  }

  /* Generate an image from scaled distances */
  output = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
  out = gdk_pixbuf_get_pixels(output);

  for (y=0; y<height; y++) {
    for (x=0; x<width; x++) {
      guchar gray = (dist_buffer[x + width*y] - dist_min) * 255 / (dist_max - dist_min);
      out[x*3 + stride*y + 0] = gray;
      out[x*3 + stride*y + 1] = gray;
      out[x*3 + stride*y + 2] = gray;
      out[x*3 + stride*y + 3] = gray;
    }
  }

  gdk_pixbuf_save(output, "heightmap.png", "png", NULL, NULL);
  return 0;
}
