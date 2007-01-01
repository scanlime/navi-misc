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
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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

#include "config.h"
#include "platform.h"

/* Extra unixy includes for dropping privileges */
#ifdef HAVE_FORK
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#endif

#include <gnet.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "iterative-map.h"
#include "gui-util.h"
#include "histogram-view.h"
#include "remote-server.h"
#include "de-jong.h"

typedef struct _RemoteServer      RemoteServer;
typedef struct _RemoteServerConn  RemoteServerConn;

struct _RemoteServer {
    GServer*             gserver;
    GHashTable*          command_hash;
    GHashTable*          gui_hash;
    gboolean             have_gtk;
    gboolean             verbose;
};

struct _RemoteServerConn {
    RemoteServer*        server;
    GConn*               gconn;

    /* State we maintain on behalf of the client */
    IterativeMap*        map;
    ParameterHolderPair  frame;

    /* Temporary buffer for sending back histogram streams */
    guchar*              buffer;
    gsize                buffer_size;

    /* Optional GUI, enabled with set_gui_style */
    GtkWidget*           gui;
};

typedef void      (*RemoteServerCallback)     (RemoteServerConn*     self,
					       const char*           command,
					       const char*           parameters);
typedef void      (*RemoteGUIInitializer)     (RemoteServerConn*     self);

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
static void       remote_server_add_gui       (RemoteServer*         self,
					       const char*           name,
					       RemoteGUIInitializer  initializer);
static void       remote_server_init_commands (RemoteServer*         self);

static void       gui_init_none               (RemoteServerConn*     self);
static void       release_privileges          (RemoteServer*         self);


/************************************************************************************/
/***************************************************************** I/O Layer ********/
/************************************************************************************/

void              remote_server_main_loop     (int        port_number,
					       gboolean   have_gtk,
					       gboolean   verbose)
{
    RemoteServer self;

    self.have_gtk = have_gtk;
    self.verbose = verbose;

    self.gserver = gnet_server_new(NULL, port_number,
				   remote_server_connect, &self);
    if (!self.gserver) {
	printf("Unable to listen on port %d!\n", port_number);
	exit(1);
    }

    self.command_hash = g_hash_table_new(g_str_hash, g_str_equal);
    self.gui_hash = g_hash_table_new(g_str_hash, g_str_equal);
    remote_server_init_commands(&self);

    if (self.verbose)
	printf("Fyre server listening on port %d\n", port_number);

    /* At this point, now that we've bound to the port and such,
     * make sure we aren't running as a privileged user. If so,
     * ditch all privileges permanently.
     */
    release_privileges(&self);

    if (have_gtk)
	gtk_main();
    else
	g_main_loop_run(g_main_loop_new(NULL, FALSE));

    if (self.verbose)
	printf("Fyre server shutting down.\n");

    gnet_server_delete(self.gserver);
    g_hash_table_destroy(self.command_hash);
    g_hash_table_destroy(self.gui_hash);
}


#ifdef HAVE_FORK
/* Currently this is only valid on unixy OSes */
static void release_privileges(RemoteServer* self)
{
    struct passwd* nobody;

    if (getuid() != 0)
	return;
    if (self->verbose)
	printf("Fyre is running as root, dropping all privileges\n");
    
    nobody = getpwnam("nobody");
    if (!nobody) {
	printf("Can't find the 'nobody' user, refusing to run as root.\n");
	exit(1);
    }

    if (setuid(nobody->pw_uid) < 0) {
	perror("setuid");
	exit(1);
    }
}
#else
static void release_privileges(RemoteServer* self) {}
#endif /* HAVE_FORK */

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

    if (self->server->verbose)
	printf("[%s:%d] Connected\n", gconn->hostname, gconn->port);
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
    if (self->server->verbose)
	printf("[%s:%d] Disconnected\n", self->gconn->hostname, self->gconn->port);

    gnet_conn_delete(self->gconn);
    iterative_map_stop_calculation(self->map);
    gui_init_none(self);

    g_object_unref(self->map);
    if (self->buffer)
	g_free(self->buffer);
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
    int write_size;
    remote_server_send_response(self, FYRE_RESPONSE_BINARY,
				"%d byte binary response", length);
    while (length > 0) {
	write_size = MIN(length, 4096);
	gnet_conn_write(self->gconn, data, write_size);
	length -= write_size;
	data += write_size;
    }
}

static void       remote_server_add_command   (RemoteServer*          self,
					       const char*            command,
					       RemoteServerCallback   callback)
{
    g_hash_table_insert(self->command_hash, (void*) command, callback);
}

static void       remote_server_add_gui       (RemoteServer*         self,
					       const char*           name,
					       RemoteGUIInitializer  initializer)
{
    g_hash_table_insert(self->gui_hash, (void*) name, initializer);
}


/************************************************************************************/
/*************************************************** Shared convenience functions ***/
/************************************************************************************/

static void sig_histogram_view_update(IterativeMap *map, HistogramView *view)
{
    histogram_view_update(view);
}

static void remove_deleted_object_signals(gpointer user_data, GObject* deleted_object)
{
    GObject* object = G_OBJECT(user_data);
    g_signal_handlers_disconnect_matched(object, G_SIGNAL_MATCH_DATA, 0,
					 0, NULL, NULL, deleted_object);
}

