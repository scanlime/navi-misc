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

#ifndef __BASIC_RENDER_PASS_H__
#define __BASIC_RENDER_PASS_H__

#include "renderpass.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define TEXTURE_GROUP_TYPE            (texture_group_get_type ())
#define TEXTURE_GROUP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXTURE_GROUP_TYPE, TextureGroup))
#define TEXTURE_GROUP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TEXTURE_GROUP_TYPE, TextureGroupClass))
#define IS_TEXTURE_GROUP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEXTURE_GROUP_TYPE))
#define IS_TEXTURE_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TEXTURE_GROUP_TYPE))

typedef struct _TextureGroup      TextureGroup;
typedef struct _TextureGroupClass TextureGroupClass;

struct _TextureGroup
{
  DisplayList parent;

  GList *static_drawables;
  GList *dynamic_drawables;
};

struct _TextureGroupClass
{
  DisplayListClass parent_class;
};

GType         texture_group_get_type (void) G_GNUC_CONST;
TextureGroup* texture_group_new      (void);

#define BASIC_RENDER_PASS_TYPE            (basic_render_pass_get_type ())
#define BASIC_RENDER_PASS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASIC_RENDER_PASS_TYPE, BasicRenderPass))
#define BASIC_RENDER_PASS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASIC_RENDER_PASS_TYPE, BasicRenderPassClass))
#define IS_BASIC_RENDER_PASS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASIC_RENDER_PASS_TYPE))
#define IS_BASIC_RENDER_PASS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASIC_RENDER_PASS_TYPE))

typedef struct _BasicRenderPass      BasicRenderPass;
typedef struct _BasicRenderPassClass BasicRenderPassClass;

struct _BasicRenderPass
{
  RenderPass parent;

  GHashTable *texture_groups;
  gint size;
};

struct _BasicRenderPassClass
{
  RenderPassClass parent_class;
};

GType basic_render_pass_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif
