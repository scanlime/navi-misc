/*
 * accumulator.h - The accumulator object handles pulling data from
 *                 computation threads and accumulating it into a
 *                 single frame buffer.
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

#ifndef __ACCUMULATOR_H__
#define __ACCUMULATOR_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define ACCUMULATOR_TYPE            (accumulator_get_type ())
#define ACCUMULATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ACCUMULATOR_TYPE, Accumulator))
#define ACCUMULATOR_CLASS(klass)    (G_TYPE_CLASS_CAST ((klass), ACCUMULATOR_TYPE, AccumulatorClass))
#define IS_ACCUMULATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ACCUMULATOR_TYPE))
#define IS_ACCUMULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ACCUMULATOR_TYPE))

typedef struct _Accumulator      Accumulator;
typedef struct _AccumulatorClass AccumulatorClass;

struct _Accumulator {
  GObject parent;
};

struct _AccumulatorClass {
  GObjectClass parent_class;
};

GType        accumulator_get_type ();
Accumulator* accumulator_new      ();

G_END_DECLS

#endif /* __ACCUMULATOR_H__ */

/* The End */
