/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * remote-client.c - A client for communicating with remote Fyre
 *                   servers. The remote Fyre servers may actually
 *                   be on the local machine, or they may be connected
 *                   via ssh or sockets.
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
static void       remote_client_update_status (RemoteClient*         self,
					       const gchar*          fmt,
					       ...);
static void       histogram_merge_callback    (RemoteClient*         self,
					       RemoteResponse*       response,
					       gpointer              user_data);
static void       remote_client_start_retry   (RemoteClient*         self);
static void       remote_client_stop_retry    (RemoteClient*         self);
static gboolean   remote_client_retry_callback(gpointer              user_data);
static void       remote_client_empty_queue   (RemoteClient*         self);

/* Smallest time interval, in seconds, to allow in speed calculations */
#define MINIMUM_SPEED_WINDOW 1.0


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

    remote_client_stop_retry(self);

    if (self->gconn) {
	gnet_conn_delete(self->gconn);
	self->gconn = NULL;
    }

    if (self->response_queue) {
	remote_client_empty_queue(self);
	g_queue_free(self->response_queue);
	self->response_queue = NULL;
    }

    if (self->status_speed_timer) {
	g_timer_destroy(self->status_speed_timer);
	self->status_speed_timer = NULL;
    }
    if (self->stream_speed_timer) {
	g_timer_destroy(self->stream_speed_timer);
	self->stream_speed_timer = NULL;
    }
    if (self->stream_request_timer) {
	g_timer_destroy(self->stream_request_timer);
	self->stream_request_timer = NULL;
    }
}

static
void       remote_client_empty_queue   (RemoteClient*         self)
{
    /* Empty the queue of any outstanding requests */
    RemoteClosure *closure;
    while ((closure = g_queue_pop_tail(self->response_queue)))
	g_free(closure);
}

static void remote_client_init(RemoteClient *self)
{
    self->response_queue = g_queue_new();
    self->status_speed_timer = g_timer_new();
    self->stream_speed_timer = g_timer_new();
    self->stream_request_timer = g_timer_new();

    /* Default stream interval: every second */
    self->min_stream_interval = 1.0;

    /* By default, retry connections every minute */
    self->retry_timeout = 60.0;
    self->is_retry_enabled = TRUE;
}

RemoteClient*  remote_client_new              (const gchar*          hostname,
					       gint                  port)
{
    RemoteClient *self = REMOTE_CLIENT(g_object_new(remote_client_get_type(), NULL));

    self->host = hostname;
    self->port = port;

    return self;
}

void           remote_client_set_status_cb    (RemoteClient*         self,
					       RemoteStatusCallback  status_cb,
					       gpointer              user_data)
{
    self->status_callback = status_cb;
    self->status_callback_user_data = user_data;
}

void           remote_client_set_speed_cb     (RemoteClient*         self,
					       RemoteSpeedCallback   speed_cb,
					       gpointer              user_data)
{
    self->speed_callback = speed_cb;
    self->speed_callback_user_data = user_data;
}

void           remote_client_connect          (RemoteClient*         self)
{
    /* Clean up after the previous connection, if we need to */
    if (self->gconn) {
	gnet_conn_delete(self->gconn);
	self->gconn = NULL;
    }
    remote_client_empty_queue(self);

    /* Reset our speed counters and rate limiting timers */
    self->iter_accumulator = 0;
    self->byte_accumulator = 0;
    self->iters_per_sec = 0;
    self->bytes_per_sec = 0;
    g_timer_start(self->stream_request_timer);
    g_timer_start(self->status_speed_timer);
    g_timer_start(self->stream_speed_timer);

    /* Create the new connection object */
    self->gconn = gnet_conn_new(self->host, self->port, remote_client_callback, self);
    gnet_conn_set_watch_error(self->gconn, TRUE);
    gnet_conn_connect(self->gconn);
    gnet_conn_readline(self->gconn);

    remote_client_update_status(self, "Connecting...");
}

static
void       remote_client_start_retry   (RemoteClient*         self)
{
    remote_client_stop_retry(self);
    if (self->is_retry_enabled)
	self->retry_timer = g_timeout_add(self->retry_timeout * 1000,
					  remote_client_retry_callback,
					  self);
}

