/*
 * light.c - Abstraction for an OpenGL light's position and intensity
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

#include "light.h"

static void light_class_init (LightClass *klass);
static void light_init       (Light *light);

GType
light_get_type (void)
{
  static GType light_type = 0;
  if (!light_type)
  {
    static const GTypeInfo light_info =
    {
      sizeof (LightClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    light_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Light),
      0,                  /* n preallocs */
      (GInstanceInitFunc) light_init,
    };

    light_type = g_type_register_static (G_TYPE_OBJECT, "Light", &light_info, 0);
  }

  return light_type;
}

static void
light_class_init (LightClass *klass)
{
}

static void
light_init (Light *light)
{
  light_reset (light);
}

Light*
light_new (guint lightnum)
{
  Light *light = LIGHT (g_object_new (light_get_type (), NULL));
  light->lightnum = lightnum;
  return light;
}

void
light_reset (Light* light)
{
  light->enabled = FALSE;

  light->ambient[0] = 0.0;
  light->ambient[1] = 0.0;
  light->ambient[2] = 0.0;
  light->ambient[3] = 1.0;

  light->diffuse[0] = 1.0;
  light->diffuse[1] = 1.0;
  light->diffuse[2] = 1.0;
  light->diffuse[3] = 1.0;

  light->position[0] = 0.0;
  light->position[1] = 0.0;
  light->position[2] = 0.0;
  light->position[3] = 1.0;
}

void
light_set (Light* light)
{
  if (light->enabled)
  {
    glEnable (light->lightnum);
    glLightfv (light->lightnum, GL_AMBIENT, light->ambient);
    glLightfv (light->lightnum, GL_DIFFUSE, light->diffuse);
    glLightfv (light->lightnum, GL_POSITION, light->position);
  }
  else
  {
    glDisable (light->lightnum);
  }
}
