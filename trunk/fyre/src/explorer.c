/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer.c - An interactive GUI for manipulating a DeJong object and viewing its output
 *
 * Fyre - rendering and interactive exploration of chaotic functions
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

#include "config.h"
#include <string.h>
#include "explorer.h"
#include "parameter-editor.h"
#include "math-util.h"
#include "histogram-view.h"
#include "prefix.h"

static void explorer_class_init  (ExplorerClass *klass);
static void explorer_init        (Explorer *self);
static void explorer_dispose     (GObject *gobject);

static gboolean explorer_auto_limit_update_rate (Explorer *self);
static gboolean limit_update_rate               (GTimeVal *last_update, float max_rate);
static gdouble  explorer_get_iter_speed         (Explorer *self);
static gchar*   explorer_strdup_status          (Explorer *self);
static gchar*   explorer_strdup_speed           (Explorer *self);

static gdouble generate_random_param();

static void on_randomize(GtkWidget *widget, gpointer user_data);
static void on_load_defaults(GtkWidget *widget, gpointer user_data);
static void on_save(GtkWidget *widget, gpointer user_data);
static void on_save_exr(GtkWidget *widget, gpointer user_data);
static void on_quit(GtkWidget *widget, gpointer user_data);
static void on_pause_rendering_toggle(GtkWidget *widget, gpointer user_data);
static void on_load_from_image(GtkWidget *widget, gpointer user_data);
static void on_widget_toggle(GtkWidget *widget, gpointer user_data);
static void on_render_time_changed(GtkWidget *widget, gpointer user_data);
static void on_calculation_finished(IterativeMap *map, gpointer user_data);
static gboolean on_interactive_prefs_delete(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean on_cluster_window_delete(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_about_activate(GtkWidget *widget, Explorer *self);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType explorer_get_type(void) {
    static GType exp_type = 0;

    if (!exp_type) {
	static const GTypeInfo exp_info = {
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

	exp_type = g_type_register_static(G_TYPE_OBJECT, "Explorer", &exp_info, 0);
    }

    return exp_type;
}

static void explorer_class_init(ExplorerClass *klass) {
    GObjectClass *object_class = (GObjectClass*) klass;

    object_class->dispose = explorer_dispose;

    glade_init();
}

static void explorer_init(Explorer *self) {
    self->xml = glade_xml_new (FYRE_DATADIR "/explorer.glade", NULL, NULL);
#ifdef ENABLE_BINRELOC
    if (!self->xml)
	self->xml = glade_xml_new(BR_DATADIR("/fyre/explorer.glade"), NULL, NULL);
#endif
    self->window = glade_xml_get_widget(self->xml, "explorer_window");

    /* Connect signal handlers */
    glade_xml_signal_connect_data(self->xml, "on_randomize",                    G_CALLBACK(on_randomize),                    self);
    glade_xml_signal_connect_data(self->xml, "on_load_defaults",                G_CALLBACK(on_load_defaults),                self);
    glade_xml_signal_connect_data(self->xml, "on_save",                         G_CALLBACK(on_save),                         self);
    glade_xml_signal_connect_data(self->xml, "on_save_exr",                     G_CALLBACK(on_save_exr),                     self);
    glade_xml_signal_connect_data(self->xml, "on_quit",                         G_CALLBACK(on_quit),                         self);
    glade_xml_signal_connect_data(self->xml, "on_pause_rendering_toggle",       G_CALLBACK(on_pause_rendering_toggle),       self);
    glade_xml_signal_connect_data(self->xml, "on_load_from_image",              G_CALLBACK(on_load_from_image),              self);
    glade_xml_signal_connect_data(self->xml, "on_widget_toggle",                G_CALLBACK(on_widget_toggle),                self);
    glade_xml_signal_connect_data(self->xml, "on_render_time_changed",          G_CALLBACK(on_render_time_changed),          self);
    glade_xml_signal_connect_data(self->xml, "on_interactive_prefs_delete",     G_CALLBACK(on_interactive_prefs_delete),     self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_window_delete",        G_CALLBACK(on_cluster_window_delete),        self);
    glade_xml_signal_connect_data(self->xml, "on_about_activate",               G_CALLBACK(on_about_activate),               self);

#ifndef HAVE_EXR
    /* If we don't have OpenEXR support, gray out the menu item
     * so it sits there taunting the user and not breaking HIG
     */
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "save_image_as_exr"), FALSE);
#endif

    /* Set up the statusbar */
    self->statusbar = GTK_STATUSBAR(glade_xml_get_widget(self->xml, "statusbar"));
    self->render_status_context = gtk_statusbar_get_context_id(self->statusbar, "Rendering status");
    self->speed_timer = g_timer_new();
}

static void explorer_dispose(GObject *gobject) {
    Explorer *self = EXPLORER(gobject);

    explorer_dispose_animation(self);
    explorer_dispose_cluster(self);

    if (self->speed_timer) {
	g_timer_destroy(self->speed_timer);
	self->speed_timer = NULL;
    }

    if (self->map) {
	g_object_unref(self->map);
	self->map = NULL;
    }
}

Explorer* explorer_new(IterativeMap *map, Animation *animation) {
    Explorer *self = EXPLORER(g_object_new(explorer_get_type(), NULL));
    GtkWidget *editor, *window, *scroll;
    GtkRequisition win_req;

    self->animation = ANIMATION(g_object_ref(animation));
    self->map = ITERATIVE_MAP(g_object_ref(map));

    /* Create the parameter editor */
    editor = parameter_editor_new(PARAMETER_HOLDER(map));
    gtk_box_pack_start(GTK_BOX(glade_xml_get_widget(self->xml, "parameter_editor_box")),
		       editor, FALSE, FALSE, 0);
    gtk_widget_show_all(editor);

    /* Create the view */
    self->view = histogram_view_new(HISTOGRAM_IMAGER(map));
    gtk_container_add(GTK_CONTAINER(glade_xml_get_widget(self->xml, "drawing_area_viewport")), self->view);
    gtk_widget_show_all(self->view);

    /* Set the initial render time */
    on_render_time_changed(glade_xml_get_widget(self->xml, "render_time"), self);

    explorer_init_animation(self);
    explorer_init_tools(self);
    explorer_init_cluster(self);

    /* Start the iterative map rendering in the background, and get a callback every time a block
     * of calculations finish so we can update the GUI.
     */
    iterative_map_start_calculation(self->map);
    g_signal_connect(G_OBJECT(self->map), "calculation-finished",
		     G_CALLBACK(on_calculation_finished), self);

    /* Set the window's default size to include our default image size.
     * The cleanest way I know of to do this is to set the scrolled window's scrollbar policies
     * to 'never' and get the window's size requests, set them back to automatic, then set the
     * default size to that size request.
     */
    window = glade_xml_get_widget(self->xml, "explorer_window");
    scroll = glade_xml_get_widget(self->xml, "main_scrolledwindow");
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_widget_size_request(window, &win_req);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_window_set_default_size(GTK_WINDOW(window), win_req.width, win_req.height);
    gtk_widget_show(window);

    return self;
}


/************************************************************************************/
/*********************************************************************** Clustering */
/************************************************************************************/

#ifndef HAVE_GNET
/* Fake cluster functions, if gnet support is not available */

void      explorer_init_cluster          (Explorer *self)
{
    /* If we have no cluster support, disable that menu item */
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "toggle_cluster_window"), FALSE);
}

