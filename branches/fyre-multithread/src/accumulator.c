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
  /* nothing to do here */
}

static void
accumulator_init (Accumulator *self)
{
  /* nothing to do here */
}

Accumulator*
accumulator_new ()
{
  return ACCUMULATOR (g_object_new (accumulator_get_type (), NULL));
}

void
accumulator_clear (Accumulator *acc)
{
  histogram_imager_clear (HISTOGRAM_IMAGER (acc));

  acc->total_iterations = 0;
}

void
accumulator_merge (Accumulator *acc, IterativeMap *image)
{
  int ax, ay;
  int ix, iy;
  int i, j;
  HistogramImager *ha = HISTOGRAM_IMAGER (acc);
  HistogramImager *hi = HISTOGRAM_IMAGER (image);

  histogram_imager_get_hist_size (ha, &ax, &ay);
  histogram_imager_get_hist_size (hi, &ix, &iy);

  g_assert ((ax == ix) && (ay == iy));

  histogram_imager_prepare_plots (ha, &acc->plot);
  for (i = 0; i < ay; i++)
  {
    for (j = 0; j < ax; j++)
    {
      HISTOGRAM_IMAGER_PLOT_MULTIPLE(acc->plot, i, j, ((guint**)hi->histogram)[i][j]);
    }
  }
  histogram_imager_finish_plots (HISTOGRAM_IMAGER (acc), &acc->plot);
}
