/*
 * camera.h - Abstraction for the OpenGL transforms used to set up the
 *            camera.  For 1st person operation, use 'position', 'azimuth'
 *            and 'elevation', with 'distance' set to zero. For 3rd person
 *            to a nonzero value.
 *
 * BZEdit
 * Copyright (C) 2004 David Trowbridge
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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <gtk/gtk.h>
#include <GL/gl.h>

G_BEGIN_DECLS

#define CAMERA_TYPE            (camera_get_type ())
#define CAMERA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CAMERA_TYPE, Camera))
#define CAMERA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CAMERA_TYPE, CameraClass))
#define IS_CAMERA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CAMERA_TYPE))
#define IS_CAMERA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CAMERA_TYPE))

typedef struct _Camera      Camera;
typedef struct _CameraClass CameraClass;

struct _Camera
{
  GObject parent;

  gdouble position[3];
  gdouble azimuth;
  gdouble elevation;
  gdouble azimuthOffset;
  gdouble distance;
};

struct _CameraClass
{
  GObjectClass parent_class;

  void (*jump) (Camera *self);
  void (*load) (Camera *self);
};

GType   camera_get_type (void) G_GNUC_CONST;
Camera* camera_new      (void);
void    camera_jump     (Camera *camera);
void    camera_load     (Camera *camera);

G_END_DECLS

#endif
