/*
 * path.c - Implement the methods in path.h
 *
 * Copyright (C) 2005-2006 W. Evan Sheehan
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

#include <stdlib.h>
#include "path.h"

path*
path_new (gpointer data)
{
	path* ret = malloc (sizeof (path));
	ret->parent = NULL;
	ret->children = NULL;
	ret->data = data;

	return ret;
}

void
path_append_new (path* parent, gpointer data)
{
	path* child = path_new (data);
	parent->children = g_slist_prepend (parent->children, child);
	child->parent = parent;
}

void
path_destroy_all (path* root)
{
}
