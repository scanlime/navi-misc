/*
 * explorer-tools.c - Implementation for the GUI 'tools' that allow
 *                    direct interaction with the mouse.
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

#include "explorer.h"
#include <stdlib.h>
#include <math.h>


typedef struct _ToolInput {
  double delta_x, delta_y;
  double absolute_x, absolute_y;
  double click_relative_x, click_relative_y;
  double delta_time;
  GdkModifierType state;
} ToolInput;

typedef void (ToolHandler)(Explorer *self, ToolInput *i);

typedef enum {
  TOOL_USE_MOTION_EVENTS = 1 << 0,
  TOOL_USE_IDLE          = 1 << 1,
} ToolFlags;

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

    /* If we're paused, manually update one frame */
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(self->xml, "pause_menu")))) {
      explorer_run_iterations(self);
      explorer_update_gui(self);
    }
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
  double scale = 5.0 / self->dejong->zoom / HISTOGRAM_IMAGER(self->dejong)->width;
  g_object_set(self->dejong,
	       "xoffset", self->dejong->xoffset + i->delta_x * scale,
	       "yoffset", self->dejong->yoffset + i->delta_y * scale,
	       NULL);
}

static void tool_blur(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "blur_ratio",  self->dejong->blur_ratio  + i->delta_x * 0.002,
	       "blur_radius", self->dejong->blur_radius - i->delta_y * 0.001,
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

  g_object_set(self->dejong,
	       "zoom", self->dejong->zoom - scaled_p * i->delta_time,
	       NULL);
}

static void tool_rotate(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "rotation", (gdouble) (self->dejong->rotation - i->delta_x * 0.0089),
	       NULL);
}

static void tool_exposure_gamma(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "exposure", HISTOGRAM_IMAGER(self->dejong)->exposure - i->delta_y * 0.001,
	       "gamma",    HISTOGRAM_IMAGER(self->dejong)->gamma    + i->delta_x * 0.001,
	       NULL);
}

static void tool_a_b(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "a", self->dejong->param.a + i->delta_x * 0.001,
	       "b", self->dejong->param.b + i->delta_y * 0.001,
	       NULL);
}

static void tool_a_c(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "a", self->dejong->param.a + i->delta_x * 0.001,
	       "c", self->dejong->param.c + i->delta_y * 0.001,
	       NULL);
}

static void tool_a_d(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "a", self->dejong->param.a + i->delta_x * 0.001,
	       "d", self->dejong->param.d + i->delta_y * 0.001,
	       NULL);
}

static void tool_b_c(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "b", self->dejong->param.b + i->delta_x * 0.001,
	       "c", self->dejong->param.c + i->delta_y * 0.001,
	       NULL);
}

static void tool_b_d(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "b", self->dejong->param.b + i->delta_x * 0.001,
	       "d", self->dejong->param.d + i->delta_y * 0.001,
	       NULL);
}

static void tool_c_d(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "c", self->dejong->param.c + i->delta_x * 0.001,
	       "d", self->dejong->param.d + i->delta_y * 0.001,
	       NULL);
}

static void tool_ab_cd(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "a", self->dejong->param.a + i->delta_x * 0.001,
	       "b", self->dejong->param.b + i->delta_x * 0.001,
	       "c", self->dejong->param.c + i->delta_y * 0.001,
	       "d", self->dejong->param.d + i->delta_y * 0.001,
	       NULL);
}

static void tool_ac_bd(Explorer *self, ToolInput *i) {
  g_object_set(self->dejong,
	       "a", self->dejong->param.a + i->delta_x * 0.001,
	       "b", self->dejong->param.b + i->delta_y * 0.001,
	       "c", self->dejong->param.c + i->delta_x * 0.001,
	       "d", self->dejong->param.d + i->delta_y * 0.001,
	       NULL);
}

/* The End */
