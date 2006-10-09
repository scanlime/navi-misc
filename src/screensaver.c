/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * screensaver.c - A self-running Fyre "screen saver" that progressively
 *                 renders a looping animation.
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

#include "screensaver.h"
#include "histogram-view.h"
#include "de-jong.h"

static void screensaver_class_init(ScreenSaverClass *klass);
static void screensaver_init(ScreenSaver *self);
static void screensaver_dispose(GObject *gobject);

static int screensaver_idle_handler(gpointer user_data);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType screensaver_get_type(void) {
    static GType exp_type = 0;

    if (!exp_type) {
	static const GTypeInfo dj_info = {
	    sizeof(ScreenSaverClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) screensaver_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(ScreenSaver),
	    0,
	    (GInstanceInitFunc) screensaver_init,
	};

	exp_type = g_type_register_static(G_TYPE_OBJECT, "ScreenSaver", &dj_info, 0);
    }

    return exp_type;
}

static void screensaver_class_init(ScreenSaverClass *klass) {
    GObjectClass *object_class = (GObjectClass*) klass;

    object_class->dispose = screensaver_dispose;
}

static void screensaver_init(ScreenSaver *self) {
}

static void screensaver_dispose(GObject *gobject) {
    ScreenSaver *self = SCREENSAVER(gobject);

    screensaver_stop(self);

    if (self->frame_renders) {
	int i;
	for (i=0; i<self->num_frames; i++)
	    g_object_unref(self->frame_renders[i]);
	g_free(self->frame_renders);
	self->frame_renders = NULL;
    }

    if (self->frame_parameters) {
	int i;
	for (i=0; i<self->num_frames; i++)
	    g_object_unref(self->frame_parameters[i].a);
	g_free(self->frame_parameters);
	self->frame_parameters = NULL;
    }

    if (self->view) {
	g_object_unref(self->view);
	self->view = NULL;
    }

    if (self->map) {
	g_object_unref(self->map);
	self->map = NULL;
    }
    if (self->animation) {
	g_object_unref(self->animation);
	self->animation = NULL;
    }
}

ScreenSaver* screensaver_new(IterativeMap *map, Animation *animation) {
    ScreenSaver *self = SCREENSAVER(g_object_new(screensaver_get_type(), NULL));
    int i;
    AnimationIter iter;
    gchar* common_parameters;

    self->animation = ANIMATION(g_object_ref(animation));
    self->map = ITERATIVE_MAP(g_object_ref(map));
    self->view = g_object_ref(histogram_view_new(HISTOGRAM_IMAGER(self->map)));

    /* Allocate and interpolate all frames */
    self->framerate = 10;
    self->num_frames = animation_get_length(self->animation) * self->framerate;
    self->frame_renders = g_new0(IterativeMap*, self->num_frames);
    self->frame_parameters = g_new0(ParameterHolderPair, self->num_frames);
    self->current_frame = 0;
    common_parameters = parameter_holder_save_string(PARAMETER_HOLDER(map));

    animation_iter_seek(animation, &iter, 0);
    for (i=0; i<self->num_frames; i++) {
	self->frame_renders[i] = ITERATIVE_MAP(de_jong_new());
	parameter_holder_load_string(PARAMETER_HOLDER(self->frame_renders[i]), common_parameters);

	self->frame_parameters[i].a = PARAMETER_HOLDER(de_jong_new());
	animation_iter_load(animation, &iter, self->frame_parameters[i].a);
	animation_iter_seek_relative(animation, &iter, 1/self->framerate);
    }
    for (i=0; i<self->num_frames-1; i++)
	self->frame_parameters[i].b = self->frame_parameters[i+1].a;
    self->frame_parameters[self->num_frames-1].b = self->frame_parameters[self->num_frames-1].a;

    g_free(common_parameters);
    self->direction = 1;

    screensaver_start(self);
    return self;
}


/************************************************************************************/
/************************************************************************ Rendering */
/************************************************************************************/

void          screensaver_start    (ScreenSaver *self)
{
    if (!self->idler)
	self->idler = g_idle_add(screensaver_idle_handler, self);
}

void          screensaver_stop     (ScreenSaver *self)
{
    if (self->idler) {
	g_source_remove(self->idler);
	self->idler = 0;
    }
}

static int screensaver_idle_handler(gpointer user_data) {
    ScreenSaver *self = SCREENSAVER(user_data);

    iterative_map_calculate_motion(ITERATIVE_MAP(self->frame_renders[self->current_frame]), 100000,
				   TRUE, PARAMETER_INTERPOLATOR(parameter_holder_interpolate_linear),
				   &self->frame_parameters[self->current_frame]);

    if (GTK_WIDGET_DRAWABLE(self->view)) {

	histogram_view_set_imager(HISTOGRAM_VIEW(self->view),
				  HISTOGRAM_IMAGER(self->frame_renders[self->current_frame]));
	histogram_view_update(HISTOGRAM_VIEW(self->view));

	self->current_frame += self->direction;
	if (self->current_frame >= self->num_frames) {
	    self->current_frame = self->num_frames-2;
	    self->direction = -1;
	}
	if (self->current_frame < 0) {
	    self->current_frame = 1;
	    self->direction = 1;
	}

    }
    return 1;
}

/* The End */
