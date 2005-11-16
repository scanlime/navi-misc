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

void
remove_paths (gpointer data, gpointer user_data)
{
	g_slist_free ((GSList*) data);
}

static PyObject*
depth_limited_search (PyObject* self, PyObject* args)
{
	PyObject* adjacency_list;
	PyObject* start;
	PyObject* end;
	PyObject* query;
	PyObject* ret_list;
	int       len;
	int       depth;
	GSList*   paths = NULL;
	GSList*   path = NULL;
	GSList*   good_paths = NULL;
	GSList*   next_paths = NULL;
	GSList*   ret_val = NULL;

	// Get the graph and nodes or die trying.
	if (!PyArg_ParseTuple (args, "OOOi", &adjacency_list, &start, &end, &depth)) {
		PyErr_SetObject (PyExc_TypeError, PyString_FromString ("expected an adjacency list, starting and ending nodes, and a depth"));
		return NULL;
	}

	path  = g_slist_prepend (path, (gpointer) start);
	paths = g_slist_prepend (paths, (gpointer) path);

	query = PyObject_GetAttrString (adjacency_list, "query");

	for (int i = 0; i < depth; i++) {
		path = paths;

		while (path) {
			GSList*   node = g_slist_last (path->data);
			PyObject* args = Py_BuildValue ("(O)", (PyObject*) node->data);
			PyObject* iter = PyEval_CallObject (query, args);
			PyObject* edge;

			Py_DECREF (args);

			while (edge = PyIter_Next (iter)) {
				PyObject* u = PyObject_GetAttrString (edge, "u");

				if (PyObject_Compare ((PyObject*)node->data, u) == 0) {
					PyObject* v = PyObject_GetAttrString (edge, "v");
					GSList* tmp = g_slist_copy (path);

					tmp = g_slist_prepend (tmp, (gpointer) v);

					if (PyObject_Compare (end, v) == 0) {
						good_paths = g_slist_prepend (good_paths, (gpointer) tmp);
					} else {
						next_paths = g_slist_prepend (next_paths, (gpointer) tmp);
					}

					Py_DECREF (v);
				}

				Py_DECREF (u);
				Py_DECREF (edge);
			}

			Py_DECREF (iter);
			path = g_slist_next (path);
		}

		g_slist_foreach (paths, remove_paths, NULL);
		g_slist_free (paths);
		paths = next_paths;
		next_paths = NULL;

		ret_val = g_slist_prepend (ret_val, good_paths);

		good_paths = NULL;
	}

	Py_DECREF (query);

	ret_val = g_slist_reverse (ret_val);

	len = g_slist_length (ret_val);
	ret_list = PyList_New (len);

	for (int i = 0; i < len; i++) {
		PyObject* path_pyList;
		int       size;

		path        = g_slist_nth (ret_val, i);
		size        = g_slist_length (path);
		path_pyList = PyList_New (size);

		for (int j = 0; j < size; j++) {
			PyObject* node = (PyObject*) g_slist_nth (path, j);
			PyList_SetItem (path_pyList, j, node);
		}

		PyList_SetItem (ret_list, i, path_pyList);
	}

	return ret_list;
}
