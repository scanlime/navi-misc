/*
 * renderpass.c - Abstract implementation of a rendering pass, as well as several
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

#include "renderpass.h"

static void render_pass_class_init (RenderPassClass *klass);
static void render_pass_init       (RenderPass *pass);

GType
render_pass_get_type (void)
{
  static GType rp_type = 0;
  if (!rp_type)
    {
      static const GTypeInfo rp_info =
      {
	sizeof (RenderPassClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    render_pass_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (RenderPass),
	0,                  /* n preallocs */
	(GInstanceInitFunc) render_pass_init,
      };

      rp_type = g_type_register_static (G_TYPE_OBJECT, "RenderPass", &rp_info, 0);
    }

  return rp_type;
}

static void
render_pass_class_init (RenderPassClass *klass)
{
  klass->preprocess = NULL;
}

static void
render_pass_init (RenderPass *pass)
{
  /* subclasses should take care of this */
}

void
render_pass_render (RenderPass *pass, RenderState *rstate)
{
  RenderPassClass *klass = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (pass));
  klass->render (pass, rstate);
}

gboolean
render_pass_filter (RenderPass *pass, Drawable *drawable)
{
  RenderPassClass *klass = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (pass));
  return klass->filter (pass, drawable);
}

void
render_pass_preprocess (RenderPass *pass)
{
  RenderPassClass *klass = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (pass));
  if (klass->preprocess)
    klass->preprocess (pass);
}

void
render_pass_add (RenderPass *pass, Drawable *drawable)
{
  RenderPassClass *klass = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (pass));
  klass->add (pass, drawable);
}

void
render_pass_erase (RenderPass *pass)
{
  RenderPassClass *klass = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (pass));
  klass->erase (pass);
}

gboolean
render_pass_is_empty (RenderPass *pass)
{
  RenderPassClass *klass = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (pass));
  return klass->is_empty (pass);
}

gint
render_pass_get_size (RenderPass *pass)
{
  RenderPassClass *klass = RENDER_PASS_CLASS (G_OBJECT_GET_CLASS (pass));
  return klass->size (pass);
}
