/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer-tools.c - Implementation for the GUI 'tools' that allow
 *                    direct interaction with the mouse.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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
#include "de-jong.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef void (ToolHandler)(Explorer *self, ToolInput *i);

typedef struct _ToolInfo {
    gchar *menu_name;
    ToolHandler *handler;
    ToolFlags flags;
} ToolInfo;

static const ToolInfo* explorer_get_current_tool(Explorer *self);
static void explorer_fill_toolinput_relative_positions(Explorer *self, ToolInput *ti);

static gboolean on_motion_notify(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean on_button_press(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean on_button_release(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_tool_activate(GtkWidget *widget, gpointer user_data);

static void tool_grab(Explorer *self, ToolInput *i);
static void tool_blur(Explorer *self, ToolInput *i);
static void tool_zoom(Explorer *self, ToolInput *i);
static void tool_rotate(Explorer *self, ToolInput *i);
static void tool_exposure_gamma(Explorer *self, ToolInput *i);
static void tool_a_b(Explorer *self, ToolInput *i);
static void tool_a_c(Explorer *self, ToolInput *i);
static void tool_a_d(Explorer *self, ToolInput *i);
static void tool_b_c(Explorer *self, ToolInput *i);
static void tool_b_d(Explorer *self, ToolInput *i);
static void tool_c_d(Explorer *self, ToolInput *i);
static void tool_ab_cd(Explorer *self, ToolInput *i);
static void tool_ac_bd(Explorer *self, ToolInput *i);
static void tool_initial_offset(Explorer *self, ToolInput *i);
static void tool_initial_scale(Explorer *self, ToolInput *i);


/* A table of tool handlers and menu item names */
static const ToolInfo tool_table[] = {

    {"tool_grab",           tool_grab,           TOOL_USE_MOTION_EVENTS},
    {"tool_blur",           tool_blur,           TOOL_USE_MOTION_EVENTS},
    {"tool_zoom",           tool_zoom,           TOOL_USE_IDLE},
    {"tool_rotate",         tool_rotate,         TOOL_USE_MOTION_EVENTS},
    {"tool_exposure_gamma", tool_exposure_gamma, TOOL_USE_MOTION_EVENTS},
    {"tool_a_b",            tool_a_b,            TOOL_USE_MOTION_EVENTS},
    {"tool_a_c",            tool_a_c,            TOOL_USE_MOTION_EVENTS},
    {"tool_a_d",            tool_a_d,            TOOL_USE_MOTION_EVENTS},
    {"tool_b_c",            tool_b_c,            TOOL_USE_MOTION_EVENTS},
    {"tool_b_d",            tool_b_d,            TOOL_USE_MOTION_EVENTS},
    {"tool_c_d",            tool_c_d,            TOOL_USE_MOTION_EVENTS},
    {"tool_ab_cd",          tool_ab_cd,          TOOL_USE_MOTION_EVENTS},
    {"tool_ac_bd",          tool_ac_bd,          TOOL_USE_MOTION_EVENTS},
    {"tool_initial_offset", tool_initial_offset, TOOL_USE_MOTION_EVENTS},
    {"tool_initial_scale",  tool_initial_scale,  TOOL_USE_MOTION_EVENTS},

    {NULL,},
};


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

void explorer_init_tools(Explorer *self) {
    gtk_widget_add_events(self->view,
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_BUTTON_MOTION_MASK |
			  GDK_POINTER_MOTION_HINT_MASK);

    glade_xml_signal_connect_data(self->xml, "on_tool_activate",  G_CALLBACK(on_tool_activate),  self);

    g_signal_connect(self->view, "motion_notify_event",  G_CALLBACK(on_motion_notify),  self);
    g_signal_connect(self->view, "button_press_event",   G_CALLBACK(on_button_press),   self);
    g_signal_connect(self->view, "button_release_event", G_CALLBACK(on_button_release), self);

    self->current_tool = "None";
}


/************************************************************************************/
/****************************************************************** Tool Invocation */
/************************************************************************************/

static const ToolInfo* explorer_get_current_tool(Explorer *self) {
    /* Return the current tool, or NULL if no tool is active
     */
    const ToolInfo *current = tool_table;

    for (current=tool_table; current->menu_name; current++) {
	GtkWidget *w = glade_xml_get_widget(self->xml, current->menu_name);
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(w)))
	    return current;
    }
    return NULL;
}

static void explorer_fill_toolinput_relative_positions(Explorer *self, ToolInput *ti) {
    /* Fill in the delta and click-relative positions in a given toolinfo
     */

    /* Compute delta position */
    ti->delta_x = ti->absolute_x - self->last_mouse_x;
    ti->delta_y = ti->absolute_y - self->last_mouse_y;

    /* Compute click-relative position */
    ti->click_relative_x = ti->absolute_x - self->last_click_x;
    ti->click_relative_y = ti->absolute_y - self->last_click_y;
}

static gboolean on_motion_notify(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    Explorer *self = EXPLORER(user_data);
    const ToolInfo *tool = explorer_get_current_tool(self);
    ToolInput ti;
    memset(&ti, 0, sizeof(ti));

    /* Fill in the absolute position and state for the ToolInput */
    if (event->motion.is_hint) {
	gint ix, iy;
	gdk_window_get_pointer(event->motion.window, &ix, &iy, &ti.state);
	ti.absolute_x = ix;
	ti.absolute_y = iy;
    }
    else {
	ti.absolute_x = event->motion.x;
	ti.absolute_y = event->motion.y;
	ti.state = event->motion.state;
    }
    explorer_fill_toolinput_relative_positions(self, &ti);
    self->last_mouse_x = ti.absolute_x;
    self->last_mouse_y = ti.absolute_y;

    if (tool && (tool->flags & TOOL_USE_MOTION_EVENTS)) {
	tool->handler(self, &ti);

	/* Always push through one frame of updates manually
	 * before going on. This serves multiple purposes-
	 * if we're paused, we need this to get any response
	 * at all. This also forces an update to happen even
	 * if the idle handler won't be run for a while, making
	 * the GUI more responsive. This is especially important
	 * under Windows, where the idle handler seems to have
	 * a lower priority.
	 */
	explorer_run_iterations(self);
    }

    return FALSE;
}

static void on_tool_activate(GtkWidget *widget, gpointer user_data) {
    Explorer *self = EXPLORER(user_data);

    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
	gtk_label_get(GTK_LABEL(gtk_bin_get_child(GTK_BIN(widget))), &self->current_tool);

    self->status_dirty_flag = TRUE;
}

static gboolean on_button_press(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    Explorer *self = EXPLORER(user_data);

    self->tool_active = TRUE;
    self->last_mouse_x = event->button.x;
    self->last_mouse_y = event->button.y;
    self->last_click_x = event->button.x;
    self->last_click_y = event->button.y;
    return FALSE;
}

static gboolean on_button_release(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    Explorer *self = EXPLORER(user_data);
    self->tool_active = FALSE;
    return FALSE;
}

gboolean explorer_update_tools(Explorer *self) {
    /* If we're using a tool that needs to be updated when idle, do it.
     * Returns a boolean indicating whether a tool is active or not.
     */
    const ToolInfo *tool = explorer_get_current_tool(self);
    ToolInput ti;
    gint ix, iy;
    GTimeVal now;
    memset(&ti, 0, sizeof(ti));

    gdk_window_get_pointer(self->view->window, &ix, &iy, &ti.state);
    ti.absolute_x = ix;
    ti.absolute_y = iy;
    explorer_fill_toolinput_relative_positions(self, &ti);

    /* Compute the delta time */
    g_get_current_time(&now);
    ti.delta_time = ((now.tv_usec - self->last_tool_idle_update.tv_usec) / 1000000.0 +
		     (now.tv_sec  - self->last_tool_idle_update.tv_sec ));
    self->last_tool_idle_update = now;

    if (tool && self->tool_active && (tool->flags & TOOL_USE_IDLE)) {
	tool->handler(self, &ti);
	return TRUE;
    }
    else {
	return FALSE;
    }
}


/************************************************************************************/
/******************************************************************** Tool handlers */
/************************************************************************************/

static void tool_grab(Explorer *self, ToolInput *i) {
    double scale = 5.0 / DE_JONG(self->map)->zoom / HISTOGRAM_IMAGER(self->map)->width;
    g_object_set(self->map,
		 "xoffset", DE_JONG(self->map)->xoffset + i->delta_x * scale,
		 "yoffset", DE_JONG(self->map)->yoffset + i->delta_y * scale,
		 NULL);
}

static void tool_blur(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "blur_ratio",  DE_JONG(self->map)->blur_ratio  + i->delta_x * 0.002,
		 "blur_radius", DE_JONG(self->map)->blur_radius - i->delta_y * 0.001,
		 NULL);
}

