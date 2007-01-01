/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * discovery-server.h - This is a server that listens for UDP broadcast
 *                      packets searching for a particular service, then
 *                      responds if we provide that service.
 *
 *                      Services are identified by simple null-terminated
 *                      strings. Broadcasts come in on FYRE_DISCOVERY_PORT
 *                      as UDP packets containing this string. If it matches
 *                      our provided service, we send a UDP packet back with
 *                      the service name again and the port we run it on.
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

#ifndef __DISCOVERY_SERVER_H__
#define __DISCOVERY_SERVER_H__

#include "platform.h"
#include <gtk/gtk.h>
#include <gnet.h>

G_BEGIN_DECLS

#define FYRE_DISCOVERY_PORT         7932

#define DISCOVERY_SERVER_TYPE            (discovery_server_get_type ())
#define DISCOVERY_SERVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DISCOVERY_SERVER_TYPE, DiscoveryServer))
#define DISCOVERY_SERVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DISCOVERY_SERVER_TYPE, DiscoveryServerClass))
#define IS_DISCOVERY_SERVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DISCOVERY_SERVER_TYPE))
#define IS_DISCOVERY_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DISCOVERY_SERVER_TYPE))

typedef struct _DiscoveryServer         DiscoveryServer;
typedef struct _DiscoveryServerClass    DiscoveryServerClass;

struct _DiscoveryServer {
    GObject      object;

    gchar*       service_name;
    int          service_port;

    GUdpSocket*  socket;
    guchar*      buffer;
    gsize        buffer_size;
};

struct _DiscoveryServerClass {
    GObjectClass parent_class;
};

/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType             discovery_server_get_type         ();
DiscoveryServer*  discovery_server_new              (const gchar* service_name,
						     int          service_port);

G_END_DECLS

#endif /* __DISCOVERY_SERVER_H__ */

/* The End */
