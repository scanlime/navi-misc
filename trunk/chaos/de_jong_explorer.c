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
#include <unistd.h>
#include <sys/time.h>


struct {
  double x,y;
} point;

struct {
  double a, b, c, d;
  double zoom, xoffset, yoffset;
} params;

struct {
  guint width, height;
  guint *counts;
  guint32 *pixels;

  guint color_table_size;
  guint32 *color_table;

  double iterations;
  guint current_density;

  double exposure;
  guint target_density;
} render;

struct {
  GtkWidget *window, *drawing_area, *iterl;
  GtkWidget *as, *bs, *cs, *ds, *ls, *zs, *xos, *yos;
  GtkWidget *start, *stop, *save, *randbutton;
  GdkGC *gc;
  guint idler;
} gui;


void usage(char **argv);
void interactive_main(int argc, char ** argv);
void render_main(const char *filename);
void set_defaults();
GtkWidget *build_sidebar();
void resize(int w, int h);
int limit_update_rate(float max_rate);
int auto_limit_update_rate(void);
float get_pixel_scale();
void update_color_table();
void update_pixels();
void update_gui();
void update_drawing_area();
void clear();
void run_iterations(int count);
int interactive_idle_handler(gpointer user_data);
gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
gboolean deletee(GtkWidget *widget, GdkEvent *event, gpointer user_data);
void startclick(GtkWidget *widget, gpointer user_data);
void stopclick(GtkWidget *widget, gpointer user_data);
void param_spinner_changed(GtkWidget *widget, gpointer user_data);
void exposure_changed(GtkWidget *widget, gpointer user_data);
float generate_random_param();
void randomclick(GtkWidget *widget, gpointer user_data);
gchar* save_parameters();
void load_parameters(const gchar *params);
void load_parameters_from_file(const char *name);
void save_to_file(const char *name);
void saveclick(GtkWidget *widget, gpointer user_data);


int main(int argc, char ** argv) {
  enum {INTERACTIVE, RENDER} mode = INTERACTIVE;
  const char *outputFile;
  int opt;
  char *cptr;

  srand(time(NULL));
  g_type_init();
  set_defaults();

  while (1) {
    opt = getopt(argc, argv, "a:b:c:d:x:y:z:s:e:o:i:t:h");
    if (opt == -1)
      break;

    switch (opt) {

    case 'a':  params.a       = atof(optarg);  break;
    case 'b':  params.b       = atof(optarg);  break;
    case 'c':  params.c       = atof(optarg);  break;
    case 'd':  params.d       = atof(optarg);  break;
    case 'x':  params.xoffset = atof(optarg);  break;
    case 'y':  params.yoffset = atof(optarg);  break;
    case 'z':  params.zoom    = atof(optarg);  break;

    case 'e':  render.exposure       = atof(optarg);  break;
    case 't':  render.target_density = atol(optarg);  break;

    case 's':
      /* A single number can be given to make a square image,
       * or a size in WIDTHxHEIGHT form can be given.
       */
      render.width = strtol(optarg, &cptr, 10);
      if (*cptr == 'x')
	render.height = atoi(cptr+1);
      else
	render.height = render.width;
      break;

    case 'i':
      load_parameters_from_file(optarg);
      break;

    case 'o':
      mode = RENDER;
      outputFile = optarg;
      break;

    default:
      usage(argv);
      return 1;
    }
  }

  if (optind < argc) {
    usage(argv);
    return 1;
  }

  resize(render.width, render.height);

  switch (mode) {

  case INTERACTIVE:
    interactive_main(argc, argv);
    break;

  case RENDER:
    render_main(outputFile);
    break;
  }

  return 0;
}

