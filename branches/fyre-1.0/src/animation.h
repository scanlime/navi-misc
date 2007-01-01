/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * animation.h - A simple keyframe animation system for ParameterHolder objects
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "parameter-holder.h"
#include "chunked-file.h"

G_BEGIN_DECLS

#define ANIMATION_TYPE            (animation_get_type ())
#define ANIMATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANIMATION_TYPE, Animation))
#define ANIMATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ANIMATION_TYPE, AnimationClass))
#define IS_ANIMATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANIMATION_TYPE))
#define IS_ANIMATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ANIMATION_TYPE))

typedef struct _Animation      Animation;
typedef struct _AnimationClass AnimationClass;

struct _Animation {
    GObject object;

    GtkListStore *model;
    gulong next_row_id;
};

struct _AnimationClass {
    GObjectClass parent_class;
};

/* Items in the GdkListStore holding our keyframes */
enum {
    ANIMATION_MODEL_THUMBNAIL,    /* The thumbnail, as a GdkPixbuf */
    ANIMATION_MODEL_PARAMS,       /* The parameters, serialized to a string */
    ANIMATION_MODEL_DURATION,     /* The duration of the following transition, in seconds */
    ANIMATION_MODEL_SPLINE,       /* The interpolation spline, a boxed Spline instance */
    ANIMATION_MODEL_ROW_ID,       /* A unique ID for this row */
    ANIMATION_MODEL_BIFURCATION,  /* Cached BifurcationDiagram instance for this keyframe */
};

typedef struct _AnimationIter {
    GtkTreeIter keyframe;
    gboolean valid;
    gdouble absolute_time;
    gdouble time_after_keyframe;
} AnimationIter;

typedef struct {
    Animation *self;
    GtkTreeIter iter;
} AnimChunkState;


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType        animation_get_type              ();
Animation*   animation_new                   ();
Animation*   animation_copy                  (Animation           *self);
void         animation_clear                 (Animation           *self);

/* Persistence */
void         animation_load_file             (Animation           *self,
					      const gchar         *filename);
void         animation_save_file             (Animation           *self,
					      const gchar         *filename);

/* A chunk source and ChunkCallback that can be used to serialize or
 * copy an animation. These are used internally to implement animation_load_file,
 * animation_save_file, and animation_copy.
 */
void         animation_generate_chunks       (Animation           *self,
					      ChunkCallback        callback,
					      gpointer             user_data);
void         animation_store_chunk           (AnimChunkState      *state,
					      ChunkType            type,
					      gsize                length,
					      const guchar        *data);

/* Keyframe manipulation
 * Normal GtkTreeIters are used to refer to keyframes.
 */
void         animation_keyframe_store        (Animation           *self,
					      GtkTreeIter         *iter,
					      ParameterHolder     *key);
void         animation_keyframe_load         (Animation           *self,
					      GtkTreeIter         *iter,
					      ParameterHolder     *key);
void         animation_keyframe_append       (Animation           *self,
					      ParameterHolder     *key);
gdouble      animation_keyframe_get_time     (Animation           *self,
					      GtkTreeIter         *iter);
gulong       animation_keyframe_get_id       (Animation           *self,
					      GtkTreeIter         *iter);
gboolean     animation_keyframe_find_by_id   (Animation           *self,
					      gulong               id,
					      GtkTreeIter         *iter);

/* Animation iterators
 * Iterators can seek through an animation using wallclock-time,
 * and they are used to extract parameter sets for rendering.
 * Animation iters can be placed anywhere, not just at a keyframe.
 */
gdouble      animation_get_length            (Animation           *self);
void         animation_iter_get_first        (Animation           *self,
					      AnimationIter       *iter);
void         animation_iter_seek             (Animation           *self,
					      AnimationIter       *iter,
					      gdouble              absolute_time);
void         animation_iter_seek_relative    (Animation           *self,
					      AnimationIter       *iter,
					      gdouble delta_time);
void         animation_iter_load             (Animation           *self,
					      AnimationIter       *iter,
					      ParameterHolder     *inbetween);
gboolean     animation_iter_read_frame       (Animation           *self,
					      AnimationIter       *iter,
					      ParameterHolderPair *frame,
					      double               frame_rate);

G_END_DECLS

#endif /* __ANIMATION_H__ */

/* The End */