static void tool_zoom(Explorer *self, ToolInput *i) {
    double p, scaled_p;
    const double exponent = 1.4;

    /* Scale the zooming speed nonlinearly with the distance from click location */
    p = i->click_relative_y * 0.01;
    if (p < 0) {
	scaled_p = -pow(-p, exponent);
    }
    else {
	scaled_p = pow(p, exponent);
    }

    g_object_set(self->map,
		 "zoom", DE_JONG(self->map)->zoom - scaled_p * i->delta_time,
		 NULL);
}

static void tool_rotate(Explorer *self, ToolInput *i) {
    /* We're a bit tricky here and also rotate the X and Y offset such that we're
     * rotating around the center of the view, rather than rotating around the
     * center of rendering coordinates.
     */
    double delta_r = -i->delta_x * 0.0089;
    double sin_d_r = sin(delta_r);
    double cos_d_r = cos(delta_r);
    g_object_set(self->map,
		 "rotation", (gdouble) (DE_JONG(self->map)->rotation + delta_r),
		 "xoffset",  (gdouble) ( cos_d_r * DE_JONG(self->map)->xoffset + sin_d_r * DE_JONG(self->map)->yoffset),
		 "yoffset",  (gdouble) (-sin_d_r * DE_JONG(self->map)->xoffset + cos_d_r * DE_JONG(self->map)->yoffset),
		 NULL);
}

