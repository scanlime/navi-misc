/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * remote-client.h - A client for communicating with remote Fyre
 *                   rendering servers.
 *
 *                   The lowest-level interface allows sending
 *                   commands, with responses triggering callback
 *                   functions. A higher level interface can attach
 *                   a remote rendering server to a local ParameterHolder
 *                   and HistogramImager.
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

#ifndef __REMOTE_CLIENT_H__
#define __REMOTE_CLIENT_H__

#include "platform.h"
#include <gtk/gtk.h>
#include <gnet.h>
#include "animation.h"
#include "iterative-map.h"
#include "remote-server.h"

G_BEGIN_DECLS

#define REMOTE_CLIENT_TYPE            (remote_client_get_type ())
#define REMOTE_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), REMOTE_CLIENT_TYPE, RemoteClient))
#define REMOTE_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), REMOTE_CLIENT_TYPE, RemoteClientClass))
#define IS_REMOTE_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), REMOTE_CLIENT_TYPE))
#define IS_REMOTE_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), REMOTE_CLIENT_TYPE))

typedef struct _RemoteClient         RemoteClient;
typedef struct _RemoteClientClass    RemoteClientClass;
typedef struct _RemoteResponse       RemoteResponse;
typedef struct _RemoteClosure        RemoteClosure;

typedef void   (*RemoteCallback)              (RemoteClient*     self,
					       RemoteResponse*   response,
					       gpointer          user_data);

typedef void   (*RemoteStatusCallback)        (RemoteClient*     self,
					       const gchar*      status,
					       gpointer          user_data);

typedef void   (*RemoteSpeedCallback)         (RemoteClient*     self,
					       double            iters_per_sec,
					       double            bytes_per_sec,
					       gpointer          user_data);

struct _RemoteClosure {
    RemoteCallback callback;
    gpointer       user_data;
};

struct _RemoteClient {
    GObject               object;

    double                min_stream_interval;
    double                retry_timeout;
    gboolean              is_retry_enabled;

    /* Private */

    const gchar*          host;
    gint                  port;
    GConn*                gconn;

    RemoteStatusCallback  status_callback;
    gpointer              status_callback_user_data;

    RemoteSpeedCallback   speed_callback;
    gpointer              speed_callback_user_data;

    gboolean              is_ready;
    guint                 retry_timer;
    int                   pending_param_changes;
    int                   pending_stream_requests;

    GTimer*               stream_request_timer;
    double                prev_iterations;
    GTimer*               status_speed_timer;
    GTimer*               stream_speed_timer;
    double                iter_accumulator;
    double                byte_accumulator;
    double                iters_per_sec;
    double                bytes_per_sec;

    GQueue*               response_queue;
    RemoteResponse*       current_binary_response;
};

struct _RemoteClientClass {
    GObjectClass parent_class;
};

struct _RemoteResponse {
    int      code;
    gchar*   message;
    guchar*  data;
    gsize    data_length;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType          remote_client_get_type         ();
RemoteClient*  remote_client_new              (const gchar*          hostname,
					       gint                  port);

void           remote_client_set_status_cb    (RemoteClient*         self,
					       RemoteStatusCallback  status_cb,
					       gpointer              user_data);
void           remote_client_set_speed_cb     (RemoteClient*         self,
					       RemoteSpeedCallback   speed_cb,
					       gpointer              user_data);

void           remote_client_connect          (RemoteClient*         self);
gboolean       remote_client_is_ready         (RemoteClient*         self);

/* Low-level interface */

void           remote_client_command          (RemoteClient*     self,
					       RemoteCallback    callback,
					       gpointer          user_data,
					       const gchar*      format,
					       ...);

/* High-level interface */

void           remote_client_send_param       (RemoteClient*     self,
					       ParameterHolder*  ph,
					       const gchar*      name);
void           remote_client_send_all_params  (RemoteClient*     self,
					       ParameterHolder*  ph);
void           remote_client_merge_results    (RemoteClient*     self,
					       IterativeMap*     dest);

G_END_DECLS

#endif /* __REMOTE_CLIENT_H__ */

/* The End */
