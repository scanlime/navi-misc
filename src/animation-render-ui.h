/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * animation-render-ui.h - A user interface for preparing an animation
 *                         rendering and viewing its progress.
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

#ifndef __ANIMATION_RENDER_UI_H__
#define __ANIMATION_RENDER_UI_H__

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "iterative-map.h"
#include "de-jong.h"
#include "animation.h"
#include "avi-writer.h"

G_BEGIN_DECLS

#define ANIMATION_RENDER_UI_TYPE            (animation_render_ui_get_type ())
#define ANIMATION_RENDER_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANIMATION_RENDER_UI_TYPE, AnimationRenderUi))
#define ANIMATION_RENDER_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ANIMATION_RENDER_UI_TYPE, AnimationRenderUiClass))
#define IS_ANIMATION_RENDER_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANIMATION_RENDER_UI_TYPE))
#define IS_ANIMATION_RENDER_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ANIMATION_RENDER_UI_TYPE))

typedef struct _AnimationRenderUi      AnimationRenderUi;
typedef struct _AnimationRenderUiClass AnimationRenderUiClass;

struct _AnimationRenderUi {
    GObject object;

    GladeXML *xml;

    Animation *animation;

    const gchar *filename;
    gdouble frame_rate;
    guint width, height, oversample;
    double quality;

    IterativeMap *map;
    AviWriter *avi;
    AnimationIter iter;
    ParameterHolderPair frame;
    gboolean continuation;
    guint idler;

    gdouble elapsed_anim_time;
    gdouble anim_length;
    gboolean render_in_progress;

    gboolean confirm_on;
};

struct _AnimationRenderUiClass {
    GObjectClass parent_class;

    void (* animation_render_ui) (AnimationRenderUi *self);
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType               animation_render_ui_get_type();
AnimationRenderUi*  animation_render_ui_new(Animation *animation);


G_END_DECLS

#endif /* __ANIMATION_RENDER_UI_H__ */

/* The End */
