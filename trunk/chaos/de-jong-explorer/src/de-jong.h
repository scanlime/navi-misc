/*
 * de-jong.h - The DeJong object stores all the parameters necessary for
 *             rendering the de Jong map, runs iterations to generate a
 *             point histogram, and can update a GdkPixbuf with an image
 *             generated from the histogram.
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

#ifndef __DE_JONG_H__
#define __DE_JONG_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DE_JONG_TYPE            (de_jong_get_type ())
#define DE_JONG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DE_JONG_TYPE, DeJong))
#define DE_JONG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DE_JONG_TYPE, DeJongClass))
#define IS_DE_JONG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DE_JONG_TYPE))
#define IS_DE_JONG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DE_JONG_TYPE))

typedef struct _DeJong      DeJong;
typedef struct _DeJongClass DeJongClass;

struct _DeJong {
  GObject object;

  /* Current image size
   */
  guint width, height;
  guint oversample;
  gboolean size_dirty_flag;

  /* Calculation Parameters
   *
   * Parameters that affect the calculation process.
   * Changing these requires starting calculation over.
   */
  gdouble a, b, c, d;
  gdouble zoom, xoffset, yoffset, rotation;
  gdouble blur_radius, blur_ratio;
  gboolean tileable;
  gboolean calc_dirty_flag;

  /* Rendering Parameters
   *
   * Changing these parameters will not affect the
   * histogram, only the image generated from it.
   */
  gdouble exposure, gamma;
  GdkColor fgcolor, bgcolor;
  guint fgalpha, bgalpha;
  gboolean clamped;
  gboolean render_dirty_flag;

  /* Current rendering/calculation state
   */
  gdouble point_x, point_y;
  gdouble iterations;
  gulong current_density;
  gulong target_density;
  guint *histogram;

  GdkPixbuf *image;

  guint color_table_size;
  guint32 *color_table;
};

struct _DeJongClass {
  GObjectClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType      de_jong_get_type();
DeJong*    de_jong_new();

void       de_jong_reset_to_defaults(DeJong *self);
void       de_jong_set(DeJong *self, const gchar* property, const gchar* value);

void       de_jong_calculate(DeJong *self, guint iterations);
void       de_jong_update_image(DeJong *self);
GdkPixbuf* de_jong_make_thumbnail(DeJong *self, guint max_width, guint max_height);

void       de_jong_interpolate_linear(DeJong *self, DeJong *a, DeJong *b, double alpha);
void       de_jong_calculate_motion(DeJong *self, DeJong *a, DeJong *b,
				    guint iterations, gboolean continuation);

void       de_jong_load_string(DeJong *self, const gchar *params);
gchar*     de_jong_save_string(DeJong *self);

void       de_jong_load_image_file(DeJong *self, const gchar *filename);
void       de_jong_save_image_file(DeJong *self, const gchar *filename);


G_END_DECLS

#endif /* __DE_JONG_H__ */

/* The End */
