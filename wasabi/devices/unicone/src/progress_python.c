/*
 * progress_python.c - A progress reporter object that wraps an analogous
 *                     set of Python objects implementing a progress reporter
 *                     and progress operations.
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
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

#include "progress_python.h"
#include <assert.h>
#include <string.h>
#include <malloc.h>

static struct progress_operation* python_start      (struct progress_reporter*  self,
						     const char*                operation_name);
static void                       python_message    (struct progress_reporter*  self,
						     const char*                message);
static void                       python_error      (struct progress_reporter*  self,
						     const char*                error);
static void                       python_destroy    (struct progress_reporter*  self);
static void                       python_op_update  (struct progress_operation* self,
						     float                      completion);
static void                       python_op_finish  (struct progress_operation* self,
						     const char*                error);

struct python_reporter {
  struct progress_reporter super;
  PyObject *pyo;
};

struct python_operation {
  struct progress_operation super;
  PyObject *pyo;
};

struct progress_reporter* progress_reporter_python_new(PyObject *pyo)
{
  struct python_reporter* self;

  self = malloc(sizeof(struct python_reporter));
  assert(self != NULL);
  memset(self, 0, sizeof(struct python_reporter));

  self->super.start = python_start;
  self->super.message = python_message;
  self->super.error = python_error;
  self->super.destroy = python_destroy;

  self->pyo = pyo;

  return (struct progress_reporter*) self;
}

static struct progress_operation* python_start      (struct progress_reporter*  _self,
						     const char*                operation_name)
{
  struct python_reporter* self = (struct python_reporter*) _self;
  struct python_operation *op;

  op = malloc(sizeof(struct python_operation));
  assert(op != NULL);
  memset(op, 0, sizeof(struct python_operation));

  op->super.update = python_op_update;
  op->super.finish = python_op_finish;

  op->pyo = PyObject_CallMethod(self->pyo, "start", "s", operation_name);

  return (struct progress_operation*) op;
}

static void                       python_message    (struct progress_reporter*  _self,
						     const char*                message)
{
  struct python_reporter* self = (struct python_reporter*) _self;

  Py_XDECREF(PyObject_CallMethod(self->pyo, "message", "s", message));
}

static void                       python_error      (struct progress_reporter*  _self,
						     const char*                error)
{
  struct python_reporter* self = (struct python_reporter*) _self;

  Py_XDECREF(PyObject_CallMethod(self->pyo, "error", "s", error));
}

static void                       python_destroy    (struct progress_reporter*  _self)
{
  struct python_reporter* self = (struct python_reporter*) _self;

  Py_XDECREF(self->pyo);
  free(self);
}

static void                       python_op_update  (struct progress_operation* _self,
						     float                      completion)
{
  struct python_operation* self = (struct python_operation*) _self;

  Py_XDECREF(PyObject_CallMethod(self->pyo, "update", "f", completion));
}

static void                       python_op_finish  (struct progress_operation* _self,
						     const char*                error)
{
  struct python_operation* self = (struct python_operation*) _self;

  Py_XDECREF(PyObject_CallMethod(self->pyo, "finish", "s", error));

  Py_XDECREF(self->pyo);
  free(self);
}

/* The End */