void usage(char **argv) {
  printf("Usage: %s [options]\n"
	 "Interactive exploration of the Peter de Jong attractor\n"
	 "\n"
	 "Parameters:\n"
	 "  -a VALUE           Set the 'a' parameter [%f]\n"
	 "  -b VALUE           Set the 'b' parameter [%f]\n"
	 "  -c VALUE           Set the 'c' parameter [%f]\n"
	 "  -d VALUE           Set the 'd' parameter [%f]\n"
	 "  -x OFFSET          Set the X offest [%f]\n"
	 "  -y OFFSET          Set the Y offset [%f]\n"
	 "  -z ZOOM            Set the zoom factor [%f]\n"
	 "\n"
	 "Rendering:\n"
	 "  -e EXPOSURE        Set the image exposure [%f]\n"
	 "\n"
	 "Quality:\n"
	 "  -s WIDTH[xHEIGHT]  Set the image size in pixels. If only one value is\n"
	 "                       given, a square image is produced [%d]\n"
	 "  -t DENSITY         In noninteractive rendering, set the count\n"
	 "                       density to stop rendering at. Larger numbers give\n"
	 "                       smoother and more detailed results, but increase\n"
	 "                       running time linearly [%d]\n"
	 "\n"
	 "Actions:\n"
	 "  -i FILE            Load all parameters from the tEXt chunk of any\n"
	 "                       .png image file generated by this program.\n"
	 "  -o FILE            Instead of presenting an interactive GUI, render\n"
	 "                       an image with the provided settings and write it\n"
	 "                       in PNG format to FILE.\n",
	 argv[0],
	 params.a, params.b, params.c, params.d, params.xoffset, params.yoffset, params.zoom,
	 render.exposure, render.width, render.target_density);
}

void interactive_main(int argc, char ** argv) {
  /* After common initialization code needed whether or not we're running
   * interactively, this takes over to provide the gtk UI for playing with
   * the de jong attractor in mostly-real-time. Yay.
   */
  GtkWidget *hbox, *vsep;
  gtk_init(&argc, &argv);

  gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(gui.window), "delete-event", G_CALLBACK(deletee), NULL);
  gui.drawing_area = gtk_drawing_area_new();
  vsep = gtk_vseparator_new();
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), build_sidebar(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), vsep, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), gui.drawing_area, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(gui.window), hbox);
  g_signal_connect(G_OBJECT(gui.drawing_area), "expose-event", G_CALLBACK(expose), NULL);
  gtk_widget_show_all(gui.window);

  gui.gc = gdk_gc_new(gui.drawing_area->window);
  gtk_widget_set_size_request(gui.drawing_area, render.width, render.height);
  update_gui();

  gui.idler = g_idle_add(interactive_idle_handler, NULL);
  gtk_main();
}

void render_main(const char *filename) {
  /* Main function for noninteractive rendering. This renders an image with the
   * current settings until render.current_density reaches target_density. We show helpful
   * progress doodads on stdout while the poor user has to wait.
   */
  time_t start_time, now, elapsed, remaining;
  start_time = time(NULL);

  while (render.current_density < render.target_density) {
    run_iterations(1000000);

    /* This should be a fairly accurate time estimate, since current_density increases linearly */
    now = time(NULL);
    elapsed = now - start_time;
    remaining = ((float)elapsed) * render.target_density / render.current_density - elapsed;

    /* After each batch of iterations, show the percent completion, number
     * of iterations (in scientific notation), iterations per second,
     * density / target density, and elapsed time / remaining time.
     */
    if (elapsed > 0) {
      printf("%6.02f%%   %.3e   %.2e/sec   %6d / %d   %02d:%02d:%02d / %02d:%02d:%02d\n",
	     100.0 * render.current_density / render.target_density,
	     render.iterations, render.iterations / elapsed,
	     render.current_density, render.target_density,
	     elapsed / (60*60), (elapsed / 60) % 60, elapsed % 60,
	     remaining / (60*60), (remaining / 60) % 60, remaining % 60);
    }
  }

  printf("Creating image...\n");
  save_to_file(filename);
}

