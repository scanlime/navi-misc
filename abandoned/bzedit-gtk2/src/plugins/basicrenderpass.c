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
#include "texturemanager.h"

static void     texture_group_class_init     (TextureGroupClass *klass);
static void     texture_group_init           (TextureGroup *self);
static void     texture_group_draw           (Drawable *d, RenderState *rstate);
static void     texture_group_dispose        (GObject *object);
static void     texture_group_picking_draw   (Drawable *d, RenderState *rstate);
static void     texture_group_draw_to_list   (DisplayList *dl);
static void     texture_group_add            (TextureGroup *tg, Drawable *drawable);
static void     basic_render_pass_class_init (BasicRenderPassClass *klass);
static void     basic_render_pass_init       (BasicRenderPass *self);
static void     basic_render_pass_render     (RenderPass *pass, RenderState *rstate);
static gboolean basic_render_pass_filter     (RenderPass *pass, Drawable *drawable);
static void     basic_render_pass_preprocess (RenderPass *pass);
static void     basic_render_pass_add        (RenderPass *pass, Drawable *drawable);
static void     basic_render_pass_erase      (RenderPass *pass);
static gboolean basic_render_pass_is_empty   (RenderPass *pass);
static gint     basic_render_pass_size       (RenderPass *pass);

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
  GObjectClass *oc;

  dc = (DrawableClass*) klass;
  dlc = (DisplayListClass*) klass;
  oc = (GObjectClass*) klass;

  dc->draw = texture_group_draw;

  dlc->draw_to_list = texture_group_draw_to_list;

  oc->dispose = texture_group_dispose;
}

static void
texture_group_init (TextureGroup *self)
{
  self->static_drawables = NULL;
  self->dynamic_drawables = NULL;
}

static void
texture_group_draw (Drawable *d, RenderState *rstate)
{
  DisplayList *dl = DISPLAY_LIST (d);
  TextureGroup *tg = TEXTURE_GROUP (d);
  GList *dr;

  display_list_draw (d, rstate);

  for (dr = tg->dynamic_drawables; dr; dr = dr->next)
  {
    drawable_draw (DRAWABLE (dr->data), rstate);
  }
}

static void
texture_group_draw_picking (Drawable *d, RenderState *rstate)
{
  TextureGroup *tg = TEXTURE_GROUP (d);
  GList *dr;

  for (dr = tg->static_drawables; dr; dr = dr->next)
  {
    render_state_picking_name (rstate, dr->data);
    drawable_draw (DRAWABLE (dr->data), rstate);
  }
  for (dr = tg->dynamic_drawables; dr; dr = dr->next)
  {
    render_state_picking_name (rstate, dr->data);
    drawable_draw (DRAWABLE (dr->data), rstate);
  }
}

static void
texture_group_draw_to_list (DisplayList *dl)
{
  GList *d;
  TextureGroup *tg = TEXTURE_GROUP (dl);
  RenderState *rstate = render_state_new ();

  for (d = tg->static_drawables; d; d = d->next)
  {
    drawable_draw (DRAWABLE (d->data), rstate);
  }

  dl->dirty = FALSE;
}

static void
texture_group_member_dirty (Drawable *drawable, DisplayList *dl)
{
  dl->dirty = TRUE;
}

static void
texture_group_dispose (GObject *object)
{
  TextureGroup *tg = (TextureGroup*) object;
  GList *l;

  for (l = tg->static_drawables; l; l = l->next)
  {
    g_signal_handlers_disconnect_by_func (G_OBJECT (l->data), G_CALLBACK (texture_group_member_dirty), (gpointer) tg);
    g_object_unref (G_OBJECT (l->data));
  }
  g_list_free (tg->static_drawables);
  tg->static_drawables = NULL;

  for (l = tg->dynamic_drawables; l; l = l->next)
  {
    g_object_unref (G_OBJECT (l->data));
  }
  g_list_free (tg->dynamic_drawables);
  tg->dynamic_drawables = NULL;
}

