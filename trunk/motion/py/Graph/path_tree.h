/*
 * path.h - Define a data structure for representing graph paths in a tree-like
 *          manner.
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

#include <glib.h>

typedef struct _path_tree path_tree;

struct _path_tree
{
	path_tree*    parent;
	GSList*  children;
	gpointer data;
};

path_tree* path_tree_new         (gpointer data);
void       path_tree_append_new  (path_tree* parent, gpointer data);
void       path_tree_destroy_all (path_tree* root);
