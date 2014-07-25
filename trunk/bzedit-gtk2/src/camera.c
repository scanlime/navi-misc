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

#include "camera.h"

static void camera_class_init (CameraClass *klass);
static void camera_init       (Camera *camera);
static void camera_base_jump  (Camera *camera);
static void camera_base_load  (Camera *camera);
static void apply_transform   (gdouble position[3], gdouble azimuth, gdouble elevation, gdouble distance);

GType
camera_get_type (void)
{
  static GType camera_type = 0;
  if (!camera_type)
  {
    static const GTypeInfo camera_info =
    {
      sizeof (CameraClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    camera_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Camera),
      0,                  /* n preallocs */
      (GInstanceInitFunc) camera_init,
    };

    camera_type = g_type_register_static (G_TYPE_OBJECT, "Camera", &camera_info, 0);
  }

  return camera_type;
}

static void
camera_class_init (CameraClass *klass)
{
  klass->jump = camera_base_jump;
  klass->load = camera_base_load;
}

static void
camera_init (Camera *camera)
{
  camera->position[0]   = 0.0;
  camera->position[1]   = 0.0;
  camera->position[2]   = 0.0;
  camera->distance      = 0.0;
  camera->azimuth       = 0.0;
  camera->elevation     = 0.0;
  camera->azimuthOffset = 0.0;
}

Camera*
camera_new (void)
{
  return CAMERA (g_object_new (camera_get_type (), NULL));
}

static void
camera_base_jump (Camera *camera)
{
}

static void
camera_base_load (Camera *camera)
{
  apply_transform (camera->position, camera->azimuth + camera->azimuthOffset, camera->elevation, camera->distance);
}

void
camera_jump (Camera *camera)
{
  CameraClass *klass = CAMERA_CLASS (G_OBJECT_GET_CLASS (camera));
  klass->jump (camera);
}

void
camera_load (Camera *camera)
{
  CameraClass *klass = CAMERA_CLASS (G_OBJECT_GET_CLASS (camera));
  klass->load (camera);
}

static void
apply_transform (gdouble position[3], gdouble azimuth, gdouble elevation, gdouble distance)
{
  glLoadIdentity ();
  glTranslatef (0, 0, -distance);
  glRotatef (elevation - 90.0, 1.0, 0.0, 0.0);
  glRotatef (-azimuth, 0.0, 0.0, 1.0);
  glTranslatef(0.0 - position[0], 0.0 - position[1], 0.0 - position[2]);
}
