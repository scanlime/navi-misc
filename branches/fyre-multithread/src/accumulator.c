/*
 * accumulator.c - The accumulator object handles pulling data from
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

#include "accumulator.h"

static void accumulator_class_init (AccumulatorClass *klass);
static void accumulator_init (Accumulator *self);

/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType accumulator_get_type (void)
{
  static GType acc_type = 0;
  if (!acc_type)
  {
    static const GTypeInfo acc_info = {
      sizeof (AccumulatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) accumulator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (Accumulator),
      0, /* n_preallocs */
      (GInstanceInitFunc) accumulator_init,
    };

    acc_type = g_type_register_static (HISTOGRAM_IMAGER_TYPE, "Accumulator", &acc_info, 0);
  }

  return acc_type;
}

static void
accumulator_class_init (AccumulatorClass *klass)
{
}

static void
accumulator_init (Accumulator *self)
{
}
