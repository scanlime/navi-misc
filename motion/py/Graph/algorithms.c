/*
 * algorithms.c - C-based algorithms, for SUPER SPEED
 *
 * Copyright (C) 2005-2007 David Trowbridge
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
#include "dijkstra.h"
#include "utilities.h"

static PyObject* depth_limited_search (PyObject* self, PyObject* args);
static PyObject* dijkstra_search (PyObject* self, PyObject* args);


/* Defined in depth_limited.c */
void search (GSList* path, GHashTable* adjacency, GHashTable *edges, PyObject*
		goal, int depth, GSList* good_paths[]);

static PyMethodDef AlgorithmC_methods[] = {
	{"depthLimitedSearch", depth_limited_search, METH_VARARGS, "Execute a depth limited search of the graph"},
	{"dijkstraSearch",     dijkstra_search,      METH_VARARGS, "Execute dijkstra's single-source shortest-path search"},
        {NULL,                 NULL,                 0,            NULL},
};

PyMODINIT_FUNC
initalgorithms_c (void)
{
	(void) Py_InitModule ("algorithms_c", AlgorithmC_methods);
}

static PyObject*
depth_limited_search (PyObject* self, PyObject* args)
{
	PyObject   *adjacency_list;
	PyObject   *edge_list;
	PyObject   *start;
	PyObject   *end;
	PyObject   *path_list;
	int         depth;
	GSList     *path      = NULL;
	GHashTable *adjacency = NULL;
	GHashTable *edges     = NULL;

	/* Get the graph and nodes or die trying. */
	if (!PyArg_ParseTuple (args, "OOOOi;expected adjacency list, edge list, start, end, depth", &adjacency_list, &edge_list, &start, &end, &depth))
		return NULL;

	GSList *paths[depth];

	for (int i = 0; i < depth; i++) {
		paths[i] = NULL;
	}

	/* path gets freed when search() finishes. */
	path = g_slist_prepend (path, (gpointer) start);

	adjacency = query_adjacency (adjacency_list);
	if (adjacency == NULL) {
		PyErr_SetString (PyExc_RuntimeError, "couldn't build adjacency hash table");
		return NULL;
	}
	edges = query_edges (edge_list);
	if (edges == NULL) {
		PyErr_SetString (PyExc_RuntimeError, "couldn't build edge hash table");
		free_adjacency (adjacency);
		return NULL;
	}

	/* Search */
	search (path, adjacency, edges, end, depth, paths);

	/* Create the list of paths and populate with None. */
	path_list = PyList_New (depth + 1);
	for (int i = 0; i <= depth; i++) {
		Py_INCREF (Py_None);
		PyList_SetItem (path_list, i, Py_None);
	}

	/* For each path we found, insert it in the list at the appropriate
	 * depth.
	 */
	for (int i = 0; i < depth; i++) {
		GSList*   path  = paths[i];
		PyObject* list  = PyList_New (0);

		/* FIXME: This could be more efficient by prepending to the list
		 *        instead of appending nodes?
		 */
		if (path) {
			for (GSList* node = path; node; node = g_slist_next (node)) {
				PyList_Append (list, (PyObject*)node->data);
			}
			Py_DECREF (Py_None);
			PyList_SetItem (path_list, i, list);

			/* Don't leak the paths. */
			g_slist_free (path);
		}
	}

	free_adjacency (adjacency);
	free_edges (edges);

	return path_list;
}

static PyObject *
dijkstra_search (PyObject* self, PyObject* args)
{
	PyObject*   adjacency_list;
	PyObject*   start;
	PyObject*   end;
	PyObject*   ret;
	PyObject*   node;
	GHashTable* adjacency = NULL;
	GHashTable* previous = NULL;
	GHashTable* d = NULL;

	/* Get all the arguments */
	if (!PyArg_ParseTuple (args, "OOO;expected adjacency list, start, end", &adjacency_list, &start, &end)) {
		return NULL;
	}

	/* Hack (not sure why this is needed) */
	if (start == end) {
		ret = PyList_New (0);
		PyList_Append (ret, start);
		return ret;
	}

	/* Build adjacency hash table */
	adjacency = query_adjacency (adjacency_list);
	if (adjacency == NULL) {
		PyErr_SetString (PyExc_RuntimeError, "couldn't build adjacency hash table");
		return NULL;
	}

	/* Shortest path estimate */
	d        = g_hash_table_new (g_direct_hash, g_direct_equal);
	previous = g_hash_table_new (g_direct_hash, g_direct_equal);

	/* Initialize shortest path estimates */
	g_hash_table_foreach (adjacency, (GHFunc) initialize_dijkstra_d, d);
	g_hash_table_insert (d, start, GINT_TO_POINTER (0));

	/* Execute the actual search */
	find_shortest_paths (adjacency, d, previous);

	/* Build a list containing the path */
	ret = PyList_New (0);
	node = g_hash_table_lookup (previous, end);
	if (node == NULL) {
		PyErr_SetString (PyExc_RuntimeError, "No path found");
		g_hash_table_destroy (d);
		g_hash_table_destroy (previous);
		free_adjacency (adjacency);
		Py_DECREF (ret);
		return NULL;
	}
	PyList_Append (ret, end);
	PyList_Append (ret, node);
	while (node != start) {
		node = g_hash_table_lookup (previous, node);
		if (node == NULL) {
			PyErr_SetString (PyExc_RuntimeError, "No path found");
			g_hash_table_destroy (d);
			g_hash_table_destroy (previous);
			free_adjacency (adjacency);
			Py_DECREF (ret);
			return NULL;
		}
		PyList_Append (ret, node);
	}

	PyList_Reverse (ret);

	/* Clean up */
	g_hash_table_destroy (d);
	g_hash_table_destroy (previous);
	free_adjacency (adjacency);

	return ret;
}
