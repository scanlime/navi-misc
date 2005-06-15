/*
 * motion.c - python class which wraps the AMC data
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

#include "motion.h"

static void      Motion_dealloc (Motion *motion);
static PyObject *Motion_getAttr (Motion *motion, char *name);
static void      Motion_setAttr (Motion *motion, char *name, PyObject *v);
static PyObject *Motion_repr    (Motion *motion);

static PyMethodDef Motion_methods[] = {
	{NULL, NULL, 0, NULL},
};

PyTypeObject Motion_Type = {
	PyObject_HEAD_INIT(NULL)
	0,				// ob_size
	"AMC Motion",			// tp_name
	sizeof (Motion),		// tp_basicsize
	0,				// tp_itemsize
	(destructor) Motion_dealloc,	// tp_dealloc
	0,				// tp_print
	(getattrfunc) Motion_getAttr,	// tp_getattr
	(setattrfunc) Motion_setAttr,	// tp_setattr
	0,				// tp_compare
	(reprfunc) Motion_repr,		// tp_repr
	0,				// tp_as_number
	0,				// tp_as_sequence
	0,				// tp_as_mapping
	0,				// tp_as_hash
	0,				// tp_call
	0,				// tp_str
	0,				// tp_getattro
	0,				// tp_setattro
	0,				// tp_as_buffer
	0,				// tp_flags
	0,				// tp_doc
	0,				// tp_traverse
	0,				// tp_clear
	0,				// tp_richcompare
	0,				// tp_weaklistoffset
	0,				// tp_iter
	0,				// tp_iternext
	Motion_methods,			// tp_methods
	0,				// tp_members

};

PyObject *CreateMotion ()
{
	Motion *m = (Motion *) PyObject_NEW (Motion, &Motion_Type);
	return (PyObject *) m;
}

static void
Motion_dealloc (Motion *motion)
{
}

static PyObject *
Motion_getAttr (Motion *motion, char *name)
{
}

static void
Motion_setAttr (Motion *motion, char *name, PyObject *v)
{
}

static PyObject *
Motion_repr (Motion *motion)
{
}
