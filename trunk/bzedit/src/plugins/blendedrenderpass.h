/*
 * blendedrenderpass.h - A rendering pass that collects blended objects and renders
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

#ifndef __BLENDED_RENDER_PASS__
#define __BLENDED_RENDER_PASS__

#include "basicrenderpass.h"

#define BLENDED_RENDER_PASS_TYPE            (blended_render_pass_get_type ())
#define BLENDED_RENDER_PASS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLENDED_RENDER_PASS_TYPE, BlendedRenderPass))
#define BLENDED_RENDER_PASS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BLENDED_RENDER_PASS_TYPE, BlendedRenderPassClass))
#define IS_BLENDED_RENDER_PASS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BLENDED_RENDER_PASS_TYPE))
#define IS_BLENDED_RENDER_PASS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BLENDED_RENDER_PASS_TYPE))

typedef struct _BlendedRenderPass      BlendedRenderPass;
typedef struct _BlendedRenderPassClass BlendedRenderPassClass;

struct _BlendedRenderPass
{
  BasicRenderPass parent;
};

struct _BlendedRenderPassClass
{
  BasicRenderPassClass parent_class;
};

GType blended_render_pass_get_type (void) G_GNUC_CONST;

#endif
