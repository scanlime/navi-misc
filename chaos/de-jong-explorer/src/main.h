/*
 * de-jong.h - Shared declarations
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

#ifndef __DE_JONG_H_
#define __DE_JONG_H__

struct gui_state {
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

  gboolean update_calc_params_when_convenient;
  gboolean update_render_params_when_convenient;

  gchar* current_tool;
  double last_mouse_x, last_mouse_y;

  struct {
    GtkListStore *keyframe_list;
    GtkTreeView *keyframe_view;
  } anim;
};

extern struct gui_state gui;

/* ui-main.c */
void interactive_main(DeJong* dejong, int argc, char **argv);
void restart_rendering();
void write_gui_params();

/* ui-animation.c */
void animation_ui_init();


#endif /* __DE_JONG_MAIN_H__ */

/* The End */
