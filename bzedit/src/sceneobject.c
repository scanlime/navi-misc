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

static void scene_object_class_init         (SceneObjectClass *klass);
static void scene_object_init               (SceneObject *self);

static void selection_drawable_class_init   (SelectionDrawableClass *klass);
static void selection_drawable_init         (SelectionDrawable *sd);
static void selection_drawable_draw         (Drawable *d, RenderState *rstate);

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
  self->selection = selection_drawable_new (self);

  self->path = NULL;

  self->bb.position[0] = 0.0; self->bb.position[1] = 0.0; self->bb.position[2] = 0.0;
  self->bb.rotation = 0.0;
  self->bb.size[0] = 0.0; self->bb.size[1] = 0.0; self->bb.size[2] = 0.0;
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
  GList *drawables = klass->get_drawables (self);
  return g_list_append (drawables, self->selection);
}

GList*
scene_object_get_all_drawables (SceneObject *self)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (self));
  GList *drawables;

  if (klass->canparent)
    drawables = klass->get_all_drawables (self);
  else
    drawables = klass->get_drawables (self);
  return g_list_append (drawables, self->selection);
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

gboolean
scene_object_parent (SceneObject *parent, SceneObject *child)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (parent));

  if (child->parent)
    scene_object_deparent (child->parent, child);

  if (!klass->canparent)
    return FALSE;
  return klass->parent (parent, child);
}

gboolean
scene_object_deparent (SceneObject *parent, SceneObject *child)
{
  SceneObjectClass *klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (parent));

  if (!klass->canparent)
    return FALSE;
  return klass->deparent (parent, child);
}

