/*
 * basicrenderpass.h - A simple render pass for the majority of objects. Sorts objects
 *                     by texture. This is the lowest priority render pass, and will
 *                     collect all drawables left over from other passes.
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

#include "basicrenderpass.h"

static void     texture_group_class_init     (TextureGroupClass *klass);
static void     texture_group_init           (TextureGroup *self);
static void     texture_group_draw           (Drawable *d);
static void     texture_group_draw_to_list   (DisplayList *dl);
static void     basic_render_pass_class_init (BasicRenderPassClass *klass);
static void     basic_render_pass_init       (BasicRenderPass *self);
static void     basic_render_pass_render     (RenderPass *pass);
static void     basic_render_pass_filter     (RenderPass *pass, Drawable *drawable);
static void     basic_render_pass_preprocess (RenderPass *pass);
static void     basic_render_pass_add        (RenderPass *pass, Drawable *drawable);
static void     basic_render_pass_erase      (RenderPass *pass);
static gboolean basic_render_pass_is_empty   (RenderPass *pass);

GType
texture_group_get_type (void)
{
  static GType tg_type = 0;
  if (!tg_type)
    {
      static const GTypeInfo tg_info =
      {
	sizeof (TextureGroupClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    texture_group_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (TextureGroup),
	0,                  /* n preallocs */
	(GInstanceInitFunc) texture_group_init,
      };

      tg_type = g_type_register_static (DISPLAY_LIST_TYPE, "TextureGroup", &tg_info, 0);
    }

  return tg_type;
}

static void
texture_group_class_init (TextureGroupClass *klass)
{
  DrawableClass *dc;
  DisplayListClass *dlc;

  dc = (DrawableClass*) klass;
  dlc = (DisplayListClass*) klass;

  dc->draw = texture_group_draw;

  dlc->draw_to_list = texture_group_draw_to_list;
}

static void
texture_group_init (TextureGroup *self)
{
  self->static_drawables = g_list_alloc ();
  self->dynamic_drawables = g_list_alloc ();
}

static void
texture_group_draw (Drawable *d)
{
  DisplayList *dl = DISPLAY_LIST (d);
  TextureGroup *tg = TEXTURE_GROUP (d);
  GList *dr;

  if (dl->dirty)
    texture_group_draw_to_list (dl);
  glCallList (dl->list);

  for (dr = tg->dynamic_drawables; dr; dr = dr->next)
  {
    drawable_draw (DRAWABLE (dr->data));
  }
}

static void
texture_group_draw_to_list (DisplayList *dl)
{
  GList *d;
  TextureGroup *tg = TEXTURE_GROUP (dl);

  for (d = tg->static_drawables; d; d = d->next)
  {
    display_list_build_list (DISPLAY_LIST (d->data));
  }
}

GType
basic_render_pass_get_type (void)
{
  static GType brp_type = 0;
  if (!brp_type)
    {
      static GTypeInfo brp_info =
      {
	sizeof (BasicRenderPassClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    basic_render_pass_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (BasicRenderPass),
	0,                  /* n preallocs */
	(GInstanceInitFunc) basic_render_pass_init,
      };

      brp_type = g_type_register_static (RENDER_PASS_TYPE, "BasicRenderPass", &brp_info, 0);
    }

  return brp_type;
}

static void
basic_render_pass_class_init (BasicRenderPassClass *klass)
{
  RenderPassClass *rpc;

  rpc = (RenderPassClass*) klass;

  rpc->render = basic_render_pass_render;
  rpc->filter = basic_render_pass_filter;
  rpc->preprocess = basic_render_pass_preprocess;
  rpc->add = basic_render_pass_add;
  rpc->erase = basic_render_pass_erase;
  rpc->is_empty = basic_render_pass_is_empty;
}

static void
basic_render_pass_init (BasicRenderPass *self)
{
}

static void
basic_render_pass_render (RenderPass *pass)
{
}

static void
basic_render_pass_filter (RenderPass *pass, Drawable *drawable)
{
}

static void
basic_render_pass_preprocess (RenderPass *pass)
{
}

static void
basic_render_pass_add (RenderPass *pass, Drawable *drawable)
{
}

static void
basic_render_pass_erase (RenderPass *pass)
{
}

static gboolean
basic_render_pass_is_empty (RenderPass *pass)
{
}
