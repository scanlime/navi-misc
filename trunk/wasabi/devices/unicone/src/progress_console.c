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

static void*  console_start   (struct progress_reporter* self,
			       const char*               operation_name);
static void   console_report  (struct progress_reporter* self,
			       void*                     operation,
			       float                     completion);
static void   console_finish  (struct progress_reporter* self,
			       void*                     operation,
			       const char*               error);

struct console_operation {
  int dots_written;
};

struct progress_reporter* progress_reporter_console_new()
{
  struct progress_reporter* self;

  self = malloc(sizeof(struct progress_reporter));
  assert(self != NULL);
  memset(self, 0, sizeof(struct progress_reporter));

  self->start = console_start;
  self->report = console_report;
  self->finish = console_finish;

  return self;
}

void progress_reporter_console_delete(struct progress_reporter *self)
{
  free(self);
}

static void* console_start(struct progress_reporter* self,
			   const char* operation_name)
{
  struct console_operation *op;

  op = malloc(sizeof(struct console_operation));
  assert(op != NULL);
  memset(op, 0, sizeof(struct console_operation));

  fprintf(stderr, "%s ", operation_name);
  fflush(stderr);

  return op;
}

static void console_report(struct progress_reporter* self,
			   void* operation, float completion)
{
  struct console_operation *op = operation;
  const int num_dots = 40;
  int current_dots;

  current_dots = (int)(completion * num_dots + 0.5);
  while (op->dots_written < current_dots) {
    fprintf(stderr, ".");
    op->dots_written++;
  }
  fflush(stderr);
}

static void console_finish(struct progress_reporter* self,
			   void* operation, const char *error)
{
  struct console_operation *op = operation;

  if (error)
    fprintf(stderr, " [%s]\n", error);
  else
    fprintf(stderr, " Done.\n");

  free(op);
}

/* The End */
