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

static void
increase_depth (gpointer data, gpointer user_data)
{
}

static PyObject*
depth_limited_search (PyObject* self, PyObject* args)
{
	PyObject* graph;
	PyObject* start;
	PyObject* end;
	int       depth;
	GSList*   paths      = g_slist_alloc ();
	GSList*   path       = g_slist_alloc ();
	GSList*   good_paths = g_slist_alloc ();

	// Get the graph and nodes or die trying.
	if (!PyArg_ParseTuple (args, "OOOi", &graph, &start, &end, &depth)) {
		PyErr_SetObject (PyExc_TypeError, PyString_FromString ("expected a graph, and starting and ending nodes"));
		return NULL;
	}

	g_slist_append (path, (gpointer) start);
	g_slist_append (paths, (gpointer) path);

	for (int i = 0; i < depth; i++) {
		g_slist_foreach (paths, increase_depth, NULL);
	}
}
