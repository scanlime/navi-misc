/*
 * ui-main.c - Implements most of the GTK+ user interface
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

#include "de-jong.h"
#include "main.h"
#include "color-button.h"

struct gui_state gui;


static int limit_update_rate(float max_rate);
static int auto_limit_update_rate(void);
static void update_gui();
static void update_drawing_area();
static int interactive_idle_handler(gpointer user_data);
static float generate_random_param();
static void read_gui_params();
static void gui_resize(int width, int height);

static void tool_grab(double dx, double dy);
static void tool_blur(double dx, double dy);
static void tool_zoom(double dx, double dy);
static void tool_rotate(double dx, double dy);
static void tool_exposure_gamma(double dx, double dy);
static void tool_a_b(double dx, double dy);
static void tool_a_c(double dx, double dy);
static void tool_a_d(double dx, double dy);
static void tool_b_c(double dx, double dy);
static void tool_b_d(double dx, double dy);
static void tool_c_d(double dx, double dy);
static void tool_ab_cd(double dx, double dy);
static void tool_ac_bd(double dx, double dy);

GtkWidget *custom_color_button_new(gchar *widget_name, gchar *string1, gchar *string2, gint int1, gint int2);
gboolean on_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
void on_param_spinner_changed(GtkWidget *widget, gpointer user_data);
void on_render_spinner_changed(GtkWidget *widget, gpointer user_data);
void on_color_changed(GtkWidget *widget, gpointer user_data);
void on_randomize(GtkWidget *widget, gpointer user_data);
void on_load_defaults(GtkWidget *widget, gpointer user_data);
void on_save(GtkWidget *widget, gpointer user_data);
void on_quit(GtkWidget *widget, gpointer user_data);
void on_pause_rendering_toggle(GtkWidget *widget, gpointer user_data);
void on_load_from_image(GtkWidget *widget, gpointer user_data);
void on_resize(GtkWidget *widget, gpointer user_data);
void on_resize_cancel(GtkWidget *widget, gpointer user_data);
void on_resize_ok(GtkWidget *widget, gpointer user_data);
void on_tool_activate(GtkWidget *widget, gpointer user_data);
gboolean on_viewport_expose(GtkWidget *widget, gpointer user_data);
gboolean on_motion_notify(GtkWidget *widget, GdkEvent *event);
gboolean on_button_press(GtkWidget *widget, GdkEvent *event);
void on_widget_toggle(GtkWidget *widget, gpointer user_data);


void interactive_main(DeJong *dejong, int argc, char **argv) {
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
  gtk_widget_add_events(gui.drawing_area,
			GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON_RELEASE_MASK |
			GDK_BUTTON_MOTION_MASK);
  gui.gc = gdk_gc_new(gui.drawing_area->window);
  gui_resize(render.width, render.height);

  gui.statusbar = GTK_STATUSBAR(glade_xml_get_widget(gui.xml, "statusbar"));
  gui.render_status_context = gtk_statusbar_get_context_id(gui.statusbar, "Rendering status");
  gui.current_tool = "None";

  animation_ui_init();

  gui.idler = g_idle_add(interactive_idle_handler, NULL);
  gtk_main();
}

static void gui_resize(int width, int height) {
  gtk_widget_set_size_request(gui.drawing_area, width, height);

  /* A bit of a hack to make the default window size more sane */
  gtk_widget_set_size_request(glade_xml_get_widget(gui.xml, "drawing_area_viewport"),
			      MIN(1000, width+5), MIN(1000, height+5));
  gui.just_resized = TRUE;
}