static void
texture_group_add (TextureGroup *tg, Drawable *drawable)
{
  DisplayList *dl = DISPLAY_LIST (tg);

  if (g_type_is_a (G_TYPE_FROM_INSTANCE (drawable), DISPLAY_LIST_TYPE))
  {
    display_list_build_list (DISPLAY_LIST (drawable));
  }

  if (drawable->render.statico)
  {
    tg->static_drawables = g_list_append (tg->static_drawables, (gpointer) g_object_ref (drawable));
    g_signal_connect (G_OBJECT (drawable), "dirty", G_CALLBACK (texture_group_member_dirty), (gpointer) tg);
  }
  else
  {
    tg->dynamic_drawables = g_list_append (tg->dynamic_drawables, (gpointer) g_object_ref (drawable));
  }

  dl->dirty = TRUE;
}

TextureGroup*
texture_group_new (void)
{
  return TEXTURE_GROUP (g_object_new (texture_group_get_type (), NULL));
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
  rpc->size = basic_render_pass_size;

  rpc->filter_priority = 0;
  rpc->render_priority = 100;
}

static void
basic_render_pass_init (BasicRenderPass *self)
{
  self->texture_groups = g_hash_table_new (g_str_hash, g_str_equal);
  self->size = 0;
}

static void
brp_render_iterate (gchar *texture, TextureGroup *group, RenderState *rstate)
{
  static TextureManager *tman = NULL;
  if (!tman)
    tman = texture_manager_new ();

  glEnable (GL_TEXTURE_2D);

  texture_manager_bind (tman, texture);
  texture_group_draw (DRAWABLE (group), rstate);
}

static void
brp_render_iterate_picking (gchar *texture, TextureGroup *group, RenderState *rstate)
{
  texture_group_draw_picking (DRAWABLE (group), rstate);
}

static void
basic_render_pass_render (RenderPass *pass, RenderState *rstate)
{
  BasicRenderPass *brp = BASIC_RENDER_PASS (pass);

  if (rstate->picking)
  {
    g_hash_table_foreach (brp->texture_groups, (GHFunc) brp_render_iterate_picking, (gpointer) rstate);
  }
  else
  {
    g_hash_table_foreach (brp->texture_groups, (GHFunc) brp_render_iterate, (gpointer) rstate);
  }
}

static gboolean
basic_render_pass_filter (RenderPass *pass, Drawable *drawable)
{
  return TRUE;
}

static void
brp_preprocess_iterate (gpointer key, gpointer value, gpointer data)
{
  DisplayList *dl = DISPLAY_LIST (value);
  dl->dirty = TRUE;
}

static void
basic_render_pass_preprocess (RenderPass *pass)
{
  BasicRenderPass *brp = BASIC_RENDER_PASS (pass);
  /* this builds display lists for all our texture groups */
  g_hash_table_foreach (brp->texture_groups, brp_preprocess_iterate, NULL);
}

static void
basic_render_pass_add (RenderPass *pass, Drawable *drawable)
{
  BasicRenderPass *brp = BASIC_RENDER_PASS (pass);
  TextureGroup *group;

  brp->size++;

  group = g_hash_table_lookup (brp->texture_groups, (gpointer) drawable->texture);
  if (group)
  {
    texture_group_add (group, drawable);
  } else {
    group = texture_group_new ();
    texture_group_add (group, drawable);
    g_hash_table_insert (brp->texture_groups, (gpointer) drawable->texture, (gpointer) group);
  }
}

static void
brp_tg_destroy (gchar *texture, TextureGroup *group, gpointer data)
{
  g_object_unref (group);
}

static void
basic_render_pass_erase (RenderPass *pass)
{
  BasicRenderPass *brp = BASIC_RENDER_PASS (pass);

  g_hash_table_foreach_remove (brp->texture_groups, (GHRFunc) brp_tg_destroy, NULL);
  g_free(brp->texture_groups);
  brp->size = 0;
  brp->texture_groups = g_hash_table_new (g_str_hash, g_str_equal);
}

static gboolean
basic_render_pass_is_empty (RenderPass *pass)
{
  BasicRenderPass *brp = BASIC_RENDER_PASS (pass);
  return (g_hash_table_size (brp->texture_groups) == 0);
}

static gint
basic_render_pass_size (RenderPass *pass)
{
  BasicRenderPass *brp = BASIC_RENDER_PASS (pass);
  return brp->size;
}
