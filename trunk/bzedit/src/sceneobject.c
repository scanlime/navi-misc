/*
 * sceneobject.c - Base class for objects which appear in the scene
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

#include "sceneobject.h"

static void scene_object_class_init (SceneObjectClass *klass);
static void scene_object_init       (SceneObject *self);

enum
{
  DIRTY,
  SELECT,
  LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GType
scene_object_get_type (void)
{
  static GType scene_object_type = 0;

  if (!scene_object_type)
    {
      static const GTypeInfo scene_object_info =
      {
        sizeof (SceneObjectClass),
        NULL,               /* base init */
        NULL,               /* base finalize */
        (GClassInitFunc) scene_object_class_init,
        NULL,               /* class finalize */
        NULL,               /* class data */
        sizeof (SceneObject),
        0,                  /* n preallocs */
        (GInstanceInitFunc) scene_object_init,
      };

      scene_object_type = g_type_register_static (PARAMETER_HOLDER_TYPE, "SceneObject", &scene_object_info, 0);
    }

  return scene_object_type;
}

static void
scene_object_class_init (SceneObjectClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  signals[DIRTY]  = g_signal_new ("dirty", G_OBJECT_CLASS_TYPE (klass),
                                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                                  G_STRUCT_OFFSET (SceneObjectClass, dirty),
                                  NULL, NULL,
                                  g_cclosure_marshal_VOID__VOID,
                                  G_TYPE_NONE, 0);

  signals[SELECT] = g_signal_new ("selected", G_OBJECT_CLASS_TYPE (klass),
                                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                                  G_STRUCT_OFFSET (SceneObjectClass, selected),
                                  NULL, NULL,
                                  g_cclosure_marshal_VOID__VOID,
                                  G_TYPE_NONE, 0);

  klass->creatable = TRUE;
  klass->autocreate = FALSE;
  klass->canparent = FALSE;
  klass->parentable = TRUE;

  klass->select = NULL;
  klass->deselect = NULL;
}

static void
scene_object_init (SceneObject *self)
{
  self->selected = FALSE;
  self->parent = NULL;
  self->name = NULL;
}

void
scene_object_render (SceneObject *self, guint name)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (self));
  klass->render (self, name);
}

void
scene_object_serialize (SceneObject *self, GIOChannel *out)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (self));
  klass->serialize (self, out);
}

GList*
scene_object_get_drawables (SceneObject *self)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (self));
  return klass->get_drawables (self);
}

void
scene_object_select (SceneObject *self)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (self));
  self->selected = TRUE;
  if(klass->select)
    klass->select (self);
  g_signal_emit_by_name (G_OBJECT(self), "selected");
}

void
scene_object_deselect (SceneObject *self)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (self));
  self->selected = FALSE;
  if (klass->deselect)
    klass->deselect (self);
}
