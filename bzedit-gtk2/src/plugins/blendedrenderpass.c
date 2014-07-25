/*
 * blendedrenderpass.c - A rendering pass that collects blended objects and renders
 *                       them after most other objects.
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

#include "blendedrenderpass.h"

static void     blended_render_pass_class_init (BlendedRenderPassClass *klass);
static void     blended_render_pass_init       (BlendedRenderPass *self);
static void     blended_render_pass_render     (RenderPass *pass, RenderState *rstate);
static gboolean blended_render_pass_filter     (RenderPass *pass, Drawable *drawable);

static RenderPassClass* parent_class;

GType
blended_render_pass_get_type (void)
{
  static GType brp_type = 0;
  if (!brp_type)
  {
    static const GTypeInfo brp_info =
    {
      sizeof (BlendedRenderPassClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    blended_render_pass_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (BlendedRenderPass),
      0,                  /* n preallocs */
      (GInstanceInitFunc) blended_render_pass_init,
    };

    brp_type = g_type_register_static (BASIC_RENDER_PASS_TYPE, "BlendedRenderPass", &brp_info, 0);
  }

  return brp_type;
}

static void
blended_render_pass_class_init (BlendedRenderPassClass *klass)
{
  RenderPassClass *rpc;
  parent_class = g_type_class_ref (BASIC_RENDER_PASS_TYPE);

  rpc = (RenderPassClass*) klass;

  rpc->render = blended_render_pass_render;
  rpc->filter = blended_render_pass_filter;

  rpc->filter_priority = 10;
  rpc->render_priority = 80;
}

static void
blended_render_pass_init (BlendedRenderPass *self)
{
}

static void
blended_render_pass_render (RenderPass *pass, RenderState *rstate)
{
  glDepthMask (0);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  parent_class->render (pass, rstate);

  glDisable (GL_BLEND);
  glDepthMask (1);
}

static gboolean
blended_render_pass_filter (RenderPass *pass, Drawable *drawable)
{
  return drawable->render.blended;
}
