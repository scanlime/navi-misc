/*
 * teleporter.h - Definition for the teleporter object
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

#ifndef __TELEPORTER_H__
#define __TELEPORTER_H__

#include "sceneobject.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define TELEPORTER_TYPE            (teleporter_get_type ())
#define TELEPORTER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TELEPORTER_TYPE, Teleporter))
#define TELEPORTER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TELEPORTER_TYPE, TeleporterClass))
#define IS_TELEPORTER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TELEPORTER_TYPE))
#define IS_TELEPORTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TELEPORTER_TYPE))

typedef struct _Teleporter      Teleporter;
typedef struct _TeleporterClass TeleporterClass;

typedef struct
{
  gdouble position[3];
  gdouble size[2];
  gdouble rotation;
  gdouble border;
  gboolean drive_through;
  gboolean shoot_through;
} TeleporterParams;

struct _Teleporter
{
  SceneObject parent;

  TeleporterParams param;
  gboolean state_dirty;
  GList *drawables;
  Drawable *field;
};

struct _TeleporterClass
{
  SceneObjectClass parent_class;
};

GType       teleporter_get_type (void) G_GNUC_CONST;
Teleporter* teleporter_new      (void);

#define TELEPORTER_FIELD_DRAWABLE_TYPE            (teleporter_field_drawable_get_type ())
#define TELEPORTER_FIELD_DRAWABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TELEPORTER_FIELD_DRAWABLE_TYPE, TeleporterFieldDrawable))
#define TELEPORTER_FIELD_DRAWABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TELEPORTER_FIELD_DRAWABLE_TYPE, TeleporterFieldDrawableClass))
#define IS_TELEPORTER_FIELD_DRAWABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TELEPORTER_FIELD_DRAWABLE_TYPE))
#define IS_TELEPORTER_FIELD_DRAWABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TELEPORTER_FIELD_DRAWABLE_TYPE))

typedef struct _TeleporterFieldDrawable      TeleporterFieldDrawable;
typedef struct _TeleporterFieldDrawableClass TeleporterFieldDrawableClass;

struct _TeleporterFieldDrawable
{
  DisplayList parent;
};

struct _TeleporterFieldDrawableClass
{
  DisplayListClass parent_class;
};

GType     teleporter_field_drawable_get_type (void) G_GNUC_CONST;
Drawable* teleporter_field_drawable_new      (SceneObject* parent);

G_END_DECLS

#endif
