/*
 * extended.c - BZEdit extensions
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
#include "group.h"
#include "shadow.h"
#include "plugins.h"

const gchar*
g_module_check_init (GModule *module)
{
  static GType group, shadow;

  /* permanently insert ourself */
  g_module_make_resident (module);

  /* and register our types */
  group = GROUP_TYPE;
  shadow = SHADOW_TYPE;

  return NULL;
}