gboolean on_viewport_expose(GtkWidget *widget, gpointer user_data) {
  /* After the drawing area is shown, go back to the natural size request */
  if (gui.just_resized) {
    gtk_widget_set_size_request(widget, -1, -1);
    gui.just_resized = FALSE;
  }
  return FALSE;
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
    iters = g_strdup_printf("Iterations:    %.3e    \tPeak density:    %d    \tCurrent tool: %s",
			    render.iterations, render.current_density, gui.current_tool);
    if (gui.render_status_message_id)
      gtk_statusbar_remove(gui.statusbar, gui.render_status_context, gui.render_status_message_id);
    gui.render_status_message_id = gtk_statusbar_push(gui.statusbar, gui.render_status_context, iters);
    g_free(iters);
  }

  update_pixels();
  update_drawing_area();

  /* This keeps our memory from growing without bound if the user just hit a
   * stable oscillation, or left this running unattended for a long time...
   * Memory grows linearly with the size of the color lookup table. This should
   * force the user to pause at 1.5 million density, which would give
   * a table size of between 6 and 12 megabytes.
   */
  if (render.current_density > 1500000)
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(gui.xml, "pause_menu")), TRUE);
}

static void update_drawing_area() {
  /* Update our entire drawing area.
   * We use GdkRGB directly here to force ignoring the alpha channel.
   */
    gdk_draw_rgb_32_image(gui.drawing_area->window, gui.gc,
			  0, 0, render.width, render.height, GDK_RGB_DITHER_NORMAL,
			  gdk_pixbuf_get_pixels(render.pixbuf), render.width * 4);
}

static int interactive_idle_handler(gpointer user_data) {
  /* An idle handler used for interactively rendering. This runs a relatively
   * small number of iterations, then calls update_gui() to update our visible image.
   */
  GTimer *timer;
  gulong elapsed;

  if (gui.update_calc_params_when_convenient || gui.update_render_params_when_convenient) {
    write_gui_params();
  }

  if (gui.update_calc_params_when_convenient) {
    restart_rendering();
    gui.update_calc_params_when_convenient = FALSE;
  }

  if (gui.update_render_params_when_convenient) {
    render.dirty_flag = TRUE;
    gui.update_render_params_when_convenient = FALSE;
  }

  /* Run as many blocks of iterations as we can in 12 milliseconds */
  timer = g_timer_new();
  g_timer_start(timer);
  do {
    run_iterations(5000);
    g_timer_elapsed(timer, &elapsed);
  } while (elapsed < 12000);
  g_timer_destroy(timer);

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

    /* Render a rectangle taken from our pixbuf.
     * We use GdkRGB directly here to force ignoring the alpha channel.
     */
    gdk_draw_rgb_32_image(gui.drawing_area->window, gui.gc,
			  rects[i].x, rects[i].y,
			  rects[i].width, rects[i].height,
			  GDK_RGB_DITHER_NORMAL,
			  gdk_pixbuf_get_pixels(render.pixbuf) + rects[i].x * 4 + rects[i].y * render.width * 4,
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
  render.clamped = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(gui.xml, "param_clamped")));
  params.tileable = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(gui.xml, "param_tileable")));
  color_button_get_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_fgcolor")), &render.fgcolor);
  color_button_get_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_bgcolor")), &render.bgcolor);
  render.fgalpha = color_button_get_alpha(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_fgcolor")));
  render.bgalpha = color_button_get_alpha(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_bgcolor")));
}

void write_gui_params() {
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
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(gui.xml, "param_clamped")), render.clamped);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(gui.xml, "param_tileable")), params.tileable);
  color_button_set_alpha(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_fgcolor")), render.fgalpha);
  color_button_set_alpha(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_bgcolor")), render.bgalpha);
  color_button_set_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_fgcolor")), &render.fgcolor);
  color_button_set_color(COLOR_BUTTON(glade_xml_get_widget(gui.xml, "param_bgcolor")), &render.bgcolor);
  gui.writing_params = FALSE;
}

void restart_rendering() {
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(gui.xml, "pause_menu")), TRUE);
  clear();
  read_gui_params();
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(gui.xml, "pause_menu")), FALSE);
}

