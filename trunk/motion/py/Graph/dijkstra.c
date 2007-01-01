/*
 * dijkstra.c - C implementation of Dijkstra's shortest path algorithm.
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

#include "dijkstra.h"
#include "utilities.h"

void
initialize_dijkstra_d (PyObject *u, GList *vs, GHashTable *d)
{
	g_hash_table_insert (d, u, GINT_TO_POINTER (-1));
}

static void
add_to_queue (PyObject *u, GList *vs, GQueue *q)
{
	g_queue_push_head (q, u);
}

/* Comparison function for sorting the agenda queue for Dijkstra's algorithm.
 */
static gint
dijkstra_compare (PyObject *u, PyObject *v, GHashTable *d)
{
	gint u_cost = GPOINTER_TO_INT (g_hash_table_lookup (d, u));
	gint v_cost;

	/* If u is infinity, put it after v in the queue */
	if (u_cost == -1) {
		return 1;
	}

	v_cost = GPOINTER_TO_INT (g_hash_table_lookup (d, v));
	/* If v is infinitiy, put it after u. Otherwise, compare the costs
	 * directly.
	 */
	if (v_cost == -1) {
		return -1;
	} else if (u_cost > v_cost) {
		return 1;
	} else if (u_cost == v_cost) {
		return 0;
	}

	return -1;
}

static GQueue*
build_dijkstra_queue (GHashTable *adjacency, GHashTable *d)
{
	GQueue* q = g_queue_new ();

	g_hash_table_foreach (adjacency, (GHFunc) add_to_queue, q);
	g_queue_sort (q, (GCompareDataFunc) dijkstra_compare, d);

	return q;
}

/* Update the table of shortest path estimates. If the path that ends with the
 * edge (u,v) is shorter than the currently stored path to v, replace the cost
 * in d with the new cost, and replace the old value of previous[v] with u.
 */
static void
relax (GHashTable* d, GHashTable* previous, PyObject* u, PyObject* v)
{
	gint u_cost = GPOINTER_TO_INT (g_hash_table_lookup (d, u));
	gint v_cost = GPOINTER_TO_INT (g_hash_table_lookup (d, v));

	if (v_cost == -1 || v_cost > u_cost + 1) {
		g_hash_table_insert (d, v, GINT_TO_POINTER (u_cost + 1));
		g_hash_table_insert (previous, v, u);
	}
}

/* The meat of Dijkstra's algorithm is here. */
void
find_shortest_paths (GHashTable* adjacency, GHashTable* d, GHashTable* previous)
{
	GSList *S = NULL;
	GQueue* agenda = build_dijkstra_queue (adjacency, d);

	/* Go through each node and calculate path costs */
	while (!g_queue_is_empty (agenda)) {
		/* Next node*/
		PyObject *u = (PyObject*) g_queue_pop_head (agenda);
		/* Nodes connected to the current node */
		GSList   *vs = (GSList*) g_hash_table_lookup (adjacency, u);
		/* Add the current node to the list of visited nodes */
		S = g_slist_prepend (S, u);
		/* For each connected node update path costs */
		for (GSList *v = vs; v; v = g_slist_next (v)) {
			relax (d, previous, u, (PyObject*) v->data);
		}
		/* Re-sort the queue so that the node with the minimum cost is
		 * in front
		 */
		g_queue_sort (agenda, (GCompareDataFunc) dijkstra_compare, d);
	}

	/* Clean up */
	g_slist_free (S);
	g_queue_free (agenda);
}
