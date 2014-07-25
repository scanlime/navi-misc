/*
 * link.c - Definition for the link object
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

#include "link.h"

static void       link_class_init    (LinkClass *klass);
static void       link_init          (Link *link);
static void       link_set_property  (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       link_get_property  (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       link_finalize      (GObject *object);
static void       link_init_params   (GObjectClass *object_class);
static GdkPixbuf* link_get_icon      (void);
static GList*     link_get_drawables (SceneObject *self);

enum
{
  PROP_0,
  PROP_SOURCE,
  PROP_DEST,
};

static gpointer parent_class = NULL;

GType
link_get_type (void)
{
  static GType link_type = 0;
  if (!link_type)
  {
    static const GTypeInfo link_info =
    {
      sizeof (LinkClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    link_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Link),
      0,                  /* n preallocs */
      (GInstanceInitFunc) link_init,
    };

    link_type = g_type_register_static (SCENE_OBJECT_TYPE, "Link", &link_info, 0);
  }

  return link_type;
}

static void
link_class_init (LinkClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = link_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = link_get_drawables;

  object_class->set_property = link_set_property;
  object_class->get_property = link_get_property;
  object_class->finalize = link_finalize;

  link_init_params (object_class);
}

static void
link_init (Link *link)
{
  link->drawables = NULL;
}

static void
link_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
}

static void
link_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
}

static void
link_finalize (GObject *object)
{
}

static void
link_init_params (GObjectClass *object_class)
{
}

static GdkPixbuf*
link_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
    icon = gdk_pixbuf_new_from_file ("data/link.png", NULL);

  return gdk_pixbuf_ref (icon);
}

static GList*
link_get_drawables (SceneObject *self)
{
  Link *l = LINK (self);
  return l->drawables;
}
