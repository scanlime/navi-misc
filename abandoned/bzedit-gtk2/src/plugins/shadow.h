/*
 * shadow.h - Definition for the group object
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

#ifndef __SHADOW_H__
#define __SHADOW_H__

#include "sceneobject.h"

G_BEGIN_DECLS

#define SHADOW_TYPE            (shadow_get_type ())
#define SHADOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SHADOW_TYPE, Shadow))
#define SHADOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SHADOW_TYPE, ShadowClass))
#define IS_SHADOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SHADOW_TYPE))
#define IS_SHADOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHADOW_TYPE))

typedef struct _Shadow      Shadow;
typedef struct _ShadowClass ShadowClass;

struct _Shadow
{
  SceneObject parent;
};

struct _ShadowClass
{
  SceneObjectClass parent_class;
};

GType shadow_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif
