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
#include "depth_limited.c"
#include "a_star.c"
#include "dijkstra.c"

static PyObject *depth_limited_search (PyObject* self, PyObject* args);
static PyObject *aStar_search         (PyObject* self, PyObject* args);
static PyObject *dijkstra_search      (PyObject* self, PyObject* args);

static PyMethodDef AlgorithmC_methods[] = {
	{"depthLimitedSearch", depth_limited_search, METH_VARARGS, "Execute a depth limited search of the graph"},
	{"aStarSearch",        aStar_search,         METH_VARARGS, "Execute the A* search of the graph"},
	{"dijkstraSearch",     dijkstra_search,      METH_VARARGS, "Execute dijkstra's single-source shortest-path search"},
        {NULL,                 NULL,                 0,            NULL},
};

PyMODINIT_FUNC
initalgorithms_c (void)
{
	(void) Py_InitModule ("algorithms_c", AlgorithmC_methods);
}
