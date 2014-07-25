/*
 * flagzone.c - Definition for the fez object
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

#include "flagzone.h"

static void       flag_zone_class_init    (FlagZoneClass *klass);
static void       flag_zone_init          (FlagZone *fez);
static void       flag_zone_set_property  (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       flag_zone_get_property  (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       flag_zone_finalize      (GObject *object);
static void       flag_zone_init_params   (GObjectClass *object_class);
static GdkPixbuf* flag_zone_get_icon      (void);

enum
{
  PROP_0,
};

static gpointer parent_class = NULL;

GType
flag_zone_get_type (void)
{
  static GType fez_type = 0;
  if (!fez_type)
  {
    static const GTypeInfo fez_info =
    {
      sizeof (FlagZoneClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    flag_zone_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (FlagZone),
      0,                  /* n preallocs */
      (GInstanceInitFunc) flag_zone_init,
    };

    fez_type = g_type_register_static (ZONE_TYPE, "FlagEntryZone", &fez_info, 0);
  }

  return fez_type;
}

static void
flag_zone_class_init (FlagZoneClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (ZONE_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = flag_zone_get_icon;
  so_class->creatable = TRUE;

  object_class->set_property = flag_zone_set_property;
  object_class->get_property = flag_zone_get_property;
  object_class->finalize = flag_zone_finalize;

  flag_zone_init_params (object_class);
}

static void
flag_zone_init (FlagZone *fez)
{
}

static void
flag_zone_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
}

static void
flag_zone_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
}

static void
flag_zone_finalize (GObject *object)
{
}

static void
flag_zone_init_params (GObjectClass *object_class)
{
}

static GdkPixbuf*
flag_zone_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
    icon = gdk_pixbuf_new_from_file ("data/fez.png", NULL);

  return gdk_pixbuf_ref (icon);
}
