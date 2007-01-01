/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * discovery-client.h - This is a client that looks for DiscoveryServer
 *                      objects on remote machines. Requests can be sent out,
 *                      and a callback will be invoked when any servers
 *                      reply.
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

#ifndef __DISCOVERY_CLIENT_H__
#define __DISCOVERY_CLIENT_H__

#include "platform.h"
#include <gtk/gtk.h>
#include <gnet.h>

G_BEGIN_DECLS

#define DISCOVERY_CLIENT_TYPE            (discovery_client_get_type ())
#define DISCOVERY_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DISCOVERY_CLIENT_TYPE, DiscoveryClient))
#define DISCOVERY_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DISCOVERY_CLIENT_TYPE, DiscoveryClientClass))
#define IS_DISCOVERY_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DISCOVERY_CLIENT_TYPE))
#define IS_DISCOVERY_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DISCOVERY_CLIENT_TYPE))

typedef struct _DiscoveryClient         DiscoveryClient;
typedef struct _DiscoveryClientClass    DiscoveryClientClass;

typedef void (DiscoveryCallback)(DiscoveryClient* client,
				 const gchar*     host,
				 int              port,
				 gpointer         user_data);

struct _DiscoveryClient {
    GObject object;

    gchar*             service_name;
    DiscoveryCallback* callback;
    gpointer           user_data;
    guint              interval;

    guint              broadcast_timer;
    GInetAddr*         broadcast;
    GUdpSocket*        socket;
    guint              socket_reader;
    guchar*            buffer;
    gsize              buffer_size;
};

struct _DiscoveryClientClass {
    GObjectClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType             discovery_client_get_type   ();

/* Automatically look for "service_name". Call the provided callback when the
 * service is found. Send broadcast packets every 'interval' seconds.
 */
DiscoveryClient*  discovery_client_new        (const gchar*       service_name,
					       guint              interval,
					       DiscoveryCallback* callback,
					       gpointer           user_data);

G_END_DECLS

#endif /* __DISCOVERY_CLIENT_H__ */

/* The End */
