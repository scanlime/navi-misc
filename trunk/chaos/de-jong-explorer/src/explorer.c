/*
 * explorer.c - An interactive GUI for manipulating a DeJong object and viewing its output
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

#include "explorer.h"
#include "parameter-editor.h"
#include "math-util.h"


static void explorer_class_init(ExplorerClass *klass);
static void explorer_init(Explorer *self);
static void explorer_dispose(GObject *gobject);

static int explorer_idle_handler(gpointer user_data);
static void explorer_get_params(Explorer *self);
static void explorer_resize_notify(HistogramImager *imager, GParamSpec *spec, Explorer *self);
static gboolean explorer_auto_limit_update_rate(Explorer *self);
static void explorer_get_current_keyframe(Explorer *self, GtkTreeIter *iter);
static gboolean limit_update_rate(GTimeVal *last_update, float max_rate);

static gdouble generate_random_param();

static gboolean on_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
static void on_randomize(GtkWidget *widget, gpointer user_data);
static void on_load_defaults(GtkWidget *widget, gpointer user_data);
static void on_save(GtkWidget *widget, gpointer user_data);
static void on_quit(GtkWidget *widget, gpointer user_data);
static void on_pause_rendering_toggle(GtkWidget *widget, gpointer user_data);
static void on_load_from_image(GtkWidget *widget, gpointer user_data);
static gboolean on_viewport_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
static void on_widget_toggle(GtkWidget *widget, gpointer user_data);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType explorer_get_type(void) {
  static GType exp_type = 0;

  if (!exp_type) {
    static const GTypeInfo dj_info = {
      sizeof(ExplorerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) explorer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(Explorer),
      0,
      (GInstanceInitFunc) explorer_init,
    };

    exp_type = g_type_register_static(G_TYPE_OBJECT, "Explorer", &dj_info, 0);
  }

  return exp_type;
}

static void explorer_class_init(ExplorerClass *klass) {
  GObjectClass *object_class = (GObjectClass*) klass;

  object_class->dispose = explorer_dispose;

  glade_init();
}

static void explorer_init(Explorer *self) {
  self->xml = glade_xml_new("data/de-jong-explorer.glade", NULL, NULL);
  self->window = glade_xml_get_widget(self->xml, "explorer_window");

  /* Connect signal handlers */
  glade_xml_signal_connect_data(self->xml, "on_expose",                       G_CALLBACK(on_expose),                       self);
  glade_xml_signal_connect_data(self->xml, "on_randomize",                    G_CALLBACK(on_randomize),                    self);
  glade_xml_signal_connect_data(self->xml, "on_load_defaults",                G_CALLBACK(on_load_defaults),                self);
  glade_xml_signal_connect_data(self->xml, "on_save",                         G_CALLBACK(on_save),                         self);
  glade_xml_signal_connect_data(self->xml, "on_quit",                         G_CALLBACK(on_quit),                         self);
  glade_xml_signal_connect_data(self->xml, "on_pause_rendering_toggle",       G_CALLBACK(on_pause_rendering_toggle),       self);
  glade_xml_signal_connect_data(self->xml, "on_load_from_image",              G_CALLBACK(on_load_from_image),              self);
  glade_xml_signal_connect_data(self->xml, "on_viewport_expose",              G_CALLBACK(on_viewport_expose),              self);
  glade_xml_signal_connect_data(self->xml, "on_widget_toggle",                G_CALLBACK(on_widget_toggle),                self);

  /* Set up the drawing area */
  self->drawing_area = glade_xml_get_widget(self->xml, "main_drawingarea");
  gtk_widget_add_events(self->drawing_area,
			GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON_RELEASE_MASK |
			GDK_BUTTON_MOTION_MASK |
			GDK_POINTER_MOTION_HINT_MASK);
  self->gc = gdk_gc_new(self->drawing_area->window);

  /* Set up the statusbar */
  self->statusbar = GTK_STATUSBAR(glade_xml_get_widget(self->xml, "statusbar"));
  self->render_status_context = gtk_statusbar_get_context_id(self->statusbar, "Rendering status");

  explorer_init_tools(self);
}

static void explorer_dispose(GObject *gobject) {
  Explorer *self = EXPLORER(gobject);

  if (self->dejong) {
    g_object_unref(self->dejong);
    self->dejong = NULL;
  }
  if (self->idler) {
    g_source_remove(self->idler);
    self->idler = 0;
  }

  explorer_dispose_animation(self);
}

