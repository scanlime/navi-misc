/*
 * algorithms.c - C-based algorithms, for SUPER SPEED
 *
 * Copyright (C) 2005 David Trowbridge
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

static PyObject *depth_limited_search (PyObject* self, PyObject* args);

static PyMethodDef AlgorithmC_methods[] = {
		{"depthLimitedSearch", depth_limited_search, METH_VARARGS, "Execute a depth limited search of the graph"},
	        {NULL,                 NULL,                 0,            NULL},
};

PyMODINIT_FUNC
initalgorithms_c (void)
{
	(void) Py_InitModule ("algorithms_c", AlgorithmC_methods);
}

static float
computeProbability (GSList *path)
{
}

void
search (GSList* path, GHashTable* adjacency, PyObject* goal, int depth, GArray* good_paths)
{
	PyObject* node = (PyObject*) path->data;
	PyObject* args;
	PyObject* iter;
	PyObject* edge;

	if (depth < 0) {
		g_slist_free (path);
		return NULL;
	}

	GSList *v_list = g_hash_table_lookup (adjacency, node);

	for (GSList *vn = v_list; vn; vn = g_slist_next (vn)) {
		int       depth = g_slist_length (path);
		PyObject *v     = vn->data;
		Py_INCREF (v);

		GSList *tmp = g_slist_copy (path);
		tmp = g_slist_append (tmp, v);

		/* If the end of the path is our goal, it's a good path and deserves a
		 * cookie. Otherwise put the path back in the queue for later.
		 */
		if (goal == v && computeProbability (tmp) > computeProbability (g_array_index (good_paths, (GSList*), depth))) {
			good_paths = g_slist_prepend (good_paths, (gpointer)g_slist_reverse (tmp));
		} else {
			search (tmp, adjacency, goal, depth - 1, good_paths);
		}

		Py_DECREF (v);
	}
}

static GHashTable *
query_adjacency (PyObject *adjacency_list)
{
	PyObject   *data   = NULL;
	PyObject   *u      = NULL;
	PyObject   *v_list = NULL;
	int         pos;
	GHashTable *ret    = NULL;

	data = PyObject_GetAttrString (adjacency_list, "data");
	if (data == NULL)
		return NULL;

	ret = g_hash_table_new (g_direct_hash, g_direct_equal);

	pos = 0;
	while (PyDict_Next (data, &pos, &u, &v_list)) {
		GSList *list = NULL;
		int i = 0;
		PyObject *v;

		while (PyDict_Next (v_list, &i, &v, NULL))
			list = g_slist_prepend (list, v);
		if (PyErr_Occurred ())
			goto error;

		g_hash_table_insert (ret, u, list);
	}
	if (PyErr_Occurred ())
		goto error;

	Py_DECREF (data);

	return ret;

error:
	Py_XDECREF (data);
	if (ret)
		g_hash_table_destroy (ret);

	return NULL;
}

static void
free_values (gpointer key, gpointer value, gpointer data)
{
	g_slist_free ((GSList*) value);
}

static void
free_adjacency (GHashTable *hash_table) {
	g_hash_table_foreach (hash_table, free_values, NULL);
	g_hash_table_destroy (hash_table);
}

static PyObject*
depth_limited_search (PyObject* self, PyObject* args)
{
	PyObject   *adjacency_list;
	PyObject   *start;
	PyObject   *end;
	PyObject   *path_list;
	int         depth;
	GSList     *path  = NULL;
	GHashTable *adjacency = NULL;
	GArray     *paths = g_array_sized_new (false, true, sizeof (GSList*), depth);

	/* Get the graph and nodes or die trying. */
	if (!PyArg_ParseTuple (args, "OOOi;expected adjacency list, start, end, depth", &adjacency_list, &start, &end, &depth))
		return NULL;

	path = g_slist_prepend (path, (gpointer) start);

	adjacency = query_adjacency (adjacency_list);
	if (adjacency == NULL) {
		PyErr_SetString (PyExc_RuntimeError, "couldn't build hash table");
		return NULL;
	}

	/* Search */
	search (path, adjacency, end, depth - 1, paths);

	/* Create the list of paths and populate with None. */
	path_list = PyList_New (depth + 1);
	for (int i = 0; i <= depth; i++) {
		PyList_SetItem (path_list, i, Py_None);
	}

	/* For each path we found, insert it in the list at the appropriate
	 * depth.
	 */
	path = paths;
	while (path) {
		GSList*   nodes = path->data;
		int       len   = g_slist_length (nodes);
		PyObject* list  = PyList_New (0);
		PyObject* depth_list;

		/* FIXME: This could be more efficient by prepending to the list
		 *        instead of appending nodes?
		 */
		while (nodes) {
			PyList_Append (list, (PyObject*)nodes->data);
			nodes = g_slist_next (nodes);
		}

		depth_list = PyList_GetItem (path_list, len - 1);

		if (depth_list == Py_None || depth_list == NULL) {
			depth_list = PyList_New (0);
			PyList_SetItem (path_list, len - 1, depth_list);
		}

		PyList_Append (depth_list, list);

		/* Don't leak the paths. */
		g_slist_free (path->data);
		path = g_slist_next (path);
	}

	g_slist_free (paths);

	free_adjacency (adjacency);

	return path_list;
}
