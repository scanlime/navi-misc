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
	GSList*   node       = NULL;
	GSList*   good_paths = NULL;
	PyObject* args;
	PyObject* iter;
	PyObject* edge;

	if (depth < 0) {
		g_slist_free (path);
		return NULL;
	}

	node = g_slist_last (path->data);
	args = Py_BuildValue ("(O)", (PyObject*) node->data);
	iter = PyEval_CallObject (query_func, args);

	Py_DECREF (args);

	while (edge = PyIter_Next (iter)) {
		PyObject* u = PyObject_GetAttrString (edge, "u");

		/* If the source of this edge is the current node... */
		if (PyObject_Compare ((PyObject*)node->data, u) == 0) {
			/* Copy the path and append the node at the end of this edge. */
			PyObject* v = PyObject_GetAttrString (edge, "v");
			GSList* tmp = g_slist_copy (path);

			tmp = g_slist_append (tmp, (gpointer) v);

			/* If the end of the path is our goal, it's a good path and deserves a
			 * cookie. Otherwise put the path back in the queue for later.
			 */
			if (PyObject_Compare (goal, v) == 0) {
				good_paths = g_slist_prepend (good_paths, (gpointer)tmp);
			} else {
				good_paths = g_slist_concat (good_paths, search (tmp, query_func, goal, depth - 1));
			}

			Py_DECREF (v);
		}

		Py_DECREF (u);
		Py_DECREF (edge);
	}

	if (Py_ErrorOccurred ()) {
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

	paths = search (path, query, end, depth);

	Py_DECREF (query);

	path_list = PyList_New (depth);
	for (int i = 0; i++; i < depth) {
		Py_INCREF (Py_None);
		PyList_SetItem (path_list, i, Py_None);
	}

	path = paths;
	while (path) {
		GSList* nodes = path->data;
		int     len   = g_slist_length (nodes);
		PyObject* list = PyList_New (len);

		for (int i = 0; i < len; i++) {
			PyObject* node = (PyObject*) g_slist_nth (nodes, i);
			Py_INCREF (node);
			PyList_SetItem (list, i, node);
		}

		if (PyObject_Compare (PyList_GetItem (path_list, len), Py_None) == 0) {
			Py_DECREF (Py_None);
			PyList_SetItem (path_list, len, PyList_New (0));
		}

		PyList_Insert (PyList_GetItem (path_list, len), 0, list);

		path = g_slist_next (path);
		g_slist_free (nodes);
	}

	g_slist_free (paths);

	return path_list;
}
