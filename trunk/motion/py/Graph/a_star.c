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

/* Return a GHashTable that is identical to the python dictionary object dict. */
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

/* GHFunc that inserts a key value pair from a GHashTable into a python
 * dictionary. The key should be a string and the value should be a python
 * object.
 */
void
insert_in_dict (const char* key, PyObject* value, PyObject* dict)
{
	PyDict_SetItem (dict, PyString_FromString (key), value);
}

/* Return a python dictionary with the same contents as the hash table contained
 * in node's data field.
 */
PyObject*
PyDict_from_node (path_tree* node)
{
	GHashTable* data = (GHashTable*) node->data;
	PyObject*   dict = PyDict_New ();

	if (dict == NULL) {
		PyErr_SetString (PyExc_RuntimeError, "unable to create a new dictionary");
		return NULL;
	}

	g_hash_table_foreach (data, (GHFunc) insert_in_dict, dict);

	return dict;
}

/* GHFunc for getting a list of the keys from a GHashTable. */
void
get_keys (char* key, PyObject* value, GSList** keys)
{
	(*keys) = g_slist_prepend ((*keys), key);
}

/* Recursively generate a list of all possible combinations of the successors
 * for each bone. Returns a GSList of path_tree nodes. Each path_tree node has a
 * GHashTable in its data field that maps a bone name (gchar*) to a position
 * (PyObject*).
 */
GSList*
combine_successors (GSList* successors, GSList* bone)
{
	GSList* ret = NULL;
	GSList* vs = (GSList*) successors->data;
	gchar*  bone_name = (gchar*) bone->data;

	/* Advance the pointer for the list of successor states. If this is the
	 * last bone, assemble the return value.
	 */
	successors = g_slist_next (successors);
	if (successors == NULL) {
		for (GSList* v = vs; v; v = g_slist_next (v)) {
			/* This goes in the data field of the path_tree */
			GHashTable* data = g_hash_table_new (g_str_hash, g_direct_equal);
			path_tree*  node = path_tree_new (data);
			/* Insert this bone and position into the hash table
			 * that is now the data field of the path_tree node.
			 */
			g_hash_table_insert (data, bone_name, v->data);
			/* Add the path_tree node to the return value */
			ret = g_slist_prepend (ret, node);
		}

		return ret;
	}

	/* Advance the pointer on the list of bones. If we were able to advance
	 * the successors pointer and not the bone pointer, we've got a problem.
	 */
	bone = g_slist_next (bone);
	if (bone == NULL) {
		/* FIXME - error */
	}

	/* For every position at this bone generate a list of the combinatoric
	 * states of all remaining bones and then add the position of this bone
	 * to each of those states.
	 */
	for (GSList* v = vs; v; v = g_slist_next (v)) {
		/* Combinatoric states of all remaining bones */
		GSList* nodes = combine_successors (successors, bone);
		/* For each combinatoric state add the position of this bone */
		for (GSList* n = nodes; n; n = g_slist_next (n)) {
			GHashTable* data = (GHashTable*) ((path_tree*) n)->data;
			g_hash_table_insert (data, bone_name, v->data);
		}
		/* Add the newly modified list of states to the return value */
		ret = g_slist_concat (ret, nodes);
	}

	return ret;
}

/* Return a list of the successor states for node. */
GSList*
generate_successors (GHashTable* adjacency, path_tree* node)
{
	GSList*     successors = NULL;
	GSList*     bones = NULL;
	GHashTable* frame = (GHashTable*) node->data;

	/* Obtain a list of the bone names from the dictionary */
	g_hash_table_foreach (frame, (GHFunc) get_keys, &bones);

	/* For every bone look up its current position and prepend the list of
	 * its neighbors to the list of successor states.
	 */
	for (GSList* bone = bones; bone; bone = g_slist_next (bone)) {
		PyObject* u = g_hash_table_lookup (frame, bone->data);
		GSList*   vs = g_hash_table_lookup (adjacency, u);
		successors = g_slist_prepend (successors, vs);
	}

	/* Reverse the list of successors because it needs to be in the same
	 * order as the list of bones
	 */
	successors = g_slist_reverse (successors);

	/* Use recursive combinatoric function to generate return value */
	return combine_successors (successors, bones);
}

/* Return true if the node represented by a is identical to the node represented
 * by b.
 */
gboolean
nodes_equal (path_tree* a, PyObject* b)
{
	return FALSE;
}

/* Return the cost of a path. */
gint
cost (path_tree* path, PyObject* goal, PyObject* fcost)
{
	PyObject* py_path = PyList_New (0);
	PyObject* arglist = NULL;
	PyObject* result  = NULL;
	gint      c       = -1;

	/* FIXME - error checking */

	/* Build a list of nodes that is the path */
	for (path_tree* p = path; p; p = p->parent) {
		PyList_Insert (py_path, 0, PyDict_from_node (p->data));
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
path_tree*
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
			path_tree* node = (path_tree*) s->data;
			/* If this node is the goal prepend it to the path and
			 * return
			 */
			if (nodes_equal (node, goal)) {
				g_slist_free (successors);
				g_queue_free (agenda);
				return node;
			}
			/* If this node isn't the goal calculate its f-cost and
			 * insert it into agenda
			 */
			g_hash_table_insert (costs, node,
					GINT_TO_POINTER (cost (node, goal, fcost)));
			g_queue_insert_sorted (agenda, node, f_cost_compare, costs);
		}
		g_slist_free (successors);
	}

	g_queue_free (agenda);
	return NULL;
}
