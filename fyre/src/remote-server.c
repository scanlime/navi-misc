/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * remote-server.c - Remote control mode, an interface for automating
 *                   Fyre rendering. Among other things, this is used
 *                   to implement slave nodes in a cluster.
 *
 *                   This communicates using a line oriented protocol
 *                   with SMTP-like numeric response codes.
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

#include <gnet.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "iterative-map.h"
#include "remote-server.h"
#include "de-jong.h"

typedef struct _RemoteServer      RemoteServer;
typedef struct _RemoteServerConn  RemoteServerConn;

struct _RemoteServer {
    GServer*             gserver;
    GHashTable*          command_hash;
    gboolean             have_gtk;
};

struct _RemoteServerConn {
    RemoteServer*        server;
    GConn*               gconn;

    /* State we maintain on behalf of the client */
    IterativeMap*        map;
    ParameterHolderPair  frame;
};

typedef void      (*RemoteServerCallback)     (RemoteServerConn*     self,
					       const char*           command,
					       const char*           parameters);


static void       remote_server_connect       (GServer*              gserver,
					       GConn*                gconn,
					       gpointer              user_data);
static void       remote_server_callback      (GConn*                gconn,
					       GConnEvent*           event,
					       gpointer              user_data);
static void       remote_server_disconnect    (RemoteServerConn*     self);
static void       remote_server_dispatch_line (RemoteServerConn*     self,
					       char*                 line);
static void       remote_server_send_response (RemoteServerConn*     self,
					       int                   response_code,
					       const char*           response_message,
					       ...);
static void       remote_server_send_binary   (RemoteServerConn*     self,
					       unsigned char*        data,
					       unsigned long         length);
static void       remote_server_add_command   (RemoteServer*         self,
					       const char*           command,
					       RemoteServerCallback  callback);
static void       remote_server_init_commands (RemoteServer*         self);


/************************************************************************************/
/***************************************************************** I/O Layer ********/
/************************************************************************************/

void              remote_server_main_loop     (int        port_number,
					       gboolean   have_gtk)
{
    RemoteServer self;

    self.have_gtk = have_gtk;
    self.gserver = gnet_server_new(NULL, port_number,
				   remote_server_connect, &self);
    self.command_hash = g_hash_table_new(g_str_hash, g_str_equal);
    remote_server_init_commands(&self);

    if (have_gtk)
	gtk_main();
    else
	g_main_loop_run(g_main_loop_new(NULL, FALSE));

    gnet_server_delete(self.gserver);
    g_hash_table_destroy(self.command_hash);
}

static void       remote_server_connect       (GServer*              gserver,
					       GConn*                gconn,
					       gpointer              user_data)
{
    RemoteServerConn* self = g_new0(RemoteServerConn, 1);

    self->server = (RemoteServer*) user_data;
    self->gconn = gconn;
    self->map = ITERATIVE_MAP(de_jong_new());

    gnet_conn_set_callback(gconn, remote_server_callback, self);
    gnet_conn_set_watch_error(gconn, TRUE);
    gnet_conn_readline(gconn);

    remote_server_send_response(self, FYRE_RESPONSE_READY,
				"Fyre rendering server ready");
}

static void       remote_server_callback      (GConn*                gconn,
					       GConnEvent*           event,
					       gpointer              user_data)
{
    RemoteServerConn* self = (RemoteServerConn*) user_data;
    switch (event->type) {

    case GNET_CONN_READ:
	remote_server_dispatch_line(self, event->buffer);
	gnet_conn_readline(gconn);
	break;

    case GNET_CONN_CLOSE:
    case GNET_CONN_TIMEOUT:
    case GNET_CONN_ERROR:
	remote_server_disconnect(self);
	break;

    default:
	break;
    }
}

static void       remote_server_disconnect    (RemoteServerConn*     self)
{
    gnet_conn_delete(self->gconn);
    iterative_map_stop_calculation(self->map);
    g_object_unref(self->map);
    g_free(self);
}

static void       remote_server_dispatch_line (RemoteServerConn*     self,
					       char*                line)
{
    char* args;
    RemoteServerCallback callback;

    args = strchr(line, ' ');
    if (args) {
	*args = '\0';
	args++;
    }
    else
	args = "";

    callback = (RemoteServerCallback)
	g_hash_table_lookup(self->server->command_hash, line);

    if (callback)
	callback(self, line, args);
    else
	remote_server_send_response(self, FYRE_RESPONSE_UNRECOGNIZED,
					"Command not recognized");
}

static void       remote_server_send_response (RemoteServerConn*  self,
					       int                response_code,
					       const char*        response_message,
					       ...)
{
    gchar* full_message;
    gchar* line;
    va_list ap;

    va_start(ap, response_message);
    full_message = g_strdup_vprintf(response_message, ap);
    va_end(ap);

    line = g_strdup_printf("%d %s\n", response_code, full_message);

    gnet_conn_write(self->gconn, line, strlen(line));

    g_free(full_message);
    g_free(line);
}

static void       remote_server_send_binary   (RemoteServerConn*  self,
					       unsigned char*     data,
					       unsigned long      length)
{
    remote_server_send_response(self, FYRE_RESPONSE_BINARY,
				"%d byte binary response", length);
    gnet_conn_write(self->gconn, data, length);
}

static void       remote_server_add_command   (RemoteServer*          self,
					       const char*            command,
					       RemoteServerCallback   callback)
{
    g_hash_table_insert(self->command_hash, (void*) command, callback);
}


/************************************************************************************/
/******************************************************** Command Implementations ***/
/************************************************************************************/

static void       cmd_set_param        (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    parameter_holder_set_from_line(PARAMETER_HOLDER(self->map), parameters);
    remote_server_send_response(self, FYRE_RESPONSE_OK, "ok");
}

static void       cmd_calc_start       (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    iterative_map_start_calculation(self->map);
    remote_server_send_response(self, FYRE_RESPONSE_OK, "ok");
}

static void       cmd_calc_stop        (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    iterative_map_stop_calculation(self->map);
    remote_server_send_response(self, FYRE_RESPONSE_OK, "ok");
}

static void       cmd_calc_status      (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    remote_server_send_response(self, FYRE_RESPONSE_PROGRESS, "iterations=%.3e density=%ld",
				self->map->iterations, HISTOGRAM_IMAGER(self->map)->peak_density);
}

static void       cmd_get_histogram_stream (RemoteServerConn*  self,
					    const char*        command,
					    const char*        parameters)
{
    guchar buffer[512 * 1024];
    gsize size;

    size = histogram_imager_export_stream(HISTOGRAM_IMAGER(self->map),
					  buffer, sizeof(buffer));
    remote_server_send_binary(self, buffer, size);
}

static void       remote_server_init_commands (RemoteServer*  self)
{
    remote_server_add_command(self, "set_param",            cmd_set_param);
    remote_server_add_command(self, "calc_start",           cmd_calc_start);
    remote_server_add_command(self, "calc_stop",            cmd_calc_stop);
    remote_server_add_command(self, "calc_status",          cmd_calc_status);
    remote_server_add_command(self, "get_histogram_stream", cmd_get_histogram_stream);
}


/* The End */
