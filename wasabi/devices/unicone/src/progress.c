/*
 * progress.c - An abstract interface to progress reporter objects
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

#include <stdio.h>
#include "progress.h"

struct progress_operation* progress_operation_new    (struct progress_reporter*  reporter,
						      const char*                operation_name)
{
  if (!reporter)
    return NULL;
  return reporter->start(reporter, operation_name);
}

void                       progress_operation_update (struct progress_operation* self,
						      float                      completion)
{
  if (!self)
    return;
  self->update(self, completion);
}

void                       progress_operation_finish (struct progress_operation* self,
						      const char*                error)
{
  if (!self)
    return;
  self->update(self, 1.0f);
  self->finish(self, error);
}

void                       progress_reporter_delete  (struct progress_reporter*  self)
{
  if (!self)
    return;
  self->destroy(self);
}

void                       progress_message          (struct progress_reporter*  self,
						      const char*                message)
{
  if (!self)
    return;
  self->message(self, message);
}

void                       progress_error            (struct progress_reporter*  self,
						      const char*                error)
{
  if (!self)
    return;
  self->error(self, error);
}

/* The End */
