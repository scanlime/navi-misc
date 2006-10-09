/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * discovery-client.c - This is a client that looks for DiscoveryServer
 *                      objects on remote machines. Requests can be sent out,
 *                      and a callback will be invoked when any servers
 *                      reply.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#include <string.h>
#include "discovery-server.h"
#include "discovery-client.h"

static void       discovery_client_class_init    (DiscoveryClientClass*  klass);
static void       discovery_client_init          (DiscoveryClient*       self);
static void       discovery_client_dispose       (GObject*               gobject);

static gboolean   discovery_client_read          (GIOChannel*            source,
						  GIOCondition           condition,
						  gpointer               user_data);

static gboolean   discovery_client_broadcast     (gpointer               user_data);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType discovery_client_get_type(void)
{
    static GType anim_type = 0;

    if (!anim_type) {
	static const GTypeInfo dj_info = {
	    sizeof(DiscoveryClientClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) discovery_client_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(DiscoveryClient),
	    0,
	    (GInstanceInitFunc) discovery_client_init,
	};

	anim_type = g_type_register_static(G_TYPE_OBJECT, "DiscoveryClient", &dj_info, 0);
    }

    return anim_type;
}

static void discovery_client_class_init(DiscoveryClientClass *klass)
{
    GObjectClass *object_class;
    object_class = (GObjectClass*) klass;

    object_class->dispose = discovery_client_dispose;
}

static void discovery_client_dispose(GObject *gobject)
{
    DiscoveryClient *self = DISCOVERY_CLIENT(gobject);

    if (self->broadcast_timer) {
	g_source_remove(self->broadcast_timer);
	self->broadcast_timer = 0;
    }
    if (self->socket_reader) {
	g_source_remove(self->socket_reader);
	self->socket_reader = 0;
    }
    if (self->socket) {
	gnet_udp_socket_delete(self->socket);
	self->socket = NULL;
    }
    if (self->service_name) {
	g_free(self->service_name);
	self->service_name = NULL;
    }
    if (self->buffer) {
	g_free(self->buffer);
	self->buffer = NULL;
    }
    if (self->broadcast) {
	gnet_inetaddr_delete(self->broadcast);
	self->broadcast = NULL;
    }
}

static void discovery_client_init(DiscoveryClient *self)
{
    self->socket = gnet_udp_socket_new();
    self->broadcast = gnet_inetaddr_new("255.255.255.255", FYRE_DISCOVERY_PORT);
}

DiscoveryClient*  discovery_client_new(const gchar*       service_name,
				       guint              interval,
				       DiscoveryCallback* callback,
				       gpointer           user_data)
{
    DiscoveryClient *self = DISCOVERY_CLIENT(g_object_new(discovery_client_get_type(), NULL));
    self->service_name = g_strdup(service_name);
    self->interval = interval;
    self->callback = callback;
    self->user_data = user_data;

    /* Create a buffer big enough to hold our incoming and outgoing packets */
    self->buffer_size = sizeof(service_name) + 16;
    self->buffer = g_malloc(self->buffer_size);

    /* Sign up to get notified when new packets arrive */
    self->socket_reader = g_io_add_watch(gnet_udp_socket_get_io_channel(self->socket),
					 G_IO_IN,  discovery_client_read, self);

    /* Send the first broadcast */
    discovery_client_broadcast(self);

    /* Schedule regular retries */
    self->broadcast_timer = g_timeout_add(self->interval * 1000,
					  discovery_client_broadcast,
					  self);
    return self;
}


/************************************************************************************/
/**************************************************************** Network Callbacks */
/************************************************************************************/

static gboolean discovery_client_read(GIOChannel*            source,
				      GIOCondition           condition,
				      gpointer               user_data)
{
    DiscoveryClient* self = DISCOVERY_CLIENT(user_data);
    gint length;
    GInetAddr *src;
    gint port;
    const gchar* host;

    /* Receive the packet waiting for us */
    length = gnet_udp_socket_receive(self->socket, self->buffer,
				     self->buffer_size, &src);
    self->buffer[self->buffer_size - 1] = '\0';

    /* Ignore it if it doesn't exactly match our service.
     * It will have a 16-bit port number after the service name,
     * we ignore that at this point.
     */
    if (length != strlen(self->service_name) + 3)
	return TRUE;
    if (strncmp(self->service_name, self->buffer, self->buffer_size))
	return TRUE;

    /* Yay, a service responded. Extract the host and port, and
     * invoke our owner's callback function.
     */
    port = self->buffer[length-1] | (self->buffer[length-2] << 8);
    host = gnet_inetaddr_get_canonical_name(src);

    self->callback(self, host, port, self->user_data);

    return TRUE;
}

static gboolean discovery_client_broadcast(gpointer user_data)
{
    DiscoveryClient* self = DISCOVERY_CLIENT(user_data);

    /* Send a broadcast packet with our service name */
    gnet_udp_socket_send(self->socket, self->service_name,
			 strlen(self->service_name)+1, self->broadcast);

    return TRUE;
}

/* The End */
