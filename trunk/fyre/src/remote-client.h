/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * remote-client.h - A client for communicating with remote Fyre
 *                   servers. The remote Fyre servers may actually
 *                   be on the local machine, or they may be connected
 *                   via ssh or sockets.
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

#ifndef __REMOTE_CLIENT_H__
#define __REMOTE_CLIENT_H__

#include <gtk/gtk.h>
#include "animation.h"
#include "iterative-map.h"
#include "remote-server.h"

G_BEGIN_DECLS

#define REMOTE_CLIENT_TYPE            (remote_client_get_type ())
#define REMOTE_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), REMOTE_CLIENT_TYPE, RemoteClient))
#define REMOTE_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), REMOTE_CLIENT_TYPE, RemoteClientClass))
#define IS_REMOTE_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), REMOTE_CLIENT_TYPE))
#define IS_REMOTE_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), REMOTE_CLIENT_TYPE))

typedef struct _RemoteClient      RemoteClient;
typedef struct _RemoteClientClass RemoteClientClass;

struct _RemoteClient {
    GObject object;

    FILE* output_f;
    FILE* input_f;
};

struct _RemoteClientClass {
    GObjectClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType          remote_client_get_type         ();
RemoteClient*  remote_client_new_from_streams (FILE*    output_f,
					       FILE*    input_f);
RemoteClient*  remote_client_new_from_command (char*    shell_command);


G_END_DECLS

#endif /* __REMOTE_CLIENT_H__ */

/* The End */