void set_defaults() {
  params.a = 1.4191403;
  params.b = -2.2841323;
  params.c = 2.4275403;
  params.d = -2.177196;
  params.zoom = 1;
  params.xoffset = 0;
  params.yoffset = 0;
  render.exposure = 0.05;
  render.width = 800;
  render.height = 800;
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
    gui.as = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.as), params.a);
    gtk_table_attach(GTK_TABLE(table), gui.as, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.as), "changed", G_CALLBACK(param_spinner_changed), NULL);

    gui.bs = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.bs), params.b);
    gtk_table_attach(GTK_TABLE(table), gui.bs, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.bs), "changed", G_CALLBACK(param_spinner_changed), NULL);

    gui.cs = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.cs), params.c);
    gtk_table_attach(GTK_TABLE(table), gui.cs, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.cs), "changed", G_CALLBACK(param_spinner_changed), NULL);

    gui.ds = gtk_spin_button_new_with_range(-9.999, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.ds), params.d);
    gtk_table_attach(GTK_TABLE(table), gui.ds, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.ds), "changed", G_CALLBACK(param_spinner_changed), NULL);

    gui.zs = gtk_spin_button_new_with_range(0.20, 100, 0.01);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.zs), params.zoom);
    gtk_table_attach(GTK_TABLE(table), gui.zs, 1, 2, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.zs), "changed", G_CALLBACK(param_spinner_changed), NULL);

    gui.xos = gtk_spin_button_new_with_range(-1.999, 1.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.xos), params.xoffset);
    gtk_table_attach(GTK_TABLE(table), gui.xos, 1, 2, 5, 6, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.xos), "changed", G_CALLBACK(param_spinner_changed), NULL);

    gui.yos = gtk_spin_button_new_with_range(-1.999, 1.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.yos), params.yoffset);
    gtk_table_attach(GTK_TABLE(table), gui.yos, 1, 2, 6, 7, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.yos), "changed", G_CALLBACK(param_spinner_changed), NULL);

    gui.ls = gtk_spin_button_new_with_range(0.001, 9.999, 0.001);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.ls), render.exposure);
    gtk_table_attach(GTK_TABLE(table), gui.ls, 1, 2, 7, 8, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
    g_signal_connect(G_OBJECT(gui.ls), "changed", G_CALLBACK(exposure_changed), NULL);
  }

  /* Iteration counter */
  gui.iterl = gtk_label_new("");
  gtk_table_attach(GTK_TABLE(table), gui.iterl, 0, 2, 8, 9, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

  /* Buttons */
  {
    gui.start = gtk_button_new_from_stock(GTK_STOCK_APPLY);
    gtk_widget_set_sensitive(gui.start, FALSE);
    g_signal_connect(G_OBJECT(gui.start), "clicked", G_CALLBACK(startclick), NULL);
    gtk_table_attach(GTK_TABLE(table), gui.start, 0, 2, 9, 10, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    gui.stop = gtk_button_new_from_stock(GTK_STOCK_STOP);
    g_signal_connect(G_OBJECT(gui.stop), "clicked", G_CALLBACK(stopclick), NULL);
    gtk_table_attach(GTK_TABLE(table), gui.stop, 0, 2, 10, 11, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    gui.save = gtk_button_new_from_stock(GTK_STOCK_SAVE);
    g_signal_connect(G_OBJECT(gui.save), "clicked", G_CALLBACK(saveclick), NULL);
    gtk_table_attach(GTK_TABLE(table), gui.save, 0, 2, 11, 12, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);

    gui.randbutton = gtk_button_new_with_label("Random");
    g_signal_connect(G_OBJECT(gui.randbutton), "clicked", G_CALLBACK(randomclick), NULL);
    gtk_table_attach(GTK_TABLE(table), gui.randbutton, 0, 2, 12, 13, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) 0, 6, 0);
  }

  return table;
}

void resize(int w, int h) {
  render.width = w;
  render.height = h;

  if (render.counts)
    g_free(render.counts);
  render.counts = g_malloc(sizeof(render.counts[0]) * render.width * render.height);

  if (render.pixels)
    g_free(render.pixels);
  render.pixels = g_malloc(4 * render.width * render.height);

  clear();
}

int limit_update_rate(float max_rate) {
  /* Limit the frame rate to the given value. This should be called once per
   * frame, and will return 0 if it's alright to render another frame, or 1
   * otherwise.
   */
  static struct timeval last_update;
  struct timeval now;
  gulong diff;

  /* Figure out how much time has passed, in milliseconds */
  gettimeofday(&now, NULL);
  diff = ((now.tv_usec - last_update.tv_usec) / 1000 +
	  (now.tv_sec  - last_update.tv_sec ) * 1000);

  if (diff < (1000 / max_rate)) {
    return 1;
  }
  else {
    last_update = now;
    return 0;
  }
}

int auto_limit_update_rate(void) {
  /* Automatically determine a good maximum frame rate based on the current
   * number of iterations, and use limit_update_rate() to limit us to that.
   * Returns 1 if a frame should not be rendered.
   *
   * When we just start rendering an image, we want a quite high frame rate
   * (but not high enough we bog down the GUI) so the user can interactively
   * set parameters. After the rendering has been running for a while though,
   * the image changes much less and a very slow frame rate will leave more
   * CPU for calculations.
   */
  return limit_update_rate(200 / (1 + (log(render.iterations) - 9.21) * 4));
}

float get_pixel_scale() {
  /* Calculate the scale factor for converting count[] values to luminance
   * values between 0 and 1.
   */
  float density, fscale;

  /* Scale our counts to a luminance between 0 and 1 that gets fed through our
   * colormap[] to generate an actual gdk color. 'p' contains the number of
   * times our point has passed the current pixel.
   *
   * iterations / (width * height) gives us the average density of counts[].
   */
  density = render.iterations / (render.width * render.height);

  /* fscale is a floating point number that, when multiplied by a raw
   * counts[] value, gives values between 0 and 1 corresponding to full
   * white and full black.
   */
  fscale = (render.exposure * params.zoom) / density;

  /* The very first frame we render will often be very underexposed.
   * If fscale > 0.5, this makes countsclamp negative and we get incorrect
   * results. The lowest usable value of countsclamp is 1.
   */
  if (fscale > 0.5)
    fscale = 0.5;

  return fscale;
}

void update_color_table() {
  /* Reallocate the color table if necessary, then regenerate its contents
   * to match the current_density, exposure, gamma, and other rendering parameters.
   * The color tabls is what maps counts[] values to pixels[] values quickly.
   */
  guint required_size = render.current_density + 1;
  guint count, r, g, b, a;
  float pixel_scale = get_pixel_scale();
  float luma;

  /* Current table too small? */
  if (render.color_table_size < required_size) {
    if (render.color_table)
      g_free(render.color_table);

    /* Allocate it to double the size we need now, as we expect our needs to grow. */
    render.color_table_size = required_size * 2;
    render.color_table = g_malloc(render.color_table_size * sizeof(render.color_table[0]));
  }

  /* Generate one color for every currently-possible count value... */
  for (count=0; count<=render.current_density; count++) {
    luma = count * pixel_scale;

    if (luma > 1)
      luma = 1;

    a = 255;
    r = 255 - luma*255;
    g = 255 - luma*255;
    b = 255 - luma*255;

    /* Colors are always ARGB order in little endian */
    render.color_table[count] = GUINT32_TO_LE( (a<<24) | (r<<16) | (g<<8) | b );
  }
}

void update_pixels() {
  /* Convert counts[] to colored 8-bit ARGB image data using our color lookup table */
  guint32 *pixel_p;
  guint *count_p;
  int x, y;

  update_color_table();

  pixel_p = render.pixels;
  count_p = render.counts;

  for (y=render.height; y; y--)
    for (x=render.width; x; x--)
      *(pixel_p++) = render.color_table[*(count_p++)];
}

void update_gui() {
  /* If the GUI needs updating, update it. This includes limiting the maximum
   * update rate, updating the iteration count display, and actually rendering
   * frames to the drawing area.
   */
  gchar *iters;

  if (auto_limit_update_rate())
    return;

  /* Update the iteration counter */
  iters = g_strdup_printf("Iterations:\n%.3e\n\nmax density:\n%d", render.iterations, render.current_density);
  gtk_label_set_text(GTK_LABEL(gui.iterl), iters);
  g_free(iters);

  update_pixels();
  update_drawing_area();
}

void update_drawing_area() {
  /* Update our drawing area */
  gdk_draw_rgb_32_image(gui.drawing_area->window, gui.gc,
			0, 0, render.width, render.height, GDK_RGB_DITHER_NORMAL,
			(guchar*) render.pixels, render.width * 4);
}

void clear() {
  memset(render.counts, 0, render.width * render.height * sizeof(int));
  render.current_density = 0;
  render.iterations = 0;
  point.x = ((float) rand()) / RAND_MAX;
  point.y = ((float) rand()) / RAND_MAX;
}

void run_iterations(int count) {
  double x, y;
  unsigned int i, ix, iy;
  guint *p;
  guint d;
  const double xcenter = render.width / 2.0;
  const double ycenter = render.height / 2.0;
  const double scale = xcenter / 2.5 * params.zoom;

  for(i=count; i; --i) {
    x = sin(params.a * point.y) - cos(params.b * point.x);
    y = sin(params.c * point.x) - cos(params.c * point.y);
    point.x = x;
    point.y = y;

    ix = (int)((x + params.xoffset) * scale + xcenter);
    iy = (int)((y + params.yoffset) * scale + ycenter);

    /* Clip to the size of our image. Note that ix and iy are
     * unsigned, so we only have to make one comparison each.
     */
    if (ix < render.width && iy < render.height) {
      p = render.counts + ix + render.width * iy;
      d = *p = *p + 1;
      if (d > render.current_density)
	render.current_density = d;
    }
  }
  render.iterations += count;
}

int interactive_idle_handler(gpointer user_data) {
  /* An idle handler used for interactively rendering. This runs a relatively
   * small number of iterations, then calls update_gui() to update our visible image.
   */
  run_iterations(10000);
  update_gui();
  return 1;
}

gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
  update_drawing_area();
  return TRUE;
}

