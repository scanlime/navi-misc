/*
 * interface.c - Implements the GTK+ user interface. Code in this module isn't
 *               used when rendering from the command line.
 *
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
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
#include <glade/glade.h>
#include "main.h"
#include "color-button.h"

struct {
  GladeXML *xml;
  GtkWidget *window;

  GtkWidget *drawing_area;
  GdkGC *gc;

  GtkStatusbar *statusbar;
  guint render_status_message_id;
  guint render_status_context;

  guint idler;
  gboolean writing_params;
  gboolean just_resized;
} gui;

static int limit_update_rate(float max_rate);
static int auto_limit_update_rate(void);
static void update_gui();
static void update_drawing_area();
static int interactive_idle_handler(gpointer user_data);
static float generate_random_param();
static void read_gui_params();
static void write_gui_params();
static void gui_resize(int width, int height);
static void start_rendering();
static void stop_rendering();
static void restart_rendering();

gboolean on_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
void on_param_spinner_changed(GtkWidget *widget, gpointer user_data);
void on_render_spinner_changed(GtkWidget *widget, gpointer user_data);
void on_color_changed(GtkWidget *widget, gpointer user_data);
void on_randomize(GtkWidget *widget, gpointer user_data);
void on_load_defaults(GtkWidget *widget, gpointer user_data);
void on_save(GtkWidget *widget, gpointer user_data);
void on_quit(GtkWidget *widget, gpointer user_data);
gboolean on_viewport_expose(GtkWidget *widget, gpointer user_data);
GtkWidget *custom_color_button_new(gchar *widget_name, gchar *string1, gchar *string2, gint int1, gint int2);


void interactive_main(int argc, char **argv) {
  /* After common initialization code needed whether or not we're running
   * interactively, this takes over to provide the gtk UI for playing with
   * the de jong attractor in mostly-real-time. Yay.
   */
  gtk_init(&argc, &argv);
  glade_init();

  gui.xml = glade_xml_new("data/de-jong-explorer.glade", NULL, NULL);
  write_gui_params();
  glade_xml_signal_autoconnect(gui.xml);

  gui.window = glade_xml_get_widget(gui.xml, "explorer_window");

  gui.drawing_area = glade_xml_get_widget(gui.xml, "main_drawingarea");
  gui.gc = gdk_gc_new(gui.drawing_area->window);
  gui_resize(render.width, render.height);

  gui.statusbar = GTK_STATUSBAR(glade_xml_get_widget(gui.xml, "statusbar"));
  gui.render_status_context = gtk_statusbar_get_context_id(gui.statusbar, "Rendering status");

  gui.idler = g_idle_add(interactive_idle_handler, NULL);
  gtk_main();
}

static void gui_resize(int width, int height) {
  gtk_widget_set_size_request(gui.drawing_area, width, height);

  /* A bit of a hack to make the default window size more sane */
  gtk_widget_set_size_request(glade_xml_get_widget(gui.xml, "drawing_area_viewport"),
			      width+5, height+5);
  gui.just_resized = TRUE;
}

gboolean on_viewport_expose(GtkWidget *widget, gpointer user_data) {
  /* After the drawing area is shown, go back to the natural size request */
  if (gui.just_resized) {
    gtk_widget_set_size_request(widget, -1, -1);
    gui.just_resized = FALSE;
  }
  return TRUE;
}

static int limit_update_rate(float max_rate) {
  /* Limit the frame rate to the given value. This should be called once per
   * frame, and will return 0 if it's alright to render another frame, or 1
   * otherwise.
   */
  static GTimeVal last_update;
  GTimeVal now;
  gulong diff;

  /* Figure out how much time has passed, in milliseconds */
  g_get_current_time(&now);
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

static int auto_limit_update_rate(void) {
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
  return limit_update_rate(200 / (1 + (log(render.iterations) - 9.21) * 5));
}

static void update_gui() {
  /* If the GUI needs updating, update it. This includes limiting the maximum
   * update rate, updating the iteration count display, and actually rendering
   * frames to the drawing area.
   */
  GtkWidget *statusbar;
  gchar *iters;

  /* Skip frame rate limiting and updating the iteration counter if we're in
   * a hurry to show the user the result of a modified rendering parameter.
   */
  if (!render.dirty_flag) {

    if (auto_limit_update_rate())
      return;

    /* Update the iteration counter, removing the previous one if it existed */
    iters = g_strdup_printf("Iterations:    %.3e        Peak density:    %d", render.iterations, render.current_density);
    if (gui.render_status_message_id)
      gtk_statusbar_remove(gui.statusbar, gui.render_status_context, gui.render_status_message_id);
    gui.render_status_message_id = gtk_statusbar_push(gui.statusbar, gui.render_status_context, iters);
    g_free(iters);
  }

  update_pixels();
  update_drawing_area();
}

static void update_drawing_area() {
  /* Update our drawing area */
  gdk_draw_rgb_32_image(gui.drawing_area->window, gui.gc,
			0, 0, render.width, render.height, GDK_RGB_DITHER_NORMAL,
			(guchar*) render.pixels, render.width * 4);
}

static int interactive_idle_handler(gpointer user_data) {
  /* An idle handler used for interactively rendering. This runs a relatively
   * small number of iterations, then calls update_gui() to update our visible image.
   */
  run_iterations(10000);
  update_gui();
  return 1;
}

gboolean on_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
  GdkRectangle *rects;
  int n_rects, i;

  gdk_region_get_rectangles(event->region, &rects, &n_rects);

  for (i=0; i<n_rects; i++) {
    /* Clip this rectangle to the image size, since reading outside our buffer is a bad thing */
    if (rects[i].x + rects[i].width > render.width)
      rects[i].width = render.width - rects[i].x;
    if (rects[i].y + rects[i].height > render.height)
      rects[i].height = render.height - rects[i].y;
    if (rects[i].width <= 0 || rects[i].height <= 0)
      continue;

    /* Render a rectangle taken from our pixels[] array */
    gdk_draw_rgb_32_image(gui.drawing_area->window, gui.gc,
			  rects[i].x, rects[i].y,
			  rects[i].width, rects[i].height,
			  GDK_RGB_DITHER_NORMAL,
			  (guchar*) (render.pixels + rects[i].x + rects[i].y * render.width),
			  render.width * 4);
  }

  g_free(rects);
  return FALSE;
}

