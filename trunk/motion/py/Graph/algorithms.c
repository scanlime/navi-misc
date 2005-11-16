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
static void      increase_depth       (gpointer data, gpointer user_data);

static PyMethodDef AlgorithmC_methods[] = {
		{"depthLimitedSearch", depth_limited_search, METH_VARARGS, "Execute a depth limited search of the graph"},
	        {NULL,                 NULL,                 0,            NULL},
};

PyMODINIT_FUNC
initalgorithms_c (void)
{
	(void) Py_InitModule ("algorithms_c", AlgorithmC_methods);
}

GSList*
search (GSList* path, PyObject* query_func, PyObject* goal, int depth)
{
	GSList*   good_paths = NULL;
	PyObject* node       = (PyObject*) path->data;
	PyObject* args;
	PyObject* iter;
	PyObject* edge;

	if (depth < 0) {
		g_slist_free (path);
		return NULL;
	}

	args = Py_BuildValue ("(O)", node);
	iter = PyEval_CallObject (query_func, args);

	Py_DECREF (args);

	while (edge = PyIter_Next (iter)) {
		PyObject* u = PyObject_GetAttrString (edge, "u");

		/* If the source of this edge is the current node... */
		if (PyObject_Compare (node, u) == 0) {
			/* Copy the path and append the node at the end of this edge. */
			PyObject* v = PyObject_GetAttrString (edge, "v");
			GSList* tmp = g_slist_copy (path);

			tmp = g_slist_prepend (tmp, (gpointer) v);

			/* If the end of the path is our goal, it's a good path and deserves a
			 * cookie. Otherwise put the path back in the queue for later.
			 */
			if (PyObject_Compare (goal, v) == 0) {
				good_paths = g_slist_prepend (good_paths, (gpointer)g_slist_reverse (tmp));
			} else {
				GSList* paths = search (tmp, query_func, goal, depth - 1);

				if (paths != NULL)
					good_paths = g_slist_concat (good_paths, paths);
			}

			Py_DECREF (v);
		}

		Py_DECREF (u);
		Py_DECREF (edge);
	}

	if (PyErr_Occurred ()) {
		Py_XDECREF (iter);
		return NULL;
	}

	Py_DECREF (iter);

	return good_paths;
}

static PyObject*
depth_limited_search (PyObject* self, PyObject* args)
{
	PyObject* adjacency_list;
	PyObject* start;
	PyObject* end;
	PyObject* query;
	PyObject* path_list;
	int       depth;
	GSList*   path  = NULL;
	GSList*   paths = NULL;

	/* Get the graph and nodes or die trying. */
	if (!PyArg_ParseTuple (args, "OOOi", &adjacency_list, &start, &end, &depth)) {
		PyErr_SetObject (PyExc_TypeError, PyString_FromString ("expected an adjacency list, starting and ending nodes, and a depth"));
		return NULL;
	}

	path = g_slist_prepend (path, (gpointer) start);

	/* Get the query function for the adjacency list. */
	query = PyObject_GetAttrString (adjacency_list, "query");

	/* Search */
	paths = search (path, query, end, depth - 1);

	/* Don't need our reference to query() anymore. */
	Py_DECREF (query);

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

		while (nodes) {
			PyList_Append (list, (PyObject*)nodes->data);
			nodes = g_slist_next (nodes);
		}

		depth_list = PyList_GetItem (path_list, len - 1);
		if (depth_list == Py_None || depth_list == NULL) {
			depth_list = PyList_New (0);
			PyList_SetItem (path_list, len-1, depth_list);
		}

		PyList_Append (depth_list, list);

		path = g_slist_next (path);
	}

	g_slist_free (paths);

	return path_list;
}
