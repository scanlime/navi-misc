/*
 * explorer.h - An interactive GUI for manipulating a DeJong object and viewing its output
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

#ifndef __EXPLORER_H__
#define __EXPLORER_H__

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "de-jong.h"
#include "animation.h"

G_BEGIN_DECLS

#define EXPLORER_TYPE            (explorer_get_type ())
#define EXPLORER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPLORER_TYPE, Explorer))
#define EXPLORER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPLORER_TYPE, ExplorerClass))
#define IS_EXPLORER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPLORER_TYPE))
#define IS_EXPLORER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPLORER_TYPE))

typedef struct _Explorer      Explorer;
typedef struct _ExplorerClass ExplorerClass;

struct _Explorer {
  GObject object;

  DeJong *dejong;
  Animation *animation;

  GladeXML *xml;
  GtkWidget *window;

  GtkWidget *drawing_area;
  GdkGC *gc;

  GtkStatusbar *statusbar;
  guint render_status_message_id;
  guint render_status_context;

  guint idler;
  gboolean setting_params;
  gboolean just_resized;
  GTimeVal last_update;

  gchar* current_tool;
  gboolean tool_active;
  double last_mouse_x, last_mouse_y;
  double last_click_x, last_click_y;
  GTimeVal last_tool_idle_update;

  GtkWidget *fgcolor_button, *bgcolor_button;
  GtkWidget *anim_curve;
  gboolean allow_transition_changes;

  gboolean seeking_animation;
  gboolean playing_animation;
  GTimeVal last_anim_frame_time;
};

struct _ExplorerClass {
  GObjectClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType      explorer_get_type();
Explorer*  explorer_new(DeJong *dejong, Animation *animation);


G_END_DECLS

#endif /* __EXPLORER_H__ */

/* The End */
