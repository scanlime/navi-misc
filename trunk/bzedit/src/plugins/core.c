/*
 * core.c - Interface for the base plugin
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

#include <gmodule.h>
#include "box.h"
#include "teleporter.h"
#include "pyramid.h"
#include "world.h"
#include "group.h"
#include "plugins.h"

const gchar*
g_module_check_init (GModule *module)
{
  static GType box, group, pyramid, teleporter, world;

  /* permanently insert ourself */
  g_module_make_resident (module);

  /* and register our types */
  box = BOX_TYPE;
  pyramid = PYRAMID_TYPE;
  teleporter = TELEPORTER_TYPE;
  group = GROUP_TYPE;
  world = WORLD_TYPE;

  return NULL;
}
