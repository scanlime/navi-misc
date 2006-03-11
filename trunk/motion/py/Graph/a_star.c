/*
 * a_star.c - A* heuristic search.
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
#include <Python.h>
#include "path_tree.h"
#include "utilities.h"

GHashTable*
node_from_PyDict (PyObject* dict)
{
	GHashTable* node = g_hash_table_new (g_str_hash, g_direct_equal);
	PyObject*   key;
	PyObject*   value;
	int         pos = 0;

	if (!PyDict_Check (dict)) {
		PyErr_SetString (PyExc_RuntimeError, "nodes need to be dictionary objects");
		g_hash_table_destroy (node);
		return NULL;
	}

	while (PyDict_Next (dict, &pos, &key, &value)) {
		char* bone = PyString_AsString (key);
		g_hash_table_insert (node, bone, value);
	}

	return node;
}

GSList*
generate_successors (GHashTable* adjacency, path_tree* node)
{
	GSList*     ret = NULL;
	GHashTable* bones = (GHashTable*) node->data;

	return ret;
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

	/* Build a list of nodes that is the path */ for (GSList* p = path; p; p = g_slist_next (p)) {
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

static void
free_path (gpointer key, gpointer value, gpointer data)
{
	g_slist_free ((GSList*) key);
}

static void
free_costs (GHashTable* table)
{
	g_hash_table_foreach (table, free_path, NULL);
	g_hash_table_destroy (table);
}

/* Execute a best first search using fcost to evaluate the cost of each node.
 * Return a path from start to goal if there is one, or return NULL.
 */
GSList*
heuristic_search (GHashTable* adjacency, PyObject* start, PyObject* goal,
		PyObject* fcost)
{
	/* Map nodes to f-costs */
	GHashTable* costs  = g_hash_table_new (g_direct_hash, g_int_equal);
	GQueue*     agenda = g_queue_new ();

	/* Push start onto the agenda */
	g_queue_push_head (agenda, path_tree_new (node_from_PyDict (start)));

	/* Run until we run out of things to check */
	while (!g_queue_is_empty (agenda)) {
		path_tree* path       = (path_tree*) g_queue_pop_head (agenda);
		GSList*    successors = generate_successors (adjacency, path);

		for (GSList* s = successors; s; s = g_slist_next (s)) {
			/* If this node is the goal prepend it to the path and
			 * return
			 * FIXME
			 */
			if (s->data == goal) {
				free_costs (costs);
				g_queue_free (agenda);
				return path;
			}
			/* If this node isn't the goal calculate its f-cost and
			 * insert it into agenda
			 * FIXME
			 */
		}
	}

	free_costs (costs);
	g_queue_free (agenda);
	return NULL;
}
