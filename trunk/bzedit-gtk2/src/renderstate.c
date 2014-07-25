/*
 * renderstate.c - Classes and structures to keep track of the rendering state
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

#include "renderstate.h"

static void render_state_class_init (RenderStateClass *klass);
static void render_state_init       (RenderState *self);

GType
render_state_get_type (void)
{
  static GType rs_type = 0;
  if (!rs_type)
    {
      static const GTypeInfo rs_info =
      {
	sizeof (RenderStateClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    render_state_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (RenderState),
	0,                  /* n preallocs */
	(GInstanceInitFunc) render_state_init,
      };

      rs_type = g_type_register_static (G_TYPE_OBJECT, "RenderState", &rs_info, 0);
    }

  return rs_type;
}

static void
render_state_class_init (RenderStateClass *klass)
{
}

static void
render_state_init (RenderState *self)
{
  self->cube_map = FALSE;
  self->picking = FALSE;
  self->picking_state = g_hash_table_new (g_direct_hash, g_direct_equal);
  self->picking_name = 1;
}

RenderState*
render_state_new (void)
{
  return RENDER_STATE (g_object_new (render_state_get_type (), NULL));
}

void
render_state_picking_name (RenderState *state, gpointer drawable)
{
  g_hash_table_insert(state->picking_state, GUINT_TO_POINTER (state->picking_name), drawable);
  glPushName (state->picking_name);
  state->picking_name++;
}

gpointer
render_state_do_pick (RenderState *state, guint name)
{
  gpointer result = g_hash_table_lookup (state->picking_state, GUINT_TO_POINTER (name));
  g_hash_table_destroy (state->picking_state);
  state->picking_state = g_hash_table_new (g_direct_hash, g_direct_equal);
  state->picking_name = 1;
  return result;
}