void on_quit(GtkWidget *widget, gpointer user_data) {
  g_source_remove(gui.idler);
  gtk_main_quit();
}

static void read_gui_params() {
  if (gui.writing_params)
    return;

  params.a = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_a")));
  params.b = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_b")));
  params.c = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_c")));
  params.d = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_d")));
  params.zoom = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_zoom")));
  params.xoffset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_xoffset")));
  params.yoffset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_yoffset")));
  params.rotation = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_rotation")));
  params.blur_radius = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_blur_radius")));
  params.blur_ratio = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_blur_ratio")));
  render.exposure = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_exposure")));
  render.gamma = gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_gamma")));
  color_button_get_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_fgcolor")), &render.fgcolor);
  color_button_get_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_bgcolor")), &render.bgcolor);
}

static void write_gui_params() {
  gui.writing_params = TRUE;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_a")), params.a);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_b")), params.b);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_c")), params.c);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_d")), params.d);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_zoom")), params.zoom);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_xoffset")), params.xoffset);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_yoffset")), params.yoffset);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_rotation")), params.rotation);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_blur_radius")), params.blur_radius);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_blur_ratio")), params.blur_ratio);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_exposure")), render.exposure);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "param_gamma")), render.gamma);
  color_button_set_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_fgcolor")), &render.fgcolor);
  color_button_set_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_bgcolor")), &render.bgcolor);
  gui.writing_params = FALSE;
}

static void start_rendering() {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(gui.xml, "pause_menu")), FALSE);
  clear();
  read_gui_params();
  gui.idler = g_idle_add(interactive_idle_handler, NULL);
}

static void stop_rendering() {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(gui.xml, "pause_menu")), TRUE);
  g_source_remove(gui.idler);
}

static void restart_rendering() {
  stop_rendering();
  start_rendering();
}

void on_param_spinner_changed(GtkWidget *widget, gpointer user_data) {
  struct computation_params old_params;

  if (gui.writing_params)
    return;

  /* It's worth it to make sure the parameters really changed. The
   * spin buttons seem to like sending out changed signals even when
   * nothing actually changed, and this could be really frustrating
   * if the image has been rendering for a while.
   */
  old_params = params;
  read_gui_params();
  if (!memcmp(&params, &old_params, sizeof(params)))
    return;

  restart_rendering();
}

void on_render_spinner_changed(GtkWidget *widget, gpointer user_data) {
  if (gui.writing_params)
    return;

  read_gui_params();
  render.dirty_flag = TRUE;
}

void on_color_changed(GtkWidget *widget, gpointer user_data) {
  /* The simple method of just setting dirty_flag works well when the spin
   * button values change, but the color picker sucks up too much event loop
   * time for that to work nicely for colors. This is a bit of a hack that
   * makes color picking run much more smoothly.
   */

  if (gui.writing_params)
    return;

  read_gui_params();
  render.dirty_flag = TRUE;

  gtk_main_iteration();
  update_gui();
}

static float generate_random_param() {
  return uniform_variate() * 12 - 6;
}

void on_randomize(GtkWidget *widget, gpointer user_data) {
  params.a = generate_random_param();
  params.b = generate_random_param();
  params.c = generate_random_param();
  params.d = generate_random_param();
  write_gui_params();

  restart_rendering();
}

void on_load_defaults(GtkWidget *widget, gpointer user_data) {
  set_defaults();
  write_gui_params();
  restart_rendering();
}

GtkWidget *custom_color_button_new(gchar *widget_name, gchar *string1,
				   gchar *string2, gint int1, gint int2) {
  GtkWidget *w;
  w = color_button_new(string1, int1 ? &render.fgcolor : &render.bgcolor);
  gtk_widget_show_all(w);
  return w;
}

void on_save(GtkWidget *widget, gpointer user_data) {
  GtkWidget *dialog;

  dialog = gtk_file_selection_new("Save");
  gtk_file_selection_set_filename(GTK_FILE_SELECTION(dialog), "rendering.png");

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    const gchar *filename;
    filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog));
    save_to_file(filename);
  }
  gtk_widget_destroy(dialog);
}

/* The End */
