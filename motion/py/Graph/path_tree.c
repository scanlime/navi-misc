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
#include "path_tree.h"

path_tree*
path_tree_new (gpointer data)
{
	path_tree* ret = malloc (sizeof (path_tree));
	ret->parent = NULL;
	ret->children = NULL;
	ret->data = data;

	return ret;
}

void
path_append_new (path_tree* parent, gpointer data)
{
	path_tree* child = path_tree_new (data);
	parent->children = g_slist_prepend (parent->children, child);
	child->parent = parent;
}

void
path_tree_destroy_all (path_tree* root)
{
	for (GSList* node = root->children; node; node = g_slist_next (node)) {
		path_tree_destroy_all ((path_tree*) node->data);
	}

	if (root->children != NULL) {
		g_slist_free (root->children);
	}
	/* FIXME should free the data somehow */
	free (root);
}