Explorer* explorer_new(DeJong *dejong, Animation *animation) {
  Explorer *self = EXPLORER(g_object_new(explorer_get_type(), NULL));
  GtkWidget *editor;

  self->animation = ANIMATION(g_object_ref(animation));
  self->dejong = DE_JONG(g_object_ref(dejong));

  /* Create the parameter editor */
  editor = parameter_editor_new(PARAMETER_HOLDER(dejong));
  gtk_box_pack_start(GTK_BOX(glade_xml_get_widget(self->xml, "parameter_editor_box")),
		     editor, FALSE, FALSE, 0);
  gtk_widget_show_all(editor);

  explorer_init_animation(self);

  /* Do the first resize, and connect to notify signals for future resizes */
  explorer_resize_notify(HISTOGRAM_IMAGER(self->dejong), NULL, self);
  g_signal_connect(self->dejong, "notify::width", G_CALLBACK(explorer_resize_notify), self);
  g_signal_connect(self->dejong, "notify::height", G_CALLBACK(explorer_resize_notify), self);

  self->idler = g_idle_add(explorer_idle_handler, self);
}


/************************************************************************************/
/*********************************************************************** Parameters */
/************************************************************************************/

static gdouble generate_random_param() {
  return uniform_variate() * 12 - 6;
}

static void on_randomize(GtkWidget *widget, gpointer user_data) {
  Explorer *self = EXPLORER(user_data);

  g_object_set(self->dejong,
	       "a", generate_random_param(),
	       "b", generate_random_param(),
	       "c", generate_random_param(),
	       "d", generate_random_param(),
	       NULL);
}

static void on_load_defaults(GtkWidget *widget, gpointer user_data) {
  Explorer *self = EXPLORER(user_data);
  parameter_holder_reset_to_defaults(PARAMETER_HOLDER(self->dejong));
}


/************************************************************************************/
/******************************************************************** Misc GUI goop */
/************************************************************************************/

static void on_quit(GtkWidget *widget, gpointer user_data) {
  gtk_main_quit();
}

static void on_widget_toggle(GtkWidget *widget, gpointer user_data) {
  /* Toggle visibility of another widget. This widget should be named
   * toggle_foo to control the visibility of a widget named foo.
   */
  Explorer *self = EXPLORER(user_data);
  const gchar *name;
  GtkWidget *toggled;

  name = gtk_widget_get_name(widget);
  g_assert(!strncmp(name, "toggle_", 7));
  toggled = glade_xml_get_widget(self->xml, name+7);

  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
    gtk_widget_show(toggled);
  else
    gtk_widget_hide(toggled);
}

static void on_load_from_image(GtkWidget *widget, gpointer user_data) {
  Explorer *self = EXPLORER(user_data);
  GtkWidget *dialog;

  dialog = gtk_file_selection_new("Open Image Parameters");

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    const gchar *filename;
    filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog));
    histogram_imager_load_image_file(HISTOGRAM_IMAGER(self->dejong), filename);
  }
  gtk_widget_destroy(dialog);
}

static void on_save(GtkWidget *widget, gpointer user_data) {
  Explorer *self = EXPLORER(user_data);
  GtkWidget *dialog;

  dialog = gtk_file_selection_new("Save PNG Image");
  gtk_file_selection_set_filename(GTK_FILE_SELECTION(dialog), "rendering.png");

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    const gchar *filename;
    filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog));
    histogram_imager_save_image_file(HISTOGRAM_IMAGER(self->dejong), filename);
  }
  gtk_widget_destroy(dialog);
}


/************************************************************************************/
/************************************************************************ Rendering */
/************************************************************************************/

static void on_pause_rendering_toggle(GtkWidget *widget, gpointer user_data) {
  Explorer *self = EXPLORER(user_data);
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
    g_source_remove(self->idler);
  else
    self->idler = g_idle_add(explorer_idle_handler, self);
}

static int explorer_idle_handler(gpointer user_data) {
  Explorer *self = EXPLORER(user_data);

  explorer_run_iterations(self);
  explorer_update_gui(self);
  explorer_update_animation(self);
  explorer_update_tools(self);
  return 1;
}

void explorer_run_iterations(Explorer *self) {
  /* Run as many blocks of iterations as we can in 13 milliseconds
   */
  GTimer *timer;
  gulong elapsed;

  timer = g_timer_new();
  g_timer_start(timer);

  do {
    de_jong_calculate(self->dejong, 5000);
    g_timer_elapsed(timer, &elapsed);
  } while (elapsed < 13000);

  g_timer_destroy(timer);
}

static void explorer_resize_notify(HistogramImager *imager, GParamSpec *spec, Explorer *self) {
  guint width = HISTOGRAM_IMAGER(self->dejong)->width;
  guint height = HISTOGRAM_IMAGER(self->dejong)->height;

  gtk_widget_set_size_request(self->drawing_area, width, height);

  /* A bit of a hack to make the default window size more sane */
  gtk_widget_set_size_request(glade_xml_get_widget(self->xml, "drawing_area_viewport"),
			      MIN(1000, width+5), MIN(1000, height+5));
  self->just_resized = TRUE;
}

static gboolean limit_update_rate(GTimeVal *last_update, float max_rate) {
  /* Limit the frame rate to the given value. This should be called once per
   * frame, and will return FALSE if it's alright to render another frame,
   * or TRUE otherwise.
   */
  GTimeVal now;
  gulong diff;

  /* Figure out how much time has passed, in milliseconds */
  g_get_current_time(&now);
  diff = ((now.tv_usec - last_update->tv_usec) / 1000 +
	  (now.tv_sec  - last_update->tv_sec ) * 1000);

  if (diff < (1000 / max_rate)) {
    return TRUE;
  }
  else {
    *last_update = now;
    return FALSE;
  }
}