void      explorer_dispose_cluster       (Explorer *self) {}

#endif /* !HAVE_GNET */


/************************************************************************************/
/*********************************************************************** Parameters */
/************************************************************************************/

static gdouble generate_random_param() {
    return uniform_variate() * 12 - 6;
}

static void on_randomize(GtkWidget *widget, gpointer user_data) {
    Explorer *self = EXPLORER(user_data);

    g_object_set(self->map,
		 "a", generate_random_param(),
		 "b", generate_random_param(),
		 "c", generate_random_param(),
		 "d", generate_random_param(),
		 NULL);
}

static void on_load_defaults(GtkWidget *widget, gpointer user_data) {
    Explorer *self = EXPLORER(user_data);
    parameter_holder_reset_to_defaults(PARAMETER_HOLDER(self->map));
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
    g_assert(!strncmp((void *) name, "toggle_", 7));
    toggled = glade_xml_get_widget(self->xml, name+7);

    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
	gtk_widget_show(toggled);
    else
	gtk_widget_hide(toggled);
}

#if (GTK_CHECK_VERSION(2, 4, 0))
static void
update_image_preview (GtkFileChooser *chooser, GtkImage *image) {
    GdkPixbuf *image_pixbuf, *temp;
    static GdkPixbuf *emblem_pixbuf = NULL;
    gchar *filename;
    gboolean have_preview;
    GdkPixmap *pixmap;
    gint width, height;

    if (emblem_pixbuf == NULL) {
	emblem_pixbuf = gdk_pixbuf_new_from_file (FYRE_DATADIR "/metadata-emblem.png", NULL);
#ifdef ENABLE_BINRELOC
	if (!emblem_pixbuf)
	    emblem_pixbuf = gdk_pixbuf_new_from_file (BR_DATADIR ("/metadata-emblem.png"), NULL);
#endif
    }

    filename = gtk_file_chooser_get_filename (chooser);

    image_pixbuf = gdk_pixbuf_new_from_file_at_size (filename, 112, 112, NULL);
    width = gdk_pixbuf_get_width (image_pixbuf);
    height = gdk_pixbuf_get_height (image_pixbuf);
    have_preview = (image_pixbuf != NULL);

    pixmap = gdk_pixmap_new (GTK_WIDGET (image)->window, width + 16, height + 16, -1);
    gdk_draw_rectangle (pixmap, GTK_WIDGET (image)->style->bg_gc[GTK_STATE_NORMAL], TRUE, 0, 0, width + 16, height + 16);
    gdk_draw_pixbuf (pixmap, NULL, image_pixbuf, 0, 0, 0, 0, width - 1, height - 1, GDK_RGB_DITHER_NONE, 0, 0);

    temp = gdk_pixbuf_new_from_file (filename, NULL);
    if (temp) {
        if (gdk_pixbuf_get_option (temp, "tEXt::fyre_params"))
            gdk_draw_pixbuf (pixmap, NULL, emblem_pixbuf, 0, 0, width - 16, height - 16, 31, 31, GDK_RGB_DITHER_NONE, 0, 0);
        else if (gdk_pixbuf_get_option (temp, "tEXt::de_jong_params"))
            gdk_draw_pixbuf (pixmap, NULL, emblem_pixbuf, 0, 0, width - 16, height - 16, 31, 31, GDK_RGB_DITHER_NONE, 0, 0);
        gdk_pixbuf_unref (temp);
    }

    if (image_pixbuf)
	gdk_pixbuf_unref (image_pixbuf);

    gtk_image_set_from_pixmap (GTK_IMAGE (image), pixmap, NULL);
    gdk_pixmap_unref (pixmap);
    gtk_file_chooser_set_preview_widget_active (chooser, have_preview);
}
#endif