gboolean deletee(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
  g_source_remove(gui.idler);
  gtk_main_quit();
}

void startclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(gui.stop, TRUE);
  gtk_widget_set_sensitive(gui.start, FALSE);
  clear();
  params.a = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.as));
  params.b = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.bs));
  params.c = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.cs));
  params.d = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.ds));
  params.zoom = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.zs));
  params.xoffset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.xos));
  params.yoffset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.yos));
  gui.idler = g_idle_add(interactive_idle_handler, NULL);
}

void stopclick(GtkWidget *widget, gpointer user_data) {
  gtk_widget_set_sensitive(gui.stop, FALSE);
  gtk_widget_set_sensitive(gui.start, TRUE);
  g_source_remove(gui.idler);
}

void param_spinner_changed(GtkWidget *widget, gpointer user_data) {
  stopclick(widget, user_data);
  startclick(widget, user_data);
}

void exposure_changed(GtkWidget *widget, gpointer user_data) {
  render.exposure = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui.ls));
  update_pixels();
  update_drawing_area();
}

float generate_random_param() {
  return ((float) rand()) / RAND_MAX * 12 - 6;
}

void randomclick(GtkWidget *widget, gpointer user_data) {
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.as), generate_random_param());
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.bs), generate_random_param());
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.cs), generate_random_param());
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.ds), generate_random_param());

  stopclick(widget, user_data);
  startclick(widget, user_data);
}