void on_param_spinner_changed(GtkWidget *widget, gpointer user_data) {
  struct computation_params old_params;

  if (gui.writing_params)
    return;

  old_params = params;
  read_gui_params();

  /* Make sure at least one parameter has changed enough that the
   * difference would show up on the GUI. This prevents rounding
   * done by the spinners from causing a rendering restart.
   * The epsilon values were chosen carefully so they should always
   * catch modifications at the finest spinner resolution but ignore
   * modifications that it wouldn't show.
   */
  if (fabs(params.a           - old_params.a)           < 0.000009 &&
      fabs(params.b           - old_params.b)           < 0.000009 &&
      fabs(params.c           - old_params.c)           < 0.000009 &&
      fabs(params.d           - old_params.d)           < 0.000009 &&
      fabs(params.zoom        - old_params.zoom)        < 0.0009 &&
      fabs(params.xoffset     - old_params.xoffset)     < 0.0009 &&
      fabs(params.yoffset     - old_params.yoffset)     < 0.0009 &&
      fabs(params.rotation    - old_params.rotation)    < 0.0009 &&
      fabs(params.blur_radius - old_params.blur_radius) < 0.00009 &&
      fabs(params.blur_ratio  - old_params.blur_ratio)  < 0.00009 &&
      params.tileable == old_params.tileable)
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
  resize(render.width, render.height, render.oversample);
  gui_resize(render.width, render.height);
  write_gui_params();
  restart_rendering();
}

void on_pause_rendering_toggle(GtkWidget *widget, gpointer user_data) {
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
    g_source_remove(gui.idler);
  else
    gui.idler = g_idle_add(interactive_idle_handler, NULL);
}

void on_resize(GtkWidget *widget, gpointer user_data) {
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "resize_width")), render.width);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "resize_height")), render.height);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "resize_oversample")), render.oversample);

  gtk_widget_grab_focus(glade_xml_get_widget(gui.xml, "resize_width"));
  gtk_widget_show(glade_xml_get_widget(gui.xml, "resize_window"));
}

void on_resize_cancel(GtkWidget *widget, gpointer user_data) {
  gtk_widget_hide(glade_xml_get_widget(gui.xml, "resize_window"));
}

void on_resize_ok(GtkWidget *widget, gpointer user_data) {
  int new_width, new_height, new_oversample;
  GtkSpinButton *width_widget, *height_widget, *oversample_widget;

  width_widget = GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "resize_width"));
  height_widget = GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "resize_height"));
  oversample_widget = GTK_SPIN_BUTTON(glade_xml_get_widget(gui.xml, "resize_oversample"));

  gtk_spin_button_update(width_widget);
  gtk_spin_button_update(height_widget);
  gtk_spin_button_update(oversample_widget);

  new_width = gtk_spin_button_get_value(width_widget);
  new_height = gtk_spin_button_get_value(height_widget);
  new_oversample = gtk_spin_button_get_value(oversample_widget);
  gtk_widget_hide(glade_xml_get_widget(gui.xml, "resize_window"));

  resize(new_width, new_height, new_oversample);
  gui_resize(new_width, new_height);
}

GtkWidget *custom_color_button_new(gchar *widget_name, gchar *string1,
				   gchar *string2, gint int1, gint int2) {
  GtkWidget *w;
  w = color_button_new(string1,
		       int1 ? &render.fgcolor : &render.bgcolor,
		       int1 ?  render.fgalpha :  render.bgalpha);
  gtk_widget_show_all(w);
  return w;
}

void on_load_from_image(GtkWidget *widget, gpointer user_data) {
  GtkWidget *dialog;

  dialog = gtk_file_selection_new("Load Parameters");

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    const gchar *filename;
    filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog));
    load_parameters_from_file(filename);
    write_gui_params();
    restart_rendering();
  }
  gtk_widget_destroy(dialog);
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

