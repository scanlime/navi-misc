/*
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
 *
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define PROGRESSIVE_MASK 3

struct {
  double x,y;
} point;

GtkWidget *window, *drawing_area, *iterl;
GtkWidget *as, *bs, *cs, *ds, *ls, *zs, *xos, *yos;
GtkWidget *start, *stop, *save, *randbutton;
double iterations;
GdkGC *gc;
guint *counts;
int width, height;
guint countsMax;
guchar *pixels;
double a, b, c, d, exposure, zoom, xoffset, yoffset;
guint idler;

void flip();
void clear();
void resize(int w, int h);
int interactive_idle_handler(gpointer user_data);
gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
void param_spinner_changed(GtkWidget *widget, gpointer user_data);
void exposure_changed(GtkWidget *widget, gpointer user_data);
void startclick(GtkWidget *widget, gpointer user_data);
void stopclick(GtkWidget *widget, gpointer user_data);
void saveclick(GtkWidget *widget, gpointer user_data);
void randomclick(GtkWidget *widget, gpointer user_data);
gboolean deletee(GtkWidget *widget, GdkEvent *event, gpointer user_data);
GtkWidget *build_sidebar();

int main(int argc, char ** argv) {
  srand(time(NULL));
  gtk_init(&argc, &argv);

  GtkWidget *hbox, *vsep;

  a = 1.4191403;
  b = -2.2841323;
  c = 2.4275403;
  d = -2.177196;
  exposure = 0.05;
  zoom = 1;
  xoffset = 0;
  yoffset = 0;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(deletee), NULL);
  drawing_area = gtk_drawing_area_new();
  vsep = gtk_vseparator_new();
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), build_sidebar(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), vsep, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), drawing_area, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), hbox);
  g_signal_connect(G_OBJECT(drawing_area), "expose-event", G_CALLBACK(expose), NULL);
  gtk_widget_show_all(window);

  gc = gdk_gc_new(drawing_area->window);

  resize(800,800);
  clear();
  flip();

  idler = g_idle_add(interactive_idle_handler, NULL);

  gtk_main();
}

GtkWidget *build_sidebar() {
  GtkWidget *table;

  table = gtk_table_new(11, 2, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 6);
  gtk_table_set_col_spacings(GTK_TABLE(table), 6);

  /* Labels */
  {
    GtkWidget *label;

    label = gtk_label_new("a:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    label = gtk_label_new("b:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    label = gtk_label_new("c:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    label = gtk_label_new("d:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    label = gtk_label_new("zoom:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    label = gtk_label_new("xoffset:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 5, 6, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    label = gtk_label_new("yoffset:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 6, 7, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    label = gtk_label_new("exposure:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 7, 8, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  }

  /* Spin buttons */
  {
    as = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(as), a);
    gtk_table_attach(GTK_TABLE(table), as, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(as), "changed", G_CALLBACK(param_spinner_changed), NULL);

    bs = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(bs), b);
    gtk_table_attach(GTK_TABLE(table), bs, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(bs), "changed", G_CALLBACK(param_spinner_changed), NULL);

    cs = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(cs), c);
    gtk_table_attach(GTK_TABLE(table), cs, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(cs), "changed", G_CALLBACK(param_spinner_changed), NULL);

    ds = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ds), d);
    gtk_table_attach(GTK_TABLE(table), ds, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(ds), "changed", G_CALLBACK(param_spinner_changed), NULL);

    zs = gtk_spin_button_new_with_range(0.01, 100, 0.01);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(zs), zoom);
    gtk_table_attach(GTK_TABLE(table), zs, 1, 2, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(zs), "changed", G_CALLBACK(param_spinner_changed), NULL);

    xos = gtk_spin_button_new_with_range(-1.999, 1.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(xos), xoffset);
    gtk_table_attach(GTK_TABLE(table), xos, 1, 2, 5, 6, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(xos), "changed", G_CALLBACK(param_spinner_changed), NULL);

    yos = gtk_spin_button_new_with_range(-1.999, 1.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(yos), yoffset);
    gtk_table_attach(GTK_TABLE(table), yos, 1, 2, 6, 7, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(yos), "changed", G_CALLBACK(param_spinner_changed), NULL);

    ls = gtk_spin_button_new_with_range(0.001, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ls), exposure);
    gtk_table_attach(GTK_TABLE(table), ls, 1, 2, 7, 8, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(ls), "changed", G_CALLBACK(exposure_changed), NULL);
  }

  /* Iteration counter */
  iterl = gtk_label_new("");
  gtk_table_attach(GTK_TABLE(table), iterl, 0, 2, 8, 9, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

  /* Buttons */
  {
    start = gtk_button_new_from_stock(GTK_STOCK_APPLY);
    gtk_widget_set_sensitive(start, FALSE);
    g_signal_connect(G_OBJECT(start), "clicked", G_CALLBACK(startclick), NULL);
    gtk_table_attach(GTK_TABLE(table), start, 0, 2, 9, 10, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    stop = gtk_button_new_from_stock(GTK_STOCK_STOP);
    g_signal_connect(G_OBJECT(stop), "clicked", G_CALLBACK(stopclick), NULL);
    gtk_table_attach(GTK_TABLE(table), stop, 0, 2, 10, 11, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    save = gtk_button_new_from_stock(GTK_STOCK_SAVE);
    g_signal_connect(G_OBJECT(save), "clicked", G_CALLBACK(saveclick), NULL);
    gtk_table_attach(GTK_TABLE(table), save, 0, 2, 11, 12, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    randbutton = gtk_button_new_with_label("Random");
    g_signal_connect(G_OBJECT(randbutton), "clicked", G_CALLBACK(randomclick), NULL);
    gtk_table_attach(GTK_TABLE(table), randbutton, 0, 2, 12, 13, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  }

  return table;
}

void resize(int w, int h) {
  width = w;
  height = h;

  gtk_widget_set_size_request(drawing_area, width, height);

  if (counts)
    g_free(counts);
  counts = g_malloc(sizeof(counts[0]) * width * height);

  if (pixels)
    g_free(pixels);
  pixels = g_malloc(4 * width * height);
}

void flip() {
  /* Using the current point density, scale counts[] appropriately,
   * color it, and copy it to the screen.
   */

  const int rowstride = width * 4;
  guchar *row;
  guint32 *p;
  guint32 gray, iscale;
  guint countsclamp, dval, *count_p, *count_row;
  int x, y;
  float density, luma, fscale, max_frame_rate;
  struct timeval now;
  suseconds_t diff;
  static struct timeval last_flip;
  static int progressiveRow = 0;

  /* Figure out a good maximum frame rate. When we just start rendering an image,
   * we want a quite high frame rate (but not high enough we bog down the GUI)
   * so the user can interactively set initial conditions. After the rendering has
   * been running for a while though, the image changes much less and a very slow
   * frame rate will suffice.
   */
  max_frame_rate = 200 / (1 + (log(iterations) - 9.21) * 4);

  /* Limit the maximum frame rate, so we're more responsive when dragging parameters */
  gettimeofday(&now, NULL);
  if (now.tv_sec - last_flip.tv_sec < 2) {
    diff = now.tv_usec - last_flip.tv_usec;
    if (now.tv_sec != last_flip.tv_sec)
      diff += 1000000;
    if (diff < 1000000 / max_frame_rate)
      return;
  }
  last_flip = now;

  /* Update the iteration counter */
  gchar *iters = g_strdup_printf("Iterations:\n%.3e\n\nmax density:\n%d", iterations, countsMax);
  gtk_label_set_text(GTK_LABEL(iterl), iters);
  g_free(iters);

  /* Scale our counts to a luminance between 0 and 1 that gets fed through our
   * colormap[] to generate an actual gdk color. 'p' contains the number of
   * times our point has passed the current pixel.
   *
   * iterations / (width * height) gives us the average density of counts[].
   */
  density = iterations / (width * height);

  /* fscale is a floating point number that, when multiplied by a raw
   * counts[] value, gives values between 0 and 1 corresponding to full
   * white and full black.
   */
  fscale = (exposure * zoom) / density;

  /* The very first frame we render will often be very underexposed.
   * If fscale > 0.5, this makes countsclamp negative and we get incorrect
   * results. The lowest usable value of countsclamp is 1.
   */
  if (fscale > 0.5)
    fscale = 0.5;

  /* This is the maximum allowed value for counts[], corresponding to full black */
  countsclamp = (int)(1 / fscale) - 1;

  /* Convert fscale to an 8:24 fixed point number that will map counts[]
   * values to gray levels between 0 and 255.
   */
  iscale = (guint32)(fscale * 0xFF000000L);

  row = pixels;
  count_row = counts;
  for (y=0; y<height; y++) {
    if ((y & PROGRESSIVE_MASK) == progressiveRow) {
      p = (guint32*) row;
      count_p = count_row;
      for (x=0; x<width; x++) {

	dval = *(count_p++);
	if (dval > countsclamp)
	  dval = countsclamp;

	gray = 255 - ((dval * iscale) >> 24);

	*(p++) = GUINT32_TO_LE( gray | (gray<<8) | (gray<<16) | 0xFF000000UL );
      }
    }
    row += rowstride;
    count_row += width;
  }

  progressiveRow = (progressiveRow + 1) & PROGRESSIVE_MASK;

  gdk_draw_rgb_32_image(drawing_area->window, gc,
			0, 0, width, height, GDK_RGB_DITHER_NORMAL,
			pixels, rowstride);
}

void clear() {
  memset(counts, 0, width * height * sizeof(int));
  countsMax = 0;
  iterations = 0;
  point.x = ((float) rand()) / RAND_MAX;
  point.y = ((float) rand()) / RAND_MAX;
}

void run_iterations(int count) {
  double x, y;
  int i, ix, iy;
  guint *p;
  guint d;
  const double xcenter = width / 2.0;
  const double ycenter = height / 2.0;
  const double xscale = xcenter / 2.5 * zoom;
  const double yscale = ycenter / 2.5 * zoom;

  for(i=count; i; --i) {
    x = sin(a * point.y) - cos(b * point.x);
    y = sin(c * point.x) - cos(c * point.y);
    point.x = x;
    point.y = y;

    ix = (int)((x + xoffset) * xscale + xcenter);
    iy = (int)((y + yoffset) * yscale + ycenter);

    if (ix >= 0 && iy >= 0 && ix < width && iy < height) {
      p = counts + ix + width * iy;
      d = *p = *p + 1;
      if (d > countsMax)
	countsMax = d;
    }
  }
  iterations += count;
}

int interactive_idle_handler(gpointer user_data) {
  /* An idle handler used for interactively rendering. This runs a relatively
   * small number of iterations, then calls flip() to update our visible image.
   */
  run_iterations(10000);
  flip();
  return 1;
}

gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
  flip();
  return TRUE;
}

gboolean deletee(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
  g_source_remove(idler);
  gtk_main_quit();
}

void startclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(stop, TRUE);
  gtk_widget_set_sensitive(start, FALSE);
  clear();
  a = gtk_spin_button_get_value(GTK_SPIN_BUTTON(as));
  b = gtk_spin_button_get_value(GTK_SPIN_BUTTON(bs));
  c = gtk_spin_button_get_value(GTK_SPIN_BUTTON(cs));
  d = gtk_spin_button_get_value(GTK_SPIN_BUTTON(ds));
  zoom = gtk_spin_button_get_value(GTK_SPIN_BUTTON(zs));
  xoffset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(xos));
  yoffset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(yos));
  idler = g_idle_add(interactive_idle_handler, NULL);
}

void stopclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(stop, FALSE);
  gtk_widget_set_sensitive(start, TRUE);
  g_source_remove(idler);
}

void param_spinner_changed(GtkWidget *widget, gpointer user_data) {
  stopclick(widget, user_data);
  startclick(widget, user_data);
}

void exposure_changed(GtkWidget *widget, gpointer user_data) {
  exposure = gtk_spin_button_get_value(GTK_SPIN_BUTTON(ls));
}

float generateRandomParameter() {
  return ((float) rand()) / RAND_MAX * 12 - 6;
}

void randomclick(GtkWidget *widget, gpointer user_data) {
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(as), generateRandomParameter());
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(bs), generateRandomParameter());
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cs), generateRandomParameter());
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ds), generateRandomParameter());

  stopclick(widget, user_data);
  startclick(widget, user_data);
}

void saveclick(GtkWidget *widget, gpointer user_data) {
  /* Sorry, saving only works with gtk 2.3's file selector for now */
#if (GTK_MAJOR_VERSION > 2) || (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 3)
  GtkWidget *dialog;

  dialog = gtk_file_chooser_dialog_new("Save", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE,
  				       GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				       NULL);
  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.png");
  gtk_file_filter_set_name(filter, "PNG Image");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
  if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename;
    GdkPixbuf *pixbuf;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    pixbuf = gdk_pixbuf_new_from_data(pixels, GDK_COLORSPACE_RGB, TRUE,
				      8, width, height, width*4, NULL, NULL);
    gdk_pixbuf_save(pixbuf, filename, "png", NULL, NULL);
    gdk_pixbuf_unref(pixbuf);
    g_free(filename);
  }
  g_object_unref(filter);
  gtk_widget_destroy(dialog);
#else
#warning "If you had gtk 2.3, you'd be able to save images"
#endif
}
