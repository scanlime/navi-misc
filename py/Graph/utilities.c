/*
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

#include "utilities.h"

float
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
free_values (gpointer key, gpointer value, gpointer data)
{
	g_slist_free ((GSList*) value);
}

void
free_adjacency (GHashTable *hash_table) {
	g_hash_table_foreach (hash_table, free_values, NULL);
	g_hash_table_destroy (hash_table);
}

void
free_floats (gpointer key, gpointer value, gpointer data)
{
	g_free (value);
}

void
free_edges (GHashTable *hash_table) {
	g_hash_table_foreach (hash_table, free_floats, NULL);
	g_hash_table_destroy (hash_table);
}

GHashTable *
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

GHashTable *
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
