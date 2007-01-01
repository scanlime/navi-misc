/*
 * depth_limited.c - Depth limited search
 *
 * Copyright (C) 2005-2007 W. Evan Sheehan
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
#include <Python.h>
#include "utilities.h"

void
search (GSList* path, GHashTable* adjacency, GHashTable *edges, PyObject* goal,
		int depth, GSList* good_paths[])
{
	PyObject* node = (PyObject*) path->data;
	PyObject* args;
	PyObject* iter;
	PyObject* edge;

	if (depth == 0) {
		g_slist_free (path);
		return;
	}

	GSList *v_list = g_hash_table_lookup (adjacency, node);

	for (GSList *vn = v_list; vn; vn = g_slist_next (vn)) {
		int       d = g_slist_length (path) - 1;
		PyObject *v = vn->data;
		Py_INCREF (v);

		GSList *new = g_slist_copy (path);
		new = g_slist_prepend (new, v);

		/* If the end of the path is our goal, it's a good path and deserves a
		 * cookie. Otherwise, recurse down the path.
		 */
		if (goal == v) {
			GSList* old = good_paths[d];
			if (computeProbability (edges, new) > computeProbability (edges, old)) {
				if (old)
					g_slist_free (old);
				good_paths[d] = new;
			} else {
				g_slist_free (new);
			}
		} else {
			search (new, adjacency, edges, goal, depth - 1, good_paths);
		}

		Py_DECREF (v);
	}

	g_slist_free (path);
}
