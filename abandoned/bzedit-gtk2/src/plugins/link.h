/*
 * link.h - Definition for the link object
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

#ifndef __LINK_H__
#define __LINK_H__

#include "sceneobject.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define LINK_TYPE            (link_get_type ())
#define LINK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LINK_TYPE, Link))
#define LINK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LINK_TYPE, LinkClass))
#define IS_LINK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LINK_TYPE))
#define IS_LINK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LINK_TYPE))

typedef struct _Link      Link;
typedef struct _LinkClass LinkClass;

struct _Link
{
  SceneObject parent;
  GList *drawables;
};

struct _LinkClass
{
  SceneObjectClass parent_class;
};

GType link_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif
