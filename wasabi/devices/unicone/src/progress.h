/*
 * progress.h - A simple object for reporting progress in an abstract way
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

/* Generic interface for reporting progress. Typical usage is something like:
 *
 * void* operation = progress->start(progress, "Cleaning antimatter extractor");
 * while (1) {
 *   do_something();
 *   progress->report(progress, operation, ((float) completed) / total);
 * }
 * if (successful)
 *   progress->finish(progress, operation, NULL);
 * else
 *   progress->finish(progress, operation, "Neutron clog in port 4");
 *
 */
struct progress_reporter {
  void* (*start)(struct progress_reporter *self, const char* operation_name);
  void (*report)(struct progress_reporter *self, void* operation, float completion);
  void (*finish)(struct progress_reporter *self, void* operation, const char *error);
  void *private;
};

/* Create a new progress reporter that just splats some dots onto stdout */
struct progress_reporter*  progress_reporter_console_new();
void                       progress_reporter_console_delete(struct progress_reporter *self);

#endif /* _H_PROGRESS */

/* The End */