gchar* save_parameters() {
  /* Save the current parameters to a freshly allocated human and machine readable string */
  return g_strdup_printf("a = %f\n"
			 "b = %f\n"
			 "c = %f\n"
			 "d = %f\n"
			 "zoom = %f\n"
			 "xoffset = %f\n"
			 "yoffset = %f\n"
			 "exposure = %f\n",
			 params.a, params.b, params.c, params.d,
			 params.zoom, params.xoffset, params.yoffset,
			 render.exposure);
}

void load_parameters(const gchar *paramstring) {
  /* Load all recognized parameters from a string given in the same
   * format as the one produced by save_parameters()
   */
  gchar *copy, *line, *nextline;
  gchar *key;
  float value;

  /* Make a copy of the parameters, since we'll be modifying it */
  copy = g_strdup(paramstring);

  /* Iterate over lines... */
  line = copy;
  while (line) {
    nextline = strchr(line, '\n');
    if (nextline) {
      *nextline = '\0';
      nextline++;
    }

    /* Separate it into key and value */
    key = g_malloc(strlen(line)+1);
    if (sscanf(line, " %s = %f", key, &value) == 2) {
      printf("%s = %f", key, value);

      /* We found a valid key/value line.. see if we recognize the key */
      if (!strcmp(key, "a"))
	params.a = value;
      else if (!strcmp(key, "b"))
	params.b = value;
      else if (!strcmp(key, "c"))
	params.c = value;
      else if (!strcmp(key, "d"))
	params.d = value;
      else if (!strcmp(key, "zoom"))
	params.zoom = value;
      else if (!strcmp(key, "xoffset"))
	params.xoffset = value;
      else if (!strcmp(key, "yoffset"))
	params.yoffset = value;
      else if (!strcmp(key, "exposure"))
	render.exposure = value;
      else
	printf(" (unrecognized)");

      printf("\n");
    }
    g_free(key);
    line = nextline;
  }
  g_free(copy);
}

