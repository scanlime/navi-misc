/*
 * remote.h - Remote control mode, an interface for automating
 *            Fyre rendering. Among other things, this is used
 *            to implement slave nodes in a cluster.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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

#ifndef __REMOTE_H__
#define __REMOTE_H__

#include <gtk/gtk.h>
#include "animation.h"
#include "iterative-map.h"

G_BEGIN_DECLS

#define REMOTE_TYPE            (remote_get_type ())
#define REMOTE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), REMOTE_TYPE, Remote))
#define REMOTE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), REMOTE_TYPE, RemoteClass))
#define IS_REMOTE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), REMOTE_TYPE))
#define IS_REMOTE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), REMOTE_TYPE))

typedef struct _Remote      Remote;
typedef struct _RemoteClass RemoteClass;

struct _Remote {
  GObject object;

  IterativeMap *map;
  Animation *animation;
  gboolean have_gtk;

  FILE* output_f;
  FILE* input_f;
  GHashTable *command_hash;

  gboolean main_loop_running;
};

struct _RemoteClass {
  GObjectClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

typedef void (*RemoteCallback)  (Remote*          remote,
				 const char*      command,
				 const char*      parameters);

GType      remote_get_type      ();
Remote*    remote_new           (IterativeMap*    map,
				 Animation*       animation,
				 gboolean         have_gtk);
void       remote_main_loop     (Remote*          self);

G_END_DECLS

#endif /* __REMOTE_H__ */

/* The End */
