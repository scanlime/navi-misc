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
#include <stdarg.h>
#include <stdlib.h>
#include "remote-client.h"

static void       remote_client_class_init    (RemoteClientClass*    klass);
static void       remote_client_init          (RemoteClient*         self);
static void       remote_client_dispose       (GObject*              gobject);
static void       remote_client_callback      (GConn*                gconn,
					       GConnEvent*           event,
					       gpointer              user_data);
static void       remote_client_recv_binary   (RemoteClient*         self,
					       GConnEvent*           event);
static void       remote_client_recv_line     (RemoteClient*         self,
					       GConnEvent*           event);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType remote_client_get_type(void)
{
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

static void remote_client_class_init(RemoteClientClass *klass)
{
    GObjectClass *object_class;
    object_class = (GObjectClass*) klass;

    object_class->dispose      = remote_client_dispose;
}

static void remote_client_dispose(GObject *gobject)
{
    RemoteClient *self = REMOTE_CLIENT(gobject);

    if (self->gconn) {
	gnet_conn_delete(self->gconn);
	self->gconn = NULL;
    }

    if (self->response_queue) {
	/* Empty the queue of any outstanding requests */
	RemoteClosure *closure;
	while ((closure = g_queue_pop_tail(self->response_queue)))
	    g_free(closure);

	/* Free it */
	g_queue_free(self->response_queue);
	self->response_queue = NULL;
    }
}

static void remote_client_init(RemoteClient *self)
{
    self->response_queue = g_queue_new();
}

RemoteClient*  remote_client_new              (const gchar*      hostname,
					       gint              port)
{
    RemoteClient *self = REMOTE_CLIENT(g_object_new(remote_client_get_type(), NULL));

    self->gconn = gnet_conn_new(hostname, port, remote_client_callback, self);
    gnet_conn_set_watch_error(self->gconn, TRUE);
    gnet_conn_readline(self->gconn);

    return self;
}

gboolean       remote_client_is_connected     (RemoteClient*     self)
{
    return gnet_conn_is_connected(self->gconn);
}


/************************************************************************************/
/************************************************************** Low-level Interface */
/************************************************************************************/

void           remote_client_command          (RemoteClient*     self,
					       RemoteCallback    callback,
					       gpointer          user_data,
					       const gchar*      format,
					       ...)
{
    RemoteClosure *closure = g_new0(RemoteClosure, 1);
    gchar* full_message;
    gchar* line;
    va_list ap;

    /* Add the response callback to our queue */
    closure->callback = callback;
    closure->user_data = user_data;
    g_queue_push_head(self->response_queue, closure);

    /* Assemble the caller's formatted string */
    va_start(ap, format);
    full_message = g_strdup_vprintf(format, ap);
    va_end(ap);

    /* Send a one-line command */
    line = g_strdup_printf("%s\n", full_message);
    gnet_conn_write(self->gconn, line, strlen(line));
    g_free(full_message);
    g_free(line);
}

static void       remote_client_callback      (GConn*                gconn,
					       GConnEvent*           event,
					       gpointer              user_data)
{
    RemoteClient* self = (RemoteClient*) user_data;
    switch (event->type) {

    case GNET_CONN_READ:
	if (self->current_binary_response)
	    remote_client_recv_binary(self, event);
	else
	    remote_client_recv_line(self, event);
	break;

    default:
	break;
    }
}

static void       remote_client_recv_binary   (RemoteClient*         self,
					       GConnEvent*           event)
{
    RemoteClosure* closure = g_queue_pop_tail(self->response_queue);
    RemoteResponse* response = self->current_binary_response;
    self->current_binary_response = NULL;

    g_assert(closure != NULL);

    response->data = event->buffer;
    closure->callback(self, response, closure->user_data);

    g_free(closure);
    g_free(response->message);
    g_free(response);

    gnet_conn_readline(self->gconn);
}

static void       remote_client_recv_line     (RemoteClient*         self,
					       GConnEvent*           event)
{
    RemoteResponse *response = g_new0(RemoteResponse, 1);

    response->code = strtol(event->buffer, &response->message, 10);
    if (*response->message)
	response->message++;
    response->message = g_strdup(response->message);

    if (response->code == FYRE_RESPONSE_BINARY) {
	/* Extract the length of the binary response, then start
	 * reading the binary data itself.
	 */
	response->data_length = strtol(response->message, &response->message, 10);
	if (*response->message)
	    response->message++;

	self->current_binary_response = response;
	gnet_conn_readn(self->gconn, response->data_length);
    }
    else {
	/* We're done, signal the callback and start waiting
	 * for another normal response line.
	 */
	RemoteClosure* closure = g_queue_pop_tail(self->response_queue);
	g_assert(closure != NULL);
	closure->callback(self, response, closure->user_data);
	g_free(closure);
	g_free(response->message);
	g_free(response);

	gnet_conn_readline(self->gconn);
    }
}

/* The End */