void load_parameters_from_file(const char *name) {
  /* Try to open the given PNG file and load parameters from it */
  const gchar *params;
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(name, NULL);
  params = gdk_pixbuf_get_option(pixbuf, "tEXt::de_jong_params");
  if (params)
    load_parameters(params);
  else
    printf("No parameters chunk found\n");
  gdk_pixbuf_unref(pixbuf);
}

void save_to_file(const char *name) {
  /* Save the current contents of pixels[] to a .PNG file */
  GdkPixbuf *pixbuf;
  gchar *params;

  /* Get a higher quality rendering */
  update_pixels();

  pixbuf = gdk_pixbuf_new_from_data((guchar*) render.pixels, GDK_COLORSPACE_RGB, TRUE,
				    8, render.width, render.height, render.width*4, NULL, NULL);

  /* Save our current parameters in a tEXt chunk, using a format that
   * is both human-readable and easy to load parameters from automatically.
   */
  params = save_parameters();
  gdk_pixbuf_save(pixbuf, name, "png", NULL, "tEXt::de_jong_params", params, NULL);
  g_free(params);
  gdk_pixbuf_unref(pixbuf);
}

#if (GTK_MAJOR_VERSION > 2) || (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 3)
static void update_preview(GtkFileChooser *chooser, gpointer data) {
  GtkWidget *preview;
  char *filename;
  GdkPixbuf *pixbuf;
  gboolean have_preview;

  preview = GTK_WIDGET(data);
  filename = gtk_file_chooser_get_preview_filename(chooser);

  pixbuf = gdk_pixbuf_new_from_file_at_size(filename, 128, 128, NULL);
  have_preview = (pixbuf != NULL);
  g_free(filename);

  gtk_image_set_from_pixbuf(GTK_IMAGE(preview), pixbuf);
  if(pixbuf)
    gdk_pixbuf_unref(pixbuf);
  gtk_file_chooser_set_preview_widget_active(chooser, have_preview);
}
#endif

void saveclick(GtkWidget *widget, gpointer user_data) {
  /* Sorry, saving only works with gtk 2.3's file selector for now */
#if (GTK_MAJOR_VERSION > 2) || (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 3)
  GtkWidget *dialog, *preview;

  dialog = gtk_file_chooser_dialog_new("Save", GTK_WINDOW(gui.window), GTK_FILE_CHOOSER_ACTION_SAVE,
  				       GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				       NULL);
  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.png");
  gtk_file_filter_set_name(filter, "PNG Image");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

  preview = gtk_image_new();
  gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(dialog), preview);
  g_signal_connect(dialog, "update-preview", G_CALLBACK(update_preview), preview);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    save_to_file(filename);
    g_free(filename);
  }
  g_object_unref(filter);
  gtk_widget_destroy(dialog);
#else
#warning "If you had gtk 2.3, you'd be able to save images"
#endif
}

/* The End */