static void connect_map_to_view(IterativeMap *map, HistogramView *view)
{
    /* Set up a view to automatically update when an iterative map
     * completes a round of calculations. Automatically remove the
     * handler when the view is destroyed.
     */
    g_signal_connect(G_OBJECT(map), "calculation-finished",
		     G_CALLBACK(sig_histogram_view_update), view);
    g_object_weak_ref(G_OBJECT(view), remove_deleted_object_signals, map);
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

static void       cmd_set_render_time  (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    self->map->render_time = atof(parameters);
    remote_server_send_response(self, FYRE_RESPONSE_OK, "ok");
}

static void       cmd_calc_start       (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    if (self->server->verbose)
	printf("[%s:%d] Starting calculation\n", self->gconn->hostname, self->gconn->port);

    iterative_map_start_calculation(self->map);
    remote_server_send_response(self, FYRE_RESPONSE_OK, "ok");
}

static void       cmd_calc_stop        (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    if (self->server->verbose)
	printf("[%s:%d] Pausing calculation\n", self->gconn->hostname, self->gconn->port);

    iterative_map_stop_calculation(self->map);
    remote_server_send_response(self, FYRE_RESPONSE_OK, "ok");
}

static void       cmd_calc_step        (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    /* Instead of (or in addition to) running calculations in
     * the background when idle, the client can force us to
     * calculate right now.
     */
    iterative_map_calculate_timed(self->map, self->map->render_time);
    remote_server_send_response(self, FYRE_RESPONSE_OK, "ok");
}


static void       cmd_calc_status      (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    if (self->server->verbose)
	printf("[%s:%d]  iterations: %.5e  density: %ld\n",
	       self->gconn->hostname, self->gconn->port,
	       self->map->iterations, HISTOGRAM_IMAGER(self->map)->peak_density);

    remote_server_send_response(self, FYRE_RESPONSE_PROGRESS, "iterations=%.20e density=%ld",
				self->map->iterations, HISTOGRAM_IMAGER(self->map)->peak_density);
}

static void       cmd_get_histogram_stream (RemoteServerConn*  self,
					    const char*        command,
					    const char*        parameters)
{
    gsize size;

    if (!self->buffer) {
	/* Allocate it with an initial size of 128kB */
	self->buffer_size = 128 * 1024;
	self->buffer = g_malloc(self->buffer_size);
    }

    size = histogram_imager_export_stream(HISTOGRAM_IMAGER(self->map),
					  self->buffer, self->buffer_size);
    remote_server_send_binary(self, self->buffer, size);

    /* If we used more than half the buffer, double its size.
     * This ensures that if we do run out of room, we'll have plenty
     * of space to send the remainder of the buffer next time.
     */
    if (size > (self->buffer_size / 2)) {
	g_free(self->buffer);
	self->buffer_size *= 2;
	self->buffer = g_malloc(self->buffer_size);
    }
}

static void       cmd_is_gui_available (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    if (self->server->have_gtk)
	remote_server_send_response(self, FYRE_RESPONSE_OK, "GTK GUI is available");
    else
	remote_server_send_response(self, FYRE_RESPONSE_FALSE, "No GUI is available");
}

static void       cmd_set_gui_style    (RemoteServerConn*  self,
					const char*        command,
					const char*        parameters)
{
    /* If GUI support is available at all, the client can choose
     * one of several possible frontends to use. This looks
     * up an initializer for that frontend and calls it.
     */
    RemoteGUIInitializer callback;

    if (!self->server->have_gtk) {
	remote_server_send_response(self, FYRE_RESPONSE_UNSUPPORTED, "No GUI is available");
	return;
    }

    callback = (RemoteGUIInitializer)
	g_hash_table_lookup(self->server->gui_hash, parameters);

    if (callback) {
	/* Remove the previous GUI */
	gui_init_none(self);

	callback(self);

	if (self->server->verbose)
	    printf("[%s:%d] GUI set to '%s'\n", self->gconn->hostname, self->gconn->port, parameters);
	remote_server_send_response(self, FYRE_RESPONSE_OK, "GUI style set to '%s'", parameters);
    }
    else {
	remote_server_send_response(self, FYRE_RESPONSE_BAD_VALUE, "Unrecognized GUI style");
    }
}

static void       gui_init_none       (RemoteServerConn*     self)
{
    /* Null GUI- just disables any previous GUI that might have been running */

    if (self->gui)
	gtk_widget_destroy(self->gui);
    self->gui = NULL;
}

static void       gui_init_simple     (RemoteServerConn*     self)
{
    /* Simple GUI, just a window holding our histogram view */
    GtkWidget* view;
    GtkWidget* window;

    view = histogram_view_new(HISTOGRAM_IMAGER(self->map));

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    fyre_set_icon_later(window);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Fyre Server");
    gtk_container_add(GTK_CONTAINER(window), view);
    gtk_widget_show_all(window);

    connect_map_to_view(self->map, HISTOGRAM_VIEW(view));

    self->gui = window;
}

static void       remote_server_init_commands (RemoteServer*  self)
{
    remote_server_add_command(self, "set_param",            cmd_set_param);
    remote_server_add_command(self, "set_gui_style",        cmd_set_gui_style);
    remote_server_add_command(self, "set_render_time",      cmd_set_render_time);
    remote_server_add_command(self, "is_gui_available",     cmd_is_gui_available);
    remote_server_add_command(self, "calc_start",           cmd_calc_start);
    remote_server_add_command(self, "calc_stop",            cmd_calc_stop);
    remote_server_add_command(self, "calc_step",            cmd_calc_step);
    remote_server_add_command(self, "calc_status",          cmd_calc_status);
    remote_server_add_command(self, "get_histogram_stream", cmd_get_histogram_stream);

    remote_server_add_gui(self, "none",    gui_init_none);
    remote_server_add_gui(self, "simple",  gui_init_simple);
}


/* The End */