static void on_load_from_image (GtkWidget *widget, gpointer user_data) {
    Explorer *self = EXPLORER (user_data);
    GtkWidget *dialog, *image;
    GError *error = NULL;
    gchar *filename = NULL;

#if (GTK_CHECK_VERSION(2, 4, 0))
    dialog = gtk_file_chooser_dialog_new ("Open Image Parameters",
		                          GTK_WINDOW (glade_xml_get_widget (self->xml, "explorer_window")),
					  GTK_FILE_CHOOSER_ACTION_OPEN,
					  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					  GTK_STOCK_OK, GTK_RESPONSE_OK,
					  NULL);
    image = gtk_image_new ();
    gtk_file_chooser_set_preview_widget (GTK_FILE_CHOOSER (dialog), image);
    g_signal_connect (G_OBJECT (dialog), "update-preview", G_CALLBACK (update_image_preview), image);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	histogram_imager_load_image_file (HISTOGRAM_IMAGER (self->map), filename, &error);
    }
#else
    dialog = gtk_file_selection_new ("Open Image Parameters");

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
	filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (dialog));
	histogram_imager_load_image_file (HISTOGRAM_IMAGER (self->map), filename, &error);
    }
#endif
    gtk_widget_destroy (dialog);

    if (error) {
	GtkWidget *dialog, *label;
	gchar *text;

	dialog = glade_xml_get_widget (self->xml, "error dialog");
	label = glade_xml_get_widget (self->xml, "error label");

	text = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">Could not load \"%s\"</span>\n\n%s", filename, error->message);
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	g_error_free (error);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_hide_all (dialog);
    }
    g_free (filename);
}

