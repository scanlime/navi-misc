/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * remote.c - Remote control mode, an interface for automating
 *            Fyre rendering. Among other things, this is used
 *            to implement slave nodes in a cluster.
 *
 *            This communicates using a line oriented protocol
 *            with SMTP-like numeric response codes.
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
#include <stdarg.h>
#include <stdlib.h>
#include "animation.h"
#include "iterative-map.h"
#include "remote.h"

static void       remote_class_init    (RemoteClass *klass);
static void       remote_init          (Remote *self);
static void       remote_dispose       (GObject *gobject);

static void       remote_send_response (Remote*          self,
					int              response_code,
					const char*      response_message,
					...);
static void       remote_send_binary   (Remote*          self,
					unsigned char*   data,
					unsigned long    length);
static void       remote_add_command   (Remote*          self,
					const char*      command,
					RemoteCallback   callback);
static void       remote_init_commands (Remote*          self);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType remote_get_type(void) {
    static GType anim_type = 0;

    if (!anim_type) {
	static const GTypeInfo dj_info = {
	    sizeof(RemoteClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) remote_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(Remote),
	    0,
	    (GInstanceInitFunc) remote_init,
	};

	anim_type = g_type_register_static(G_TYPE_OBJECT, "Remote", &dj_info, 0);
    }

    return anim_type;
}

static void remote_class_init(RemoteClass *klass) {
    GObjectClass *object_class;
    object_class = (GObjectClass*) klass;

    object_class->dispose      = remote_dispose;
}

static void remote_dispose(GObject *gobject) {
    Remote *self = REMOTE(gobject);

    if (self->command_hash) {
	g_hash_table_destroy(self->command_hash);
	self->command_hash = NULL;
    }
}

static void remote_init(Remote *self) {
    self->command_hash = g_hash_table_new(g_str_hash, g_str_equal);

    self->output_f = stdout;
    self->input_f = stdin;

    remote_init_commands(self);
}

Remote*    remote_new        (IterativeMap* map,
			      Animation*    animation,
			      gboolean      have_gtk)
{
    Remote *self = REMOTE(g_object_new(remote_get_type(), NULL));

    self->map = map;
    self->animation = animation;
    self->have_gtk = have_gtk;

    return self;
}


/************************************************************************************/
/***************************************************************** I/O Layer ********/
/************************************************************************************/

static void       remote_send_response (Remote*          self,
					int              response_code,
					const char*      response_message,
					...)
{
    va_list ap;

    fprintf(self->output_f, "%d ", response_code);

    va_start(ap, response_message);
    vfprintf(self->output_f, response_message, ap);
    va_end(ap);

    fprintf(self->output_f, "\n");
    fflush(self->output_f);
}

static void       remote_send_binary   (Remote*          self,
					unsigned char*   data,
					unsigned long    length)
{
    remote_send_response(self, 380, "%d byte binary response", length);
    fwrite(data, length, 1, self->output_f);
    fflush(self->output_f);
}

static void       remote_add_command   (Remote*          self,
					const char*      command,
					RemoteCallback   callback)
{
    g_hash_table_insert(self->command_hash, (void*) command, callback);
}

void              remote_main_loop     (Remote* self)
{
    char line[1024];
    char* args;
    RemoteCallback callback;

    remote_send_response(self, 220, "Fyre rendering server ready");

    while (fgets(line, sizeof(line)-1, self->input_f)) {
	line[sizeof(line)-1] = '\0';
	args = strchr(line, '\n');
	if (args)
	    *args = '\0';

	args = strchr(line, ' ');
	if (args) {
	    *args = '\0';
	    args++;
	}
	else
	    args = "";

	callback = (RemoteCallback) g_hash_table_lookup(self->command_hash, line);

	if (callback)
	    callback(self, line, args);
	else
	    remote_send_response(self, 500, "Command not recognized");

    }
}


/************************************************************************************/
/******************************************************** Command Implementations ***/
/************************************************************************************/

static void       cmd_set_param        (Remote*          self,
					const char*      command,
					const char*      parameters)
{
    parameter_holder_set_from_line(PARAMETER_HOLDER(self->map), parameters);
    remote_send_response(self, 250, "ok");
}

static void       cmd_calculate_timed  (Remote*          self,
					const char*      command,
					const char*      parameters)
{
    iterative_map_calculate_timed(self->map, atof(parameters));
    remote_send_response(self, 251, "iterations=%.3e density=%ld",
			 self->map->iterations, HISTOGRAM_IMAGER(self->map)->peak_density);
}

static void       cmd_get_histogram_stream (Remote*          self,
					    const char*      command,
					    const char*      parameters)
{
    guchar buffer[512 * 1024];
    gsize size;

    size = histogram_imager_export_stream(HISTOGRAM_IMAGER(self->map),
					  buffer, sizeof(buffer));
    remote_send_binary(self, buffer, size);
}

static void       remote_init_commands (Remote*          self)
{
    remote_add_command(self, "set_param",            cmd_set_param);
    remote_add_command(self, "calculate_timed",      cmd_calculate_timed);

    remote_add_command(self, "get_histogram_stream", cmd_get_histogram_stream);
}


/* The End */