static
void       remote_client_stop_retry    (RemoteClient*         self)
{
    if (self->retry_timer) {
	g_source_remove(self->retry_timer);
	self->retry_timer = 0;
    }
}

static
gboolean   remote_client_retry_callback(gpointer              user_data)
{
    RemoteClient* self = REMOTE_CLIENT(user_data);
    self->retry_timer = 0;
    remote_client_connect(self);
    return FALSE;
}


gboolean       remote_client_is_ready         (RemoteClient*     self)
{
    return self->is_ready;
}


/************************************************************************************/
/************************************************************** Low-level Interface */
/************************************************************************************/

static void       remote_client_update_status (RemoteClient*         self,
					       const gchar*          fmt,
					       ...)
{
    gchar *msg;
    va_list ap;

    if (!self->status_callback)
	return;

    va_start(ap, fmt);
    msg = g_strdup_vprintf(fmt, ap);
    va_end(ap);

    self->status_callback(self, msg, self->status_callback_user_data);

    g_free(msg);
}

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

    case GNET_CONN_CONNECT:
	remote_client_update_status(self, "Connected");
	break;

    case GNET_CONN_CLOSE:
	self->is_ready = FALSE;
	remote_client_update_status(self, "Connection closed");
	remote_client_start_retry(self);
	break;

    case GNET_CONN_TIMEOUT:
	self->is_ready = FALSE;
	remote_client_update_status(self, "Timed out");
	remote_client_start_retry(self);
	break;

    case GNET_CONN_ERROR:
	self->is_ready = FALSE;
	remote_client_update_status(self, "Connection error");
	remote_client_start_retry(self);
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
    if (closure->callback)
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
    RemoteClosure *closure;

    response->code = strtol(event->buffer, &response->message, 10);
    if (*response->message)
	response->message++;
    response->message = g_strdup(response->message);

    if (response->code == FYRE_RESPONSE_BINARY) {
	/* Extract the length of the binary response, then start
	 * reading the binary data itself. Note that if we
	 * have a zero-length binary message, skip the next
	 * stage and fall through to processing a normal message.
	 */
	response->data_length = strtol(response->message, NULL, 10);

	if (response->data_length > 0) {
	    self->current_binary_response = response;
	    gnet_conn_readn(self->gconn, response->data_length);
	    return;
	}
    }

    /* We're done, signal the callback and start waiting
     * for another normal response line.
     */
    closure = g_queue_pop_tail(self->response_queue);

    if (closure) {
	/* This was an answer to some request. Invoke the callback
	 * if one was specified.
	 */
	if (closure->callback)
	    closure->callback(self, response, closure->user_data);
	g_free(closure);
    }
    else {
	/* This was unsolicited- should only occur for the server ready message */
	if (response->code == FYRE_RESPONSE_READY) {
	    self->is_ready = TRUE;
	    remote_client_update_status(self, "Ready");
	}
	else {
	    remote_client_update_status(self, "Protocol error");
	}
    }

    g_free(response->message);
    g_free(response);

    gnet_conn_readline(self->gconn);
}


/************************************************************************************/
/************************************************************* High-level Interface */
/************************************************************************************/

static void    set_param_callback             (RemoteClient*     self,
					       RemoteResponse*   response,
					       gpointer          user_data)
{
    self->pending_param_changes--;
}

void           remote_client_send_param       (RemoteClient*     self,
					       ParameterHolder*  ph,
					       const gchar*      name)
{
    /* Serialize one parameter value, and send it to the server */

    GValue val, strval;
    const gchar* string;
    GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(ph), name);
    g_assert(spec != NULL);

    memset(&val, 0, sizeof(val));
    g_value_init(&val, spec->value_type);
    g_object_get_property(G_OBJECT(ph), name, &val);

    memset(&strval, 0, sizeof(strval));
    g_value_init(&strval, G_TYPE_STRING);
    g_value_transform(&val, &strval);
    string = g_value_get_string(&strval);

    /* 'string' will be NULL if the value couldn't be serialized- currently
     * this happens for colors, since we get the GdkColor property rather than
     * the corresponding string property. Currently this isn't a problem since
     * render nodes don't deal with colors, but it's something to be aware of.
     */
    if (string) {
	self->pending_param_changes++;
	remote_client_command(self, set_param_callback, NULL, "set_param %s = %s",
			      name, string);
    }

    g_value_unset(&strval);
    g_value_unset(&val);
}