static void on_save (GtkWidget *widget, gpointer user_data) {
    Explorer *self = EXPLORER (user_data);
    GtkWidget *dialog;
    GError *error = NULL;
    gchar *filename = NULL;

#if (GTK_CHECK_VERSION(2, 4, 0))
    dialog = gtk_file_chooser_dialog_new ("Save Image",
		                          GTK_WINDOW (glade_xml_get_widget (self->xml, "explorer_window")),
					  GTK_FILE_CHOOSER_ACTION_SAVE,
					  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					  GTK_STOCK_OK, GTK_RESPONSE_OK,
					  NULL);
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), "rendering.png");
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	histogram_imager_save_image_file (HISTOGRAM_IMAGER (self->map), filename, &error);
    }
#else
    dialog = gtk_file_selection_new ("Save Image");
    gtk_file_selection_set_filename (GTK_FILE_SELECTION (dialog), "rendering.png");

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
	const gchar *filename;
	filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (dialog));
	histogram_imager_save_image_file (HISTOGRAM_IMAGER (self->map), filename, &error);
    }
#endif
    gtk_widget_destroy (dialog);

    if (error) {
	GtkWidget *dialog, *label;
	gchar *text;

	dialog = glade_xml_get_widget (self->xml, "error dialog");
	label = glade_xml_get_widget (self->xml, "error label");

	text = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">Could not save \"%s\"</span>\n\n%s", filename, error->message);
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	g_error_free (error);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_hide_all (dialog);
    }

    if (filename)
	g_free (filename);
}

static void on_save_exr (GtkWidget *widget, gpointer user_data) {
#ifdef HAVE_EXR
    Explorer *self = EXPLORER (user_data);
    GtkWidget *dialog;

#if (GTK_CHECK_VERSION(2, 4, 0))
    dialog = gtk_file_chooser_dialog_new ("Save OpenEXR Image",
		                          GTK_WINDOW (glade_xml_get_widget (self->xml, "explorer_window")),
					  GTK_FILE_CHOOSER_ACTION_SAVE,
					  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					  GTK_STOCK_OK, GTK_RESPONSE_OK,
					  NULL);
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), "rendering.exr");

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
        gchar *filename;
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	exr_save_image_file (HISTOGRAM_IMAGER (self->map), filename);
	g_free (filename);
    }
#else
    dialog = gtk_file_selection_new ("Save OpenEXR Image");
    gtk_file_selection_set_filename (GTK_FILE_SELECTION (dialog), "rendering.exr");

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
	const gchar *filename;
	filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (dialog));
	exr_save_image_file (HISTOGRAM_IMAGER (self->map), filename);
    }
#endif /* GTK_CHECK_VERSION */
    gtk_widget_destroy (dialog);
#endif /* HAVE_EXR */
}

static void on_render_time_changed(GtkWidget *widget, gpointer user_data) {
    double v = gtk_range_get_adjustment(GTK_RANGE(widget))->value;
    Explorer *self = EXPLORER(user_data);
    self->map->render_time = v / 1000.0;  /* Milliseconds to seconds */
}

static gboolean on_interactive_prefs_delete(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    /* Just hide the window when the user tries to close it */
    Explorer *self = EXPLORER(user_data);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(self->xml, "toggle_interactive_prefs")), FALSE);
    return TRUE;
}

static gboolean on_cluster_window_delete(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    /* Just hide the window when the user tries to close it */
    Explorer *self = EXPLORER(user_data);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(self->xml, "toggle_cluster_window")), FALSE);
    return TRUE;
}


/************************************************************************************/
/************************************************************************ Rendering */
/************************************************************************************/

static void on_pause_rendering_toggle(GtkWidget *widget, gpointer user_data) {
    Explorer *self = EXPLORER(user_data);
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
	iterative_map_stop_calculation(self->map);
    else
	iterative_map_start_calculation(self->map);

    /* Update the speed shown in the status bar */
    self->status_dirty_flag = TRUE;
    explorer_update_gui(self);
}

static void on_calculation_finished(IterativeMap *map, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    explorer_update_gui(self);
    explorer_update_animation(self);
    explorer_update_tools(self);
}