static void tool_exposure_gamma(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "exposure", HISTOGRAM_IMAGER(self->map)->exposure - i->delta_y * 0.001,
		 "gamma",    HISTOGRAM_IMAGER(self->map)->gamma    + i->delta_x * 0.001,
		 NULL);
}

static void tool_a_b(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "a", DE_JONG(self->map)->param.a + i->delta_x * 0.001,
		 "b", DE_JONG(self->map)->param.b + i->delta_y * 0.001,
		 NULL);
}

static void tool_a_c(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "a", DE_JONG(self->map)->param.a + i->delta_x * 0.001,
		 "c", DE_JONG(self->map)->param.c + i->delta_y * 0.001,
		 NULL);
}

static void tool_a_d(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "a", DE_JONG(self->map)->param.a + i->delta_x * 0.001,
		 "d", DE_JONG(self->map)->param.d + i->delta_y * 0.001,
		 NULL);
}

static void tool_b_c(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "b", DE_JONG(self->map)->param.b + i->delta_x * 0.001,
		 "c", DE_JONG(self->map)->param.c + i->delta_y * 0.001,
		 NULL);
}

static void tool_b_d(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "b", DE_JONG(self->map)->param.b + i->delta_x * 0.001,
		 "d", DE_JONG(self->map)->param.d + i->delta_y * 0.001,
		 NULL);
}

static void tool_c_d(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "c", DE_JONG(self->map)->param.c + i->delta_x * 0.001,
		 "d", DE_JONG(self->map)->param.d + i->delta_y * 0.001,
		 NULL);
}

static void tool_ab_cd(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "a", DE_JONG(self->map)->param.a + i->delta_x * 0.001,
		 "b", DE_JONG(self->map)->param.b + i->delta_x * 0.001,
		 "c", DE_JONG(self->map)->param.c + i->delta_y * 0.001,
		 "d", DE_JONG(self->map)->param.d + i->delta_y * 0.001,
		 NULL);
}

static void tool_ac_bd(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "a", DE_JONG(self->map)->param.a + i->delta_x * 0.001,
		 "b", DE_JONG(self->map)->param.b + i->delta_y * 0.001,
		 "c", DE_JONG(self->map)->param.c + i->delta_x * 0.001,
		 "d", DE_JONG(self->map)->param.d + i->delta_y * 0.001,
		 NULL);
}

static void tool_initial_offset(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "initial_xoffset", DE_JONG(self->map)->initial_xoffset + i->delta_x * 0.001,
		 "initial_yoffset", DE_JONG(self->map)->initial_yoffset + i->delta_y * 0.001,
		 NULL);
}

static void tool_initial_scale(Explorer *self, ToolInput *i) {
    g_object_set(self->map,
		 "initial_xscale", DE_JONG(self->map)->initial_xscale + i->delta_x * 0.001,
		 "initial_yscale", DE_JONG(self->map)->initial_yscale - i->delta_y * 0.001,
		 NULL);
}

/* The End */
