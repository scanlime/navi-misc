/*
 * animation.c - A simple keyframe animation system for DeJong objects
 *
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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

#include "animation.h"
#include "chunked-file.h"

static void animation_class_init(AnimationClass *klass);
static void animation_init(Animation *self);
static void animation_dispose(GObject *gobject);


/* Animations are serialized using chunked-file.
 * These are the chunk types and file signature
 */
#define FILE_SIGNATURE        "de Jong Explorer Animation\n\r\xFF\n"
#define CHUNK_KEYFRAME_START  CHUNK_TYPE('K','f','r','S')   /* Begin a new keyframe definition */
#define CHUNK_KEYFRAME_END    CHUNK_TYPE('K','f','r','E')   /* End a keyframe definition */
#define CHUNK_DE_JONG_PARAMS  CHUNK_TYPE('d','j','P','R')   /* Set de-jong parameters, represented as a string */
#define CHUNK_THUMBNAIL       CHUNK_TYPE('d','j','T','H')   /* Set a thumbnail, represented as a PNG image */


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType animation_get_type(void) {
  static GType anim_type = 0;

  if (!anim_type) {
    static const GTypeInfo dj_info = {
      sizeof(AnimationClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) animation_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(Animation),
      0,
      (GInstanceInitFunc) animation_init,
    };

    anim_type = g_type_register_static(G_TYPE_OBJECT, "Animation", &dj_info, 0);
  }

  return anim_type;
}

static void animation_class_init(AnimationClass *klass) {
  GObjectClass *object_class;
  object_class = (GObjectClass*) klass;

  object_class->dispose      = animation_dispose;
}

static void animation_init(Animation *self) {
  self->model = gtk_list_store_new(3,
				   GDK_TYPE_PIXBUF,   /* ANIMATION_MODEL_THUMBNAIL */
				   G_TYPE_STRING,     /* ANIMATION_MODEL_PARAMS    */
				   G_TYPE_FLOAT);     /* ANIMATION_MODEL_DURATION  */
}

static void animation_dispose(GObject *gobject) {
  Animation *self = ANIMATION(gobject);

  if (self->model) {
    g_object_unref(self->model);
    self->model = NULL;
  }
}

Animation* animation_new() {
  return ANIMATION(g_object_new(animation_get_type(), NULL));
}


/************************************************************************************/
/************************************************************ Keyframe Manipulation */
/************************************************************************************/

void animation_keyframe_store_dejong(Animation *self, GtkTreeIter *iter, DeJong *dejong) {
  /* Save de-jong parameters and a thumbnail to the keyframe at the given iterator
   */
  GdkPixbuf *thumbnail = de_jong_make_thumbnail(dejong, 128, 128);
  gchar *params = de_jong_save_string(dejong);

  gtk_list_store_set(self->model, iter,
		     ANIMATION_MODEL_THUMBNAIL, thumbnail,
		     ANIMATION_MODEL_PARAMS,    params,
		     -1);

  g_free(params);
  gdk_pixbuf_unref(thumbnail);
}

void animation_keyframe_load_dejong(Animation *self, GtkTreeIter *iter, DeJong *dejong) {
  /* Load de-jong parameters from the keyframe at the given iterator
   */
  gchar *params;

  gtk_tree_model_get(GTK_TREE_MODEL(self->model), iter,
		     ANIMATION_MODEL_PARAMS, &params,
		     -1);
  de_jong_load_string(dejong, params);
  g_free(params);
}

void animation_keyframe_append(Animation *self, DeJong *dejong) {
  GtkTreeIter iter;
  gtk_list_store_append(self->model, &iter);

  /* Save de jong parameters */
  animation_keyframe_store_dejong(self, &iter, dejong);

  /* Set a default transition */
  gtk_list_store_set(self->model, &iter,
		     ANIMATION_MODEL_DURATION, (gfloat) 1.0,
		     -1);
}


/************************************************************************************/
/************************************************************************* File I/O */
/************************************************************************************/

void animation_load_file(Animation *self, const gchar *filename) {
  FILE *f;
  ChunkType type;
  gsize length;
  guchar* data;

  g_return_if_fail(f = fopen(filename, "rb"));
  g_return_if_fail(chunked_file_read_signature(f, FILE_SIGNATURE));

  while (chunked_file_read_chunk(f, &type, &length, &data)) {
    switch (type) {


    default:
      chunked_file_warn_unknown_type(type);
    }
    g_free(data);
  }

  fclose(f);
}

void animation_save_file(Animation *self, const gchar *filename) {
  FILE *f;
  GtkTreeModel *model = GTK_TREE_MODEL(self->model);
  GtkTreeIter iter;
  gboolean valid;
  gchar *params;
  GdkPixbuf *thumb_pixbuf;
  gchar *thumb_buffer;
  gsize thumb_buffer_len;

  /* Start a new chunked file */
  g_return_if_fail(f = fopen(filename, "wb"));
  chunked_file_write_signature(f, FILE_SIGNATURE);

  /* Iterate over each keyframe in our model */
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {

    gtk_tree_model_get(model, &iter,
		       ANIMATION_MODEL_PARAMS,    &params,
		       ANIMATION_MODEL_THUMBNAIL, &thumb_pixbuf,
		       -1);

    chunked_file_write_chunk(f, CHUNK_KEYFRAME_START, 0, NULL);

    if (params) {
      chunked_file_write_chunk(f, CHUNK_DE_JONG_PARAMS, strlen(params), params);
      g_free(params);
    }

    if (thumb_pixbuf) {
      if (gdk_pixbuf_save_to_buffer(thumb_pixbuf, &thumb_buffer, &thumb_buffer_len, "png", NULL, NULL)) {
	chunked_file_write_chunk(f, CHUNK_THUMBNAIL, thumb_buffer_len, thumb_buffer);
	g_free(thumb_buffer);
	gdk_pixbuf_unref(thumb_pixbuf);
      }
    }

    chunked_file_write_chunk(f, CHUNK_KEYFRAME_END, 0, NULL);

    valid = gtk_tree_model_iter_next(model, &iter);
  }

  fclose(f);
}

/* The End */