void explorer_run_iterations(Explorer *self) {
    iterative_map_calculate_timed(self->map, self->map->render_time);
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

    elapsed = histogram_imager_get_elapsed_time(HISTOGRAM_IMAGER(self->map));
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
    if (!(HISTOGRAM_IMAGER(self->map)->render_dirty_flag || self->status_dirty_flag)) {
	if (explorer_auto_limit_update_rate(self))
	    return;
    }

    /* We don't want to update the status bar if we're trying to show rendering changes quickly */
    if (!HISTOGRAM_IMAGER(self->map)->render_dirty_flag) {
	gchar *status = explorer_strdup_status(self);
	if (self->render_status_message_id)
	    gtk_statusbar_remove(self->statusbar, self->render_status_context, self->render_status_message_id);
	self->render_status_message_id = gtk_statusbar_push(self->statusbar, self->render_status_context, status);
	g_free(status);
	self->status_dirty_flag = FALSE;
    }

    histogram_view_update(HISTOGRAM_VIEW(self->view));
}

static gchar*   explorer_strdup_status (Explorer *self)
{
    gchar *status;
    gchar *speed = explorer_strdup_speed(self);

    status = g_strdup_printf("Iterations:    %.3e    \t"
			     "Speed:    %s    \t"
			     "Peak density:    %ld    \t"
			     "Current tool: %s",
			     self->map->iterations,
			     speed,
			     HISTOGRAM_IMAGER(self->map)->peak_density,
			     self->current_tool);

    g_free(speed);
    return status;
}

static gchar*   explorer_strdup_speed (Explorer *self)
{
    if (iterative_map_is_calculation_running(self->map))
	return g_strdup_printf("%.3e/sec", explorer_get_iter_speed(self));
    else
	return g_strdup("Paused");
}

static gdouble  explorer_get_iter_speed(Explorer *self)
{
    double elapsed = g_timer_elapsed(self->speed_timer, NULL);
    double iter_diff = self->map->iterations - self->last_iterations;

    if (iter_diff < 0) {
	/* Calculation restarted */
	g_timer_start(self->speed_timer);
	self->last_iterations = self->map->iterations;
    }
    else if (iter_diff > 0 && elapsed > 1.0) {
	g_timer_start(self->speed_timer);
	self->last_iterations = self->map->iterations;
	self->iter_speed = iter_diff / elapsed;
    }
    return self->iter_speed;
}

/************************************************************************************/
/********************************************************************* About Dialog */
/************************************************************************************/

static void on_about_close(GtkWidget *widget, Explorer *self)
{
    GtkWidget *dialog;
    dialog = glade_xml_get_widget(self->xml, "about_window");
    gtk_widget_hide_all(dialog);
}

static void on_about_activate(GtkWidget *widget, Explorer *self)
{
    static gboolean init = FALSE;
    GtkWidget *dialog;

    dialog = glade_xml_get_widget (self->xml, "about_window");
    if (!init) {
        GtkWidget *close, *image, *label;
	GdkPixbuf *pixbuf;
	gchar *text;

        close = glade_xml_get_widget(self->xml, "about close");
	image = glade_xml_get_widget(self->xml, "about image");
	label = glade_xml_get_widget(self->xml, "about version");

	pixbuf = gdk_pixbuf_new_from_file(FYRE_DATADIR "/wicker-shoelace.png", NULL);
#ifdef ENABLE_BINRELOC
	if (!pixbuf)
		pixbuf = gdk_pixbuf_new_from_file(BR_DATADIR("/fyre/wicker-shoelace.png"), NULL);
#endif
        if (pixbuf)
	    gtk_image_set_from_pixbuf(GTK_IMAGE (image), pixbuf);

	g_signal_connect(G_OBJECT (dialog), "delete-event", G_CALLBACK(on_about_close), self);
	g_signal_connect(G_OBJECT (close), "clicked", G_CALLBACK(on_about_close), self);

	text = g_strdup_printf("<span size=\"xx-large\" weight=\"bold\">Fyre %s</span>", VERSION);
	gtk_label_set_markup(GTK_LABEL(label), text);
	g_free(text);
	init = TRUE;
    }
    gtk_widget_show_all(dialog);
}

/* The End */