GType
selection_drawable_get_type (void)
{
  static GType sd_type = 0;
  if (!sd_type)
  {
    static const GTypeInfo sd_info =
    {
      sizeof (SelectionDrawableClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    selection_drawable_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (SelectionDrawable),
      0,                  /* n preallocs */
      (GInstanceInitFunc) selection_drawable_init,
    };

    sd_type = g_type_register_static (DRAWABLE_TYPE, "SelectionDrawable", &sd_info, 0);
  }

  return sd_type;
}

static void
selection_drawable_class_init (SelectionDrawableClass *klass)
{
  DrawableClass *dc;

  dc = (DrawableClass*) klass;

  dc->draw = selection_drawable_draw;
}

static void
selection_drawable_init (SelectionDrawable *sd)
{
  Drawable *d = DRAWABLE (sd);
  d->texture = g_strdup ("");
  d->render.statico = FALSE;
}

Drawable*
selection_drawable_new (SceneObject *parent)
{
  Drawable *d = DRAWABLE (g_object_new (selection_drawable_get_type (), NULL));
  d->parent = parent;
  return d;
}

static void
selection_drawable_draw (Drawable *d, RenderState *rstate)
{
  SceneObject *so = d->parent;
  gdouble offset = 2.5;
  gdouble len = 2.5;
  gdouble x, y, z;

  if (!so->selected)
    return;

  glDisable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);

/*
  g_print ("selection_drawable_draw ()\n");
  g_print ("    bb->position = (%f, %f, %f)\n", so->bb.position[0], so->bb.position[1], so->bb.position[2]);
  g_print ("    bb->size     = (%f, %f, %f)\n", so->bb.size[0], so->bb.size[1], so->bb.size[2]);
  g_print ("    bb->rotation = %f\n", so->bb.rotation);
  */

  glPushMatrix ();
  glTranslated (so->bb.position[0], so->bb.position[1], so->bb.position[2]);
  glRotated (so->bb.rotation, 0.0, 0.0, 1.0);

  glColor4f (0.0, 1.0, 0.0, 1.0);

  glBegin (GL_LINES);
  {
    /* X+ Y+ Z+ corner */
    x = so->bb.size[0]; y = so->bb.size[1]; z = so->bb.size[2];
    glVertex3f (x + offset,       y + offset,       z + offset);
    glVertex3f (x + offset - len, y + offset,       z + offset);
    glVertex3f (x + offset,       y + offset,       z + offset);
    glVertex3f (x + offset,       y + offset - len, z + offset);
    glVertex3f (x + offset,       y + offset,       z + offset);
    glVertex3f (x + offset,       y + offset,       z + offset - len);

    /* X- Y+ Z+ corner */
    x = -so->bb.size[0]; y = so->bb.size[1]; z = so->bb.size[2];
    glVertex3f (x - offset,       y + offset,       z + offset);
    glVertex3f (x - offset + len, y + offset,       z + offset);
    glVertex3f (x - offset,       y + offset,       z + offset);
    glVertex3f (x - offset,       y + offset - len, z + offset);
    glVertex3f (x - offset,       y + offset,       z + offset);
    glVertex3f (x - offset,       y + offset,       z + offset - len);

    /* X- Y- Z+ corner */
    x = -so->bb.size[0]; y = -so->bb.size[1]; z = so->bb.size[2];
    glVertex3f (x - offset,       y - offset,       z + offset);
    glVertex3f (x - offset + len, y - offset,       z + offset);
    glVertex3f (x - offset,       y - offset,       z + offset);
    glVertex3f (x - offset,       y - offset + len, z + offset);
    glVertex3f (x - offset,       y - offset,       z + offset);
    glVertex3f (x - offset,       y - offset,       z + offset - len);

    /* X+ Y- Z+ corner */
    x = so->bb.size[0]; y = -so->bb.size[1]; z = so->bb.size[2];
    glVertex3f (x + offset,       y - offset,       z + offset);
    glVertex3f (x + offset - len, y - offset,       z + offset);
    glVertex3f (x + offset,       y - offset,       z + offset);
    glVertex3f (x + offset,       y - offset + len, z + offset);
    glVertex3f (x + offset,       y - offset,       z + offset);
    glVertex3f (x + offset,       y - offset,       z + offset - len);

    /* X+ Y+ Z- corner */
    x = so->bb.size[0]; y = so->bb.size[1]; z = -so->bb.size[2];
    glVertex3f (x + offset,       y + offset,       z - offset);
    glVertex3f (x + offset - len, y + offset,       z - offset);
    glVertex3f (x + offset,       y + offset,       z - offset);
    glVertex3f (x + offset,       y + offset - len, z - offset);
    glVertex3f (x + offset,       y + offset,       z - offset);
    glVertex3f (x + offset,       y + offset,       z - offset + len);

    /* X- Y+ Z- corner */
    x = -so->bb.size[0]; y = so->bb.size[1]; z = -so->bb.size[2];
    glVertex3f (x - offset,       y + offset,       z - offset);
    glVertex3f (x - offset + len, y + offset,       z - offset);
    glVertex3f (x - offset,       y + offset,       z - offset);
    glVertex3f (x - offset,       y + offset - len, z - offset);
    glVertex3f (x - offset,       y + offset,       z - offset);
    glVertex3f (x - offset,       y + offset,       z - offset + len);

    /* X- Y- Z- corner */
    x = -so->bb.size[0]; y = -so->bb.size[1]; z = -so->bb.size[2];
    glVertex3f (x - offset,       y - offset,       z - offset);
    glVertex3f (x - offset + len, y - offset,       z - offset);
    glVertex3f (x - offset,       y - offset,       z - offset);
    glVertex3f (x - offset,       y - offset + len, z - offset);
    glVertex3f (x - offset,       y - offset,       z - offset);
    glVertex3f (x - offset,       y - offset,       z - offset + len);

    /* X+ Y- Z- corner */
    x = so->bb.size[0]; y = -so->bb.size[1]; z = -so->bb.size[2];
    glVertex3f (x + offset,       y - offset,       z - offset);
    glVertex3f (x + offset - len, y - offset,       z - offset);
    glVertex3f (x + offset,       y - offset,       z - offset);
    glVertex3f (x + offset,       y - offset + len, z - offset);
    glVertex3f (x + offset,       y - offset,       z - offset);
    glVertex3f (x + offset,       y - offset,       z - offset + len);
  }
  glEnd ();

  glPopMatrix ();

  glColor4f (1.0, 1.0, 1.0, 1.0);

  glEnable (GL_TEXTURE_2D);
  glEnable (GL_LIGHTING);
}
