/*
 * texturemanager.h - Object to load in and manage textures
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

#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>

G_BEGIN_DECLS

#define TEXTURE_MANAGER_TYPE            (texture_manager_get_type ())
#define TEXTURE_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXTURE_MANAGER_TYPE, TextureManager))
#define TEXTURE_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TEXTURE_MANAGER_TYPE, TextureManagerClass))
#define IS_TEXTURE_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEXTURE_MANAGER_TYPE))
#define IS_TEXTURE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TEXTURE_MANAGER_TYPE))

typedef struct _TextureManager      TextureManager;
typedef struct _TextureManagerClass TextureManagerClass;

struct _TextureManager
{
  GObject object;

  GHashTable *textures;
};

struct _TextureManagerClass
{
  GObjectClass parent_class;
};

GType           texture_manager_get_type (void) G_GNUC_CONST;
TextureManager* texture_manager_new (void);
void            texture_manager_bind (TextureManager *tm, gchar *name);

G_END_DECLS

#endif
