/*
 * algorithms.c - C-based algorithms, for SUPER SPEED
 *
 * Copyright (C) 2005-2006 David Trowbridge
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
static PyObject *aStar_search (PyObject* self, PyObject* args);

static PyMethodDef AlgorithmC_methods[] = {
		{"depthLimitedSearch", depth_limited_search, METH_VARARGS, "Execute a depth limited search of the graph"},
		{"aStarSearch", aStar_search, METH_VARARGS, "Execute the A* search of the graph"},
	        {NULL,                 NULL,                 0,            NULL},
};

PyMODINIT_FUNC
initalgorithms_c (void)
{
	(void) Py_InitModule ("algorithms_c", AlgorithmC_methods);
}

static float
computeProbability (GHashTable *edges, GSList *path)
{
	float probability = 1.0f;

	if (path == NULL)
		return 0.0f;

	for (GSList *i = path; i && i->next; i = g_slist_next (i)) {
		PyObject *u = i->data;
		PyObject *v = g_slist_next (i)->data;

		float *weight = g_hash_table_lookup (edges, GINT_TO_POINTER (GPOINTER_TO_INT (u) + GPOINTER_TO_INT (v)));
		probability *= *weight;
	}

	return probability;
}

void
search (GSList* path, GHashTable* adjacency, GHashTable *edges, PyObject* goal, int depth, GSList* good_paths[])
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

static void
free_floats (gpointer key, gpointer value, gpointer data)
{
	g_free (value);
}

static void
free_edges (GHashTable *hash_table) {
	g_hash_table_foreach (hash_table, free_floats, NULL);
	g_hash_table_destroy (hash_table);
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
		free_adjacency (ret);

	return NULL;
}

static GHashTable *
query_edges (PyObject *edge_list)
{
	PyObject   *data = NULL;
	PyObject   *edge = NULL;
	GHashTable *ret  = NULL;

	data = PyObject_GetAttrString (edge_list, "data");
	if (data == NULL)
		return NULL;

	ret = g_hash_table_new (g_direct_hash, g_direct_equal);

	int pos = 0;
	while (PyDict_Next (data, &pos, NULL, &edge)) {
		PyObject *u, *v, *weight;
		u      = PyObject_GetAttrString (edge, "u");
		v      = PyObject_GetAttrString (edge, "v");
		weight = PyObject_GetAttrString (edge, "weight");
		float  weightf = (float) PyFloat_AsDouble (weight);
		float *weightp = g_new (float, 1);
		*weightp = weightf;

		/* FIXME: this is a really stupid way to hash (u,v) */
		g_hash_table_insert (ret, GINT_TO_POINTER (GPOINTER_TO_INT (u) + GPOINTER_TO_INT (v)), weightp);

		Py_DECREF (u);
		Py_DECREF (v);
		Py_DECREF (weight);
	}
	if (PyErr_Occurred ())
		goto error;

	Py_DECREF (data);

	return ret;

error:
	Py_XDECREF (data);
	if (ret)
		free_edges (ret);
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
		int       len   = g_slist_length (path);
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

/* Return the cost of a path. */
gint
cost (GSList* path, PyObject* goal, PyObject* fcost)
{
	gint c = -1;
	PyObject* py_path = PyList_New (0);
	PyObject* arglist = NULL;
	PyObject* result = NULL;

	/* FIXME - error checking */

	/* Build a list of nodes that is the path */
	for (GSList* p = path; p; p = g_slist_next (p)) {
		PyList_Insert (py_path, 0, (PyObject*) p->data);
	}

	arglist = Py_BuildValue ("(OO)", py_path, goal);
	result = PyEval_CallObject(fcost, arglist);

	Py_DECREF (arglist);
	Py_DECREF (py_path);

	/* FIXME - should raise a python exception */
	if (result == NULL) {
		return -1;
	}

	c = (gint) PyInt_AsLong (result);
	Py_DECREF (result);

	return c;
}

/* GCompareDataFunc for sorting the agenda queue in a best first search */
gint
f_cost_compare (gconstpointer a, gconstpointer b, gpointer data)
{
	GHashTable* costs = (GHashTable*) data;
	gint a_cost = GPOINTER_TO_INT (g_hash_table_lookup (costs, a));
	gint b_cost = GPOINTER_TO_INT (g_hash_table_lookup (costs, b));

	if (a < b) {
		return -1;
	}

	return 1;
}

/* Execute a best first search using fcost to evaluate the cost of each node.
 * Return a path from start to goal if there is one, or return NULL.
 */
GSList*
best_first_search (GHashTable* adjacency, GHashTable* edges, GSList* start,
		PyObject* goal, PyObject* fcost)
{
	/* Map nodes to f-costs */
	GHashTable* costs = g_hash_table_new (g_direct_hash, g_int_equal);
	GQueue* agenda = g_queue_new ();

	/* Push start onto the agenda */
	g_queue_push_head (agenda, (gpointer) start);

	/* Run until we run out of things to check */
	while (!g_queue_is_empty (agenda)) {
		GSList* path = (GSList*) g_queue_pop_head (agenda);
		GSList* successors = g_hash_table_lookup (adjacency, path->data);

		for (GSList* s = successors; s; s = g_slist_next (s)) {
			/* If this node is the goal prepend it to the path and
			 * return
			 */
			if (s->data == goal) {
				g_slist_prepend (path, s->data);
				return path;
			}
			/* If this node isn't the goal calculate its f-cost and
			 * insert it into agenda
			 */
			GSList* p = g_slist_copy (path);
			g_slist_prepend (p, s);
			g_hash_table_insert (costs, (gpointer) p,
					GINT_TO_POINTER (cost(p, goal, fcost)));
			g_queue_insert_sorted (agenda, (gpointer) p,
					f_cost_compare, (gpointer) costs);
		}
	}

	return NULL;
}

static PyObject*
aStar_search (PyObject* self, PyObject *args)
{
	PyObject*   adjacency_list;
	PyObject*   edge_list;
	PyObject*   start;
	PyObject*   end;
	PyObject*   f_cost;
	PyObject*   result;
	GHashTable* adjacency = NULL;
	GHashTable* edges     = NULL;

	/* Get all the arguments */
	if (!PyArg_ParseTuple (args, "OOOOO", &adjacency_list, &edge_list, &start, &end, &f_cost)) {
		return NULL;
	}

	/* Check that f_cost is a function */
	if (!PyCallable_Check (f_cost)) {
		PyErr_SetString (PyExc_RuntimeError, "f cost needs to be callable");
		return NULL;
	}

	/* Retrieve adjacency and edge lists */
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
	GSList* p = g_slist_alloc ();
	g_slist_prepend (p, start);
	GSList* path = best_first_search (adjacency, edges, p, end, f_cost);

	if (path) {
		result = PyList_New (0);
		for (GSList* node = path; node; node = g_slist_next (node)) {
			PyList_Insert (result, 0, (PyObject*) node->data);
		}
	} else {
		Py_INCREF (Py_None);
		result = Py_None;
	}

	return result;
}
