/*
 * group.c - Definition for the group object
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

#include "group.h"

static void       group_class_init    (GroupClass *klass);
static void       group_init          (Group *group);
static void       group_finalize      (GObject *object);
static GdkPixbuf* group_get_icon      (void);
static GList*     group_get_drawables (SceneObject *self);

static gpointer parent_class = NULL;

GType
group_get_type (void)
{
  static GType group_type = 0;
  if (!group_type)
  {
    static const GTypeInfo group_info =
    {
      sizeof (GroupClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    group_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Group),
      0,                  /* n preallocs */
      (GInstanceInitFunc) group_init,
    };

    group_type = g_type_register_static (SCENE_OBJECT_TYPE, "Group", &group_info, 0);
  }

  return group_type;
}

static void
group_class_init (GroupClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = group_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = group_get_drawables;

  object_class->finalize = group_finalize;
}

static void
group_init (Group *group)
{
}

Group*
group_new (void)
{
  return GROUP (g_object_new (group_get_type (), NULL));
}

static void
group_finalize (GObject *object)
{
}

static GdkPixbuf*
group_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
  {
    icon = gdk_pixbuf_new_from_file ("data/group.png", NULL);
  }

  return gdk_pixbuf_ref (icon);
}

static GList*
group_get_drawables (SceneObject *self)
{
  return NULL;
}
