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
static gboolean   group_parent        (SceneObject *self, SceneObject *child);
static gboolean   group_deparent      (SceneObject *self, SceneObject *child);

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
  so_class->canparent = TRUE;
  so_class->get_drawables = group_get_drawables;
  so_class->parent = group_parent;
  so_class->deparent = group_deparent;

  object_class->finalize = group_finalize;
}

static void
group_init (Group *group)
{
  SceneObject *so = (SceneObject *) group;

  group->children = NULL;
  group->children_handlers = g_hash_table_new (g_direct_hash, g_direct_equal);
  group->max[0] = group->max[1] = group->max[2] = 0.0;
  group->min[0] = group->min[1] = group->min[2] = 0.0;

  so->bb.rotation = 0.0;
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

static void
recompute_bb (Group *g)
{
  GList *d;
  SceneObject *so = (SceneObject *) g;
  gboolean start = TRUE;

  for (d = g->children; d; d = d->next)
  {
    SceneObject *dso = SCENE_OBJECT (d->data);
    float s = sin (dso->bb.rotation);
    float c = cos (dso->bb.rotation);
    float px =  dso->bb.size[0];
    float nx = -dso->bb.size[0];
    float py =  dso->bb.size[1];
    float ny = -dso->bb.size[1];

    float x1 = (c * px) - (s * py) + dso->bb.position[0];
    float y1 = (s * px) + (c * py) + dso->bb.position[1];
    float x2 = (c * nx) - (s * py) + dso->bb.position[0];
    float y2 = (s * nx) + (c * py) + dso->bb.position[1];
    float x3 = (c * nx) - (s * ny) + dso->bb.position[0];
    float y3 = (s * nx) + (c * ny) + dso->bb.position[1];
    float x4 = (c * px) - (s * ny) + dso->bb.position[0];
    float y4 = (s * px) + (c * ny) + dso->bb.position[1];

    float z1 = dso->bb.position[2];
    float z2 = dso->bb.position[2] + dso->bb.size[2];

    if (start)
    {
      start = FALSE;
      g->min[0] = x1;
      g->min[1] = y1;
      g->min[2] = z1;
      g->max[0] = x1;
      g->max[1] = y1;
      g->max[2] = z1;
    }
    if (x1 < g->min[0]) g->min[0] = x1;
    if (x2 < g->min[0]) g->min[0] = x2;
    if (x3 < g->min[0]) g->min[0] = x3;
    if (x4 < g->min[0]) g->min[0] = x4;

    if (y1 < g->min[1]) g->min[1] = y1;
    if (y2 < g->min[1]) g->min[1] = y2;
    if (y3 < g->min[1]) g->min[1] = y3;
    if (y4 < g->min[1]) g->min[1] = y4;

    if (z1 < g->min[2]) g->min[2] = z1;
    if (z2 < g->min[2]) g->min[2] = z2;


    if (x1 > g->max[0]) g->max[0] = x1;
    if (x2 > g->max[0]) g->max[0] = x2;
    if (x3 > g->max[0]) g->max[0] = x3;
    if (x4 > g->max[0]) g->max[0] = x4;

    if (y1 > g->max[1]) g->max[1] = y1;
    if (y2 > g->max[1]) g->max[1] = y2;
    if (y3 > g->max[1]) g->max[1] = y3;
    if (y4 > g->max[1]) g->max[1] = y4;

    if (z1 > g->max[2]) g->max[2] = z1;
    if (z2 > g->max[2]) g->max[2] = z2;
  }

  {
    float w = (g->max[0] - g->min[0]) / 2.0f;
    float d = (g->max[1] - g->min[1]) / 2.0f;
    float h = (g->max[2] - g->min[2]) / 2.0f;

    so->bb.position[0] = g->min[0] + w;
    so->bb.position[1] = g->min[1] + d;
    so->bb.position[2] = g->min[2] + h;
    so->bb.size[0] = w;
    so->bb.size[1] = d;
    so->bb.size[2] = h;
  }

  g_signal_emit_by_name (G_OBJECT (g), "dirty");
}

static void
child_dirty (SceneObject *child, SceneObject *self)
{
  g_print ("child_dirty (%s)\n", g_type_name (G_TYPE_FROM_INSTANCE (child)));
  recompute_bb (GROUP (self));
}

static gboolean
group_parent (SceneObject *self, SceneObject *child)
{
  Group *g = GROUP (self);
  guint handler;

  child->parent = self;

  g_print ("group_parent (%s)\n", g_type_name (G_TYPE_FROM_INSTANCE (child)));

  g->children = g_list_append (g->children, child);
  handler = g_signal_connect (G_OBJECT (child), "dirty", G_CALLBACK (child_dirty), (gpointer) self);
  g_hash_table_insert (g->children_handlers, (gpointer) child, GUINT_TO_POINTER (handler));

  recompute_bb (g);

  return TRUE;
}

static gboolean
group_deparent (SceneObject *self, SceneObject *child)
{
  Group *g = GROUP (self);
  guint handler;

  g_list_remove (g->children, child);

  child->parent = NULL;

  g_print ("group_deparent (%s)\n", g_type_name (G_TYPE_FROM_INSTANCE (child)));

  handler = GPOINTER_TO_UINT (g_hash_table_lookup (g->children_handlers, (gpointer) child));
  g_signal_handler_disconnect (G_OBJECT (child), handler);
  g_hash_table_remove (g->children_handlers, (gpointer) child);

  recompute_bb (g);

  return TRUE;
}
