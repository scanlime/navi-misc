/*
 * progress_console.c - A simple progress reporter that uses stderr
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "progress.h"

static struct progress_operation* console_start      (struct progress_reporter*  self,
						      const char*                operation_name);
static void                       console_message    (struct progress_reporter*  self,
						      const char*                message);
static void                       console_error      (struct progress_reporter*  self,
						      const char*                error);
static void                       console_destroy    (struct progress_reporter*  self);
static void                       console_op_update  (struct progress_operation* self,
						      float                      completion);
static void                       console_op_finish  (struct progress_operation* self,
						      const char*                error);

struct console_reporter {
  struct progress_reporter super;
};

struct console_operation {
  struct progress_operation super;
  int dots_written;
};

struct progress_reporter* progress_reporter_console_new()
{
  struct console_reporter* self;

  self = malloc(sizeof(struct console_reporter));
  assert(self != NULL);
  memset(self, 0, sizeof(struct console_reporter));

  self->super.start = console_start;
  self->super.message = console_message;
  self->super.error = console_error;
  self->super.destroy = console_destroy;

  return (struct progress_reporter*) self;
}

static struct progress_operation* console_start      (struct progress_reporter*  _self,
						      const char*                operation_name)
{
  struct console_reporter* self = (struct console_reporter*) _self;
  struct console_operation *op;

  op = malloc(sizeof(struct console_operation));
  assert(op != NULL);
  memset(op, 0, sizeof(struct console_operation));

  op->super.update = console_op_update;
  op->super.finish = console_op_finish;

  fprintf(stderr, "%-20s ", operation_name);
  fflush(stderr);

  return (struct progress_operation*) op;
}

static void                       console_message    (struct progress_reporter*  _self,
						      const char*                message)
{
  struct console_reporter* self = (struct console_reporter*) _self;

  fprintf(stderr, "%s\n", message);
}

static void                       console_error      (struct progress_reporter*  _self,
						      const char*                error)
{
  struct console_reporter* self = (struct console_reporter*) _self;

  fprintf(stderr, "*** Error: %s\n", error);
}

static void                       console_destroy    (struct progress_reporter*  _self)
{
  struct console_reporter* self = (struct console_reporter*) _self;
  free(self);
}

static void                       console_op_update  (struct progress_operation* _self,
						      float                      completion)
{
  struct console_operation* self = (struct console_operation*) _self;
  const int num_dots = 40;
  int current_dots;

  current_dots = (int)(completion * num_dots + 0.5);
  while (self->dots_written < current_dots) {
    fprintf(stderr, ".");
    self->dots_written++;
  }
  fflush(stderr);
}

static void                       console_op_finish  (struct progress_operation* _self,
						      const char*                error)
{
  struct console_operation* self = (struct console_operation*) _self;

  if (error)
    fprintf(stderr, " [%s]\n", error);
  else
    fprintf(stderr, " Done.\n");

  free(self);
}

/* The End */
