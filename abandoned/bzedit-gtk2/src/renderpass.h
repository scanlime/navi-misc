/*
 * renderpass.h - Abstract implementation of a rendering pass, as well as several
 *                concrete rendering passes. Provides an interface for instantiating
 *                the available rendering passes with the proper priorities.
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

#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include <gtk/gtk.h>
#include <GL/gl.h>
#include "drawable.h"
#include "renderstate.h"

G_BEGIN_DECLS

#define RENDER_PASS_TYPE            (render_pass_get_type ())
#define RENDER_PASS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RENDER_PASS_TYPE, RenderPass))
#define RENDER_PASS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RENDER_PASS_TYPE, RenderPassClass))
#define IS_RENDER_PASS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RENDER_PASS_TYPE))
#define IS_RENDER_PASS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RENDER_PASS_TYPE))

typedef struct _RenderPass      RenderPass;
typedef struct _RenderPassClass RenderPassClass;

struct _RenderPass
{
  GObject object;
};

struct _RenderPassClass
{
  GObjectClass parent_class;

  void     (*render)     (RenderPass *pass, RenderState *rstate);
  gboolean (*filter)     (RenderPass *pass, Drawable *drawable);
  void     (*preprocess) (RenderPass *pass);
  void     (*add)        (RenderPass *pass, Drawable *drawable);
  void     (*erase)      (RenderPass *pass);
  gboolean (*is_empty)   (RenderPass *pass);
  gint     (*size)       (RenderPass *pass);

  guint filter_priority;
  guint render_priority;
};

GType    render_pass_get_type   (void) G_GNUC_CONST;
void     render_pass_render     (RenderPass *pass, RenderState *rstate);
gboolean render_pass_filter     (RenderPass *pass, Drawable *drawable);
void     render_pass_preprocess (RenderPass *pass);
void     render_pass_add        (RenderPass *pass, Drawable *drawable);
void     render_pass_erase      (RenderPass *pass);
gboolean render_pass_is_empty   (RenderPass *pass);
gint     render_pass_get_size   (RenderPass *pass);

G_END_DECLS

#endif
