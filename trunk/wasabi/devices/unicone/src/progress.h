/*
 * progress.h - An abstract base class for reporting progress. Progress
 *              can be reported in the form of simple error and status
 *              messages, or a task can be broken into any number of
 *              'operations' whose completion can be reported as a number
 *              between 0 and 1. Operations can complete successfully,
 *              or with an error string.
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

#ifndef _H_PROGRESS
#define _H_PROGRESS

/* Generic interface for progress reporters. Since the concrete progress
 * reporter implementation is responsible for creating and destroying all
 * objects and only requires that these entry points be preserved, these
 * two structures can be 'subclassed' by embedding them as the first entry
 * in a larger structure.
 */
struct progress_reporter {
  struct progress_operation* (*start)    (struct progress_reporter*  self,
					  const char*                operation_name);
  void                       (*message)  (struct progress_reporter*  self,
					  const char*                message);
  void                       (*error)    (struct progress_reporter*  self,
					  const char*                error);
  void                       (*destroy)  (struct progress_reporter*  self);
};

struct progress_operation {
  void                       (*update)   (struct progress_operation* self,
					  float                      completion);
  void                       (*finish)   (struct progress_operation* self,
					  const char*                error);
};

/* Abstract interface to any progress reporter object.
 * All interfaces are no-ops if the supplied progress reporter is NULL,
 * so it's easy to use in a context where progress reports are optional.
 */
struct progress_operation* progress_operation_new    (struct progress_reporter*  reporter,
						      const char*                operation_name);
void                       progress_operation_update (struct progress_operation* self,
						      float                      completion);
void                       progress_operation_finish (struct progress_operation* self,
						      const char*                error);
void                       progress_reporter_delete  (struct progress_reporter*  self);
void                       progress_message          (struct progress_reporter*  self,
						      const char*                message);
void                       progress_error            (struct progress_reporter*  self,
						      const char*                error);

#endif /* _H_PROGRESS */

/* The End */
