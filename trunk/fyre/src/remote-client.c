/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * remote-client.c - A client for communicating with remote Fyre
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

#include <stdio.h>
#include <string.h>
#include "remote-client.h"

static void       remote_client_class_init    (RemoteClientClass*    klass);
static void       remote_client_init          (RemoteClient*         self);
static void       remote_client_dispose       (GObject*              gobject);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType remote_client_get_type(void) {
    static GType anim_type = 0;

    if (!anim_type) {
	static const GTypeInfo dj_info = {
	    sizeof(RemoteClientClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) remote_client_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(RemoteClient),
	    0,
	    (GInstanceInitFunc) remote_client_init,
	};

	anim_type = g_type_register_static(G_TYPE_OBJECT, "RemoteClient", &dj_info, 0);
    }

    return anim_type;
}

static void remote_client_class_init(RemoteClientClass *klass) {
    GObjectClass *object_class;
    object_class = (GObjectClass*) klass;

    object_class->dispose      = remote_client_dispose;
}

static void remote_client_dispose(GObject *gobject) {
    RemoteClient *self = REMOTE_CLIENT(gobject);

    if (self->output_f) {
	fclose(self->output_f);
	self->output_f = NULL;
    }
    if (self->input_f) {
	fclose(self->input_f);
	self->input_f = NULL;
    }
}

static void remote_client_init(RemoteClient *self) {
}

RemoteClient*  remote_client_new_from_streams (FILE*    output_f,
					       FILE*    input_f)
{
    RemoteClient *self = REMOTE_CLIENT(g_object_new(remote_client_get_type(), NULL));

    self->output_f = output_f;
    self->input_f = input_f;

    return self;
}

RemoteClient*  remote_client_new_from_command (char*    shell_command)
{
    FILE *in, *out;


    return remote_client_new_from_streams(out, in);
}

/************************************************************************************/
/***************************************************************** I/O Layer ********/
/************************************************************************************/

/* The End */
