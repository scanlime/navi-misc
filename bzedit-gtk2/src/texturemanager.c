/*
 * texturemanager.c - Object to load in and manage textures
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

#include <string.h>
#include "texturemanager.h"

static void     texture_manager_class_init (TextureManagerClass *klass);
static void     texture_manager_init       (TextureManager *tm);
static gpointer texture_manager_load       (TextureManager *tm, gchar *name);

GType
texture_manager_get_type (void)
{
  static GType texture_manager_type = 0;

  if (!texture_manager_type)
    {
      static const GTypeInfo texture_manager_info =
      {
	sizeof (TextureManagerClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    texture_manager_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (TextureManager),
	0,
	(GInstanceInitFunc) texture_manager_init,
      };

      texture_manager_type = g_type_register_static (G_TYPE_OBJECT, "TextureManager", &texture_manager_info, 0);
    }

  return texture_manager_type;
}

static void
texture_manager_class_init (TextureManagerClass *klass)
{
}

static void
texture_manager_init (TextureManager *tm)
{
  tm->textures = g_hash_table_new (g_str_hash, g_str_equal);
}

TextureManager*
texture_manager_new (void)
{
  static TextureManager *tman = NULL;
  if (!tman)
    tman = TEXTURE_MANAGER (g_object_new (texture_manager_get_type (), NULL));
  return tman;
}

void
texture_manager_bind (TextureManager *tm, gchar *name)
{
  gpointer id;
  GLuint glid;

  /* non-textured object */
  if (!strlen(name))
    return;

  id = g_hash_table_lookup (tm->textures, (gpointer) name);
  if (!id)
    id = texture_manager_load (tm, name);
  glid = GPOINTER_TO_UINT (id);

  glBindTexture (GL_TEXTURE_2D, glid);
}

static gpointer
texture_manager_load (TextureManager *tm, gchar *name)
{
  gpointer id;
  GLuint glid;
  GdkPixbuf *image;
  guint width, height;
  const void *data;

  image = gdk_pixbuf_new_from_file (name, NULL);

  glGenTextures (1, &glid);
  glBindTexture (GL_TEXTURE_2D, glid);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  width = gdk_pixbuf_get_width (image);
  height = gdk_pixbuf_get_height (image);
  data = (const void*) gdk_pixbuf_get_pixels (image);

  gluBuild2DMipmaps (GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

  gdk_pixbuf_unref (image);

  id = GUINT_TO_POINTER (glid);
  g_hash_table_insert (tm->textures, (gpointer) name, id);

  return id;
}
