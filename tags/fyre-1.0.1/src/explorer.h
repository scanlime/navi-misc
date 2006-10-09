/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer.h - An interactive GUI for manipulating an IterativeMap
 *              object and viewing its output
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

#ifndef __EXPLORER_H__
#define __EXPLORER_H__

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "animation.h"
#include "animation-render-ui.h"
#include "screensaver.h"

#ifdef HAVE_GNET
#include "cluster-model.h"
#endif

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

    IterativeMap*        map;
    Animation*           animation;
    AnimationRenderUi*   render_window;

    GladeXML*            xml;
    GtkWidget*           window;
    GtkWidget*           view;
    GtkWidget*           fgcolor_button;
    GtkWidget*           bgcolor_button;

    ScreenSaver*         about_image;

    GtkStatusbar*        statusbar;
    guint                render_status_message_id;
    guint                render_status_context;
    gboolean             status_dirty_flag;

    GTimer*              auto_update_rate_timer;
    GTimer*              status_update_rate_timer;
    GTimer*              speed_timer;
    double               last_iterations;
    double               iter_speed;

    gchar*               current_tool;
    gboolean             tool_active;
    double               last_mouse_x;
    double               last_mouse_y;
    double               last_click_x;
    double               last_click_y;
    GTimeVal             last_tool_idle_update;

    GtkWidget*           anim_curve;
    gboolean             allow_transition_changes;
    gboolean             selecting_keyframe;

    gboolean             seeking_animation;
    gboolean             seeking_animation_transition;
    gboolean             playing_animation;

    gboolean             unpause_on_restore;

    GTimeVal             last_anim_frame_time;

    GQueue*              history_queue;
    guint                history_timer;
    GList*               history_current_link;
    gboolean             history_freeze;

#ifdef HAVE_GNET
    ClusterModel*        cluster_model;
#endif
};

struct _ExplorerClass {
    GObjectClass         parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType      explorer_get_type();
Explorer*  explorer_new (IterativeMap *map, Animation *animation);


/************************************************************************************/
/***************************************************************** Internal methods */
/************************************************************************************/

void      explorer_init_tools            (Explorer *self);
gboolean  explorer_update_tools          (Explorer *self);

void      explorer_init_animation        (Explorer *self);
void      explorer_dispose_animation     (Explorer *self);
void      explorer_update_animation      (Explorer *self);

void      explorer_init_cluster          (Explorer *self);
void      explorer_dispose_cluster       (Explorer *self);

void      explorer_init_about            (Explorer *self);

void      explorer_init_history          (Explorer *self);
void      explorer_dispose_history       (Explorer *self);

void      explorer_run_iterations        (Explorer *self);
void      explorer_update_gui            (Explorer *self);

void      explorer_force_pause           (Explorer *self);
void      explorer_restore_pause         (Explorer *self);

G_END_DECLS

#endif /* __EXPLORER_H__ */

/* The End */
