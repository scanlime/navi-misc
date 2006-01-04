/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * screensaver.h - A self-running Fyre screen saver
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

#ifndef __SCREENSAVER_H__
#define __SCREENSAVER_H__

#include <gtk/gtk.h>
#include "animation.h"
#include "iterative-map.h"

G_BEGIN_DECLS

#define SCREENSAVER_TYPE            (screensaver_get_type ())
#define SCREENSAVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SCREENSAVER_TYPE, ScreenSaver))
#define SCREENSAVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SCREENSAVER_TYPE, ScreenSaverClass))
#define IS_SCREENSAVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCREENSAVER_TYPE))
#define IS_SCREENSAVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SCREENSAVER_TYPE))

typedef struct _ScreenSaver      ScreenSaver;
typedef struct _ScreenSaverClass ScreenSaverClass;

struct _ScreenSaver {
    GObject object;

    IterativeMap *map;
    Animation *animation;

    gdouble framerate;
    IterativeMap **frame_renders;
    ParameterHolderPair *frame_parameters;
    int num_frames, current_frame;
    int direction;

    GtkWidget *view;

    guint idler;
};

struct _ScreenSaverClass {
    GObjectClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType         screensaver_get_type ();
ScreenSaver*  screensaver_new      (IterativeMap *map, Animation *animation);
void          screensaver_start    (ScreenSaver *self);
void          screensaver_stop     (ScreenSaver *self);

G_END_DECLS

#endif /* __SCREENSAVER_H__ */

/* The End */