void           remote_client_send_all_params  (RemoteClient*     self,
					       ParameterHolder*  ph)
{
    /* Find all serializable parameters, and send them */

    guint n_properties;
    GParamSpec** properties;
    int i;

    properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(ph), &n_properties);

    for (i=0; i<n_properties; i++)
	if (properties[i]->flags & PARAM_SERIALIZED)
	    remote_client_send_param(self, ph, properties[i]->name);

    g_free(properties);
}

static void    histogram_merge_callback       (RemoteClient*     self,
					       RemoteResponse*   response,
					       gpointer          user_data)
{
    HistogramImager *dest = HISTOGRAM_IMAGER(user_data);
    double elapsed;

    self->pending_stream_requests--;

    if (self->pending_param_changes) {
	/* This data is for an old parameter set, ignore it.
	 * FIXME: This doesn't distinguish between parameters that
	 *        actaully affect calculation and those that don't.
	 */
	return;
    }

    if (!response->data_length)
	return;

    histogram_imager_merge_stream(dest, response->data, response->data_length);

    /* Update our download speed */
    self->byte_accumulator += response->data_length;
    elapsed = g_timer_elapsed(self->stream_speed_timer, NULL);
    if (elapsed > MINIMUM_SPEED_WINDOW) {
	g_timer_start(self->stream_speed_timer);
	self->bytes_per_sec = self->byte_accumulator / elapsed;
	self->byte_accumulator = 0;
    }
}

static void    status_merge_callback          (RemoteClient*     self,
					       RemoteResponse*   response,
					       gpointer          user_data)
{
    IterativeMap *dest = ITERATIVE_MAP(user_data);
    double iters, iter_delta;
    long density;
    double elapsed;

    sscanf(response->message, "iterations=%lf density=%ld", &iters, &density);

    /* FIXME: Since we don't know which parameters affect calculation, we don't
     *        know when the node's iteration counter gets reset. We currently
     *        assume that if it's value decreases, it's been reset.
     */
    if (iters >= self->prev_iterations) {
	iter_delta = iters - self->prev_iterations;
    }
    else {
	/* Assume it started at zero */
	iter_delta = iters;
    }
    self->prev_iterations = iters;

    if (self->pending_param_changes)
	return;
    if (!iter_delta)
	return;

    /* Merge this iteration count in */
    dest->iterations += iter_delta;

    /* Update our iteration speed */
    self->iter_accumulator += iter_delta;
    elapsed = g_timer_elapsed(self->status_speed_timer, NULL);
    if (elapsed > MINIMUM_SPEED_WINDOW) {
	g_timer_start(self->status_speed_timer);
	self->iters_per_sec = self->iter_accumulator / elapsed;
	self->iter_accumulator = 0;
	if (self->speed_callback)
	    self->speed_callback(self, self->iters_per_sec, self->bytes_per_sec,
				 self->speed_callback_user_data);
    }
}

void           remote_client_merge_results    (RemoteClient*     self,
					       IterativeMap*     dest)
{
    double elapsed;

    /* Don't let our stream requests get too backed up */
    if (self->pending_stream_requests >= 4)
	return;

    /* Always keep the status updated */
    remote_client_command(self, status_merge_callback, dest,
			  "calc_status");

    /* Have we waited long enough since the last request? */
    elapsed = g_timer_elapsed(self->stream_request_timer, NULL);
    if (elapsed < self->min_stream_interval)
	return;
    g_timer_start(self->stream_request_timer);

    self->pending_stream_requests++;
    remote_client_command(self, histogram_merge_callback, dest,
			  "get_histogram_stream");
}

/* The End */