gboolean on_motion_notify(GtkWidget *widget, GdkEvent *event) {
  int i;

  /* Compute delta position */
  double dx = event->motion.x - gui.last_mouse_x;
  double dy = event->motion.y - gui.last_mouse_y;

  /* A table of tool handlers and menu item names */
  static const struct {
    gchar *menu_name;
    void (*handler)(double dx, double dy);
  } tools[] = {

    {"tool_grab",           tool_grab},
    {"tool_blur",           tool_blur},
    {"tool_zoom",           tool_zoom},
    {"tool_rotate",         tool_rotate},
    {"tool_exposure_gamma", tool_exposure_gamma},
    {"tool_a_b",            tool_a_b},
    {"tool_a_c",            tool_a_c},
    {"tool_a_d",            tool_a_d},
    {"tool_b_c",            tool_b_c},
    {"tool_b_d",            tool_b_d},
    {"tool_c_d",            tool_c_d},
    {"tool_ab_cd",          tool_ab_cd},
    {"tool_ac_bd",          tool_ac_bd},

    {NULL, NULL},
  };

  for (i=0;tools[i].menu_name;i++) {
    GtkWidget *w = glade_xml_get_widget(gui.xml, tools[i].menu_name);
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(w)))
      tools[i].handler(dx, dy);
  }

  gui.last_mouse_x = event->motion.x;
  gui.last_mouse_y = event->motion.y;
  return FALSE;
}

void on_tool_activate(GtkWidget *widget, gpointer user_data) {
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
    gtk_label_get(GTK_LABEL(gtk_bin_get_child(GTK_BIN(widget))), &gui.current_tool);
}

gboolean on_button_press(GtkWidget *widget, GdkEvent *event) {
  gui.last_mouse_x = event->button.x;
  gui.last_mouse_y = event->button.y;
  return FALSE;
}

static void tool_grab(double dx, double dy) {
  double scale = 5.0 / params.zoom / render.width;
  params.xoffset += dx * scale;
  params.yoffset += dy * scale;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_blur(double dx, double dy) {
  params.blur_ratio += dx * 0.002;
  params.blur_radius -= dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_zoom(double dx, double dy) {
  params.zoom -= dy * 0.01;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_rotate(double dx, double dy) {
  params.rotation -= dx * 0.008;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_exposure_gamma(double dx, double dy) {
  render.exposure -= dy * 0.001;
  render.gamma += dx * 0.001;
  gui.update_render_params_when_convenient = TRUE;
}

static void tool_a_b(double dx, double dy) {
  params.a += dx * 0.001;
  params.b += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_a_c(double dx, double dy) {
  params.a += dx * 0.001;
  params.c += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_a_d(double dx, double dy) {
  params.a += dx * 0.001;
  params.d += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_b_c(double dx, double dy) {
  params.b += dx * 0.001;
  params.c += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_b_d(double dx, double dy) {
  params.b += dx * 0.001;
  params.d += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_c_d(double dx, double dy) {
  params.c += dx * 0.001;
  params.d += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_ab_cd(double dx, double dy) {
  params.a += dx * 0.001;
  params.b += dx * 0.001;
  params.c += dy * 0.001;
  params.d += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

static void tool_ac_bd(double dx, double dy) {
  params.a += dx * 0.001;
  params.c += dx * 0.001;
  params.b += dy * 0.001;
  params.d += dy * 0.001;
  gui.update_calc_params_when_convenient = TRUE;
}

void on_widget_toggle(GtkWidget *widget, gpointer user_data) {
  /* Toggle visibility of another widget. This widget should be named
   * toggle_foo to control the visibility of a widget named foo.
   */
  const gchar *name;
  GtkWidget *toggled;

  name = gtk_widget_get_name(widget);
  g_assert(!strncmp(name, "toggle_", 7));
  toggled = glade_xml_get_widget(gui.xml, name+7);

  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
    gtk_widget_show(toggled);
  else
    gtk_widget_hide(toggled);
}

/* The End */
