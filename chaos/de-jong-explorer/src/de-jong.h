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

#ifndef __DE_JONG_H_
#define __DE_JONG_H__

struct vector2 {
  double x,y;
};

struct computation_params {
  double a, b, c, d;
  double zoom, xoffset, yoffset, rotation;
  double blur_radius, blur_ratio;
};

struct render_params {
  guint width, height;
  guint *counts;
  guint32 *pixels;

  guint color_table_size;
  guint32 *color_table;

  double iterations;
  guint current_density;
  guint target_density;

  double exposure, gamma;
  GdkColor fgcolor, bgcolor;

  gboolean dirty_flag;
};


extern struct computation_params params;
extern struct render_params render;


/* main.c */
void set_defaults();
gchar* save_parameters();
gboolean set_parameter(const char *key, const char *value);
void load_parameters(const gchar *paramstring);
void load_parameters_from_file(const char *name);
void save_to_file(const char *name);

/* render.c */
float uniform_variate();
float normal_variate();
void resize(int w, int h);
void update_pixels();
void clear();
void run_iterations(int count);

/* interface.c */
void interactive_main(int argc, char **argv);


#endif /* __DE_JONG_MAIN_H__ */

/* The End */
