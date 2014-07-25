/*
 * shadow.c - Definition for the group object
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

#include "shadow.h"

static void       shadow_class_init    (ShadowClass *klass);
static void       shadow_init          (Shadow *shadow);
static void       shadow_finalize      (GObject *object);
static GdkPixbuf* shadow_get_icon      (void);
static GList*     shadow_get_drawables (SceneObject *self);

static gpointer parent_class = NULL;

GType
shadow_get_type (void)
{
  static GType shadow_type = 0;
  if (!shadow_type)
  {
    static const GTypeInfo shadow_info =
    {
      sizeof (ShadowClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    shadow_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Shadow),
      0,                  /* n preallocs */
      (GInstanceInitFunc) shadow_init,
    };

    shadow_type = g_type_register_static (SCENE_OBJECT_TYPE, "Shadow", &shadow_info, 0);
  }

  return shadow_type;
}

static void
shadow_class_init (ShadowClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = shadow_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = shadow_get_drawables;

  object_class->finalize = shadow_finalize;
}

static void
shadow_init (Shadow *shadow)
{
}

static void
shadow_finalize (GObject *object)
{
}

static GdkPixbuf*
shadow_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
    icon = gdk_pixbuf_new_from_file ("data/shadow.png", NULL);

  return gdk_pixbuf_ref (icon);
}

static GList*
shadow_get_drawables (SceneObject *self)
{
  return NULL;
}