static gboolean explorer_auto_limit_update_rate(Explorer *self) {
  /* Automatically determine a good maximum frame rate based on the current
   * elapsed time, and use limit_update_rate() to limit us to that.
   * Returns 1 if a frame should not be rendered.
   */

  const float initial_rate = 60;
  const float final_rate = 1;
  const float ramp_down_seconds = 3;
  float rate, elapsed;

  elapsed = histogram_imager_get_elapsed_time(HISTOGRAM_IMAGER(self->dejong));
  rate = initial_rate + (final_rate - initial_rate) * (elapsed / ramp_down_seconds);
  if (rate < final_rate)
    rate = final_rate;

  return limit_update_rate(&self->last_gui_update, rate);
}

void explorer_update_gui(Explorer *self) {
  /* If the GUI needs updating, update it. This includes limiting the maximum
   * update rate, updating the iteration count display, and actually rendering
   * frames to the drawing area.
   */

  /* Skip frame rate limiting if we have parameter or status changes to show quickly */
  if (!(HISTOGRAM_IMAGER(self->dejong)->render_dirty_flag || self->status_dirty_flag)) {
    if (explorer_auto_limit_update_rate(self))
      return;
  }

  /* We don't want to update the status bar if we're trying to show rendering changes quickly */
  if (!HISTOGRAM_IMAGER(self->dejong)->render_dirty_flag) {
    gchar *iters = g_strdup_printf("Iterations:    %.3e    \tPeak density:    %d    \tCurrent tool: %s",
				   self->dejong->iterations, HISTOGRAM_IMAGER(self->dejong)->peak_density, self->current_tool);
    if (self->render_status_message_id)
      gtk_statusbar_remove(self->statusbar, self->render_status_context, self->render_status_message_id);
    self->render_status_message_id = gtk_statusbar_push(self->statusbar, self->render_status_context, iters);
    g_free(iters);
  }

  histogram_imager_update_image(HISTOGRAM_IMAGER(self->dejong));

  /* Update our entire drawing area.
   * We use GdkRGB directly here to force ignoring the alpha channel.
   */
  gdk_draw_rgb_32_image(self->drawing_area->window, self->gc, 0, 0,
			HISTOGRAM_IMAGER(self->dejong)->width, HISTOGRAM_IMAGER(self->dejong)->height,
			GDK_RGB_DITHER_NORMAL, gdk_pixbuf_get_pixels(HISTOGRAM_IMAGER(self->dejong)->image),
			HISTOGRAM_IMAGER(self->dejong)->width * 4);

  HISTOGRAM_IMAGER(self->dejong)->render_dirty_flag = FALSE;
  self->status_dirty_flag = FALSE;
}

static gboolean on_viewport_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
  /* After the drawing area is shown, go back to the natural size request */
  Explorer *self = EXPLORER(user_data);
  if (self->just_resized) {
    gtk_widget_set_size_request(widget, -1, -1);
    self->just_resized = FALSE;
  }
  return FALSE;
}

static gboolean on_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
  Explorer *self = EXPLORER(user_data);
  GdkRectangle *rects;
  int n_rects, i;

  if (HISTOGRAM_IMAGER(self->dejong)->image &&
      !HISTOGRAM_IMAGER(self->dejong)->size_dirty_flag) {

    gdk_region_get_rectangles(event->region, &rects, &n_rects);

    for (i=0; i<n_rects; i++) {
      /* Clip this rectangle to the image size, since reading outside our buffer is a bad thing */
      if (rects[i].x + rects[i].width > HISTOGRAM_IMAGER(self->dejong)->width)
	rects[i].width = HISTOGRAM_IMAGER(self->dejong)->width - rects[i].x;
      if (rects[i].y + rects[i].height > HISTOGRAM_IMAGER(self->dejong)->height)
	rects[i].height = HISTOGRAM_IMAGER(self->dejong)->height - rects[i].y;
      if (rects[i].width <= 0 || rects[i].height <= 0)
	continue;

      /* Render a rectangle taken from our pixbuf.
       * We use GdkRGB directly here to force ignoring the alpha channel.
       */
      gdk_draw_rgb_32_image(self->drawing_area->window, self->gc,
			    rects[i].x, rects[i].y,
			    rects[i].width, rects[i].height,
			    GDK_RGB_DITHER_NORMAL,
			    gdk_pixbuf_get_pixels(HISTOGRAM_IMAGER(self->dejong)->image) +
			    rects[i].x * 4 +
			    rects[i].y * HISTOGRAM_IMAGER(self->dejong)->width * 4,
			    HISTOGRAM_IMAGER(self->dejong)->width * 4);
    }

    g_free(rects);
  }
  return FALSE;
}

/* The End */
