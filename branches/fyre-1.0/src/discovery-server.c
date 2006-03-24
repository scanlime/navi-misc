/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * discovery-server.c - This is a server that listens for UDP broadcast
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

#include "discovery-server.h"
#include <string.h>

static void       discovery_server_class_init    (DiscoveryServerClass*  klass);
static void       discovery_server_init          (DiscoveryServer*       self);
static void       discovery_server_dispose       (GObject*               gobject);

static gboolean   discovery_server_read          (GIOChannel*            source,
						  GIOCondition           condition,
						  gpointer               user_data);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType discovery_server_get_type(void)
{
    static GType anim_type = 0;

    if (!anim_type) {
	static const GTypeInfo dj_info = {
	    sizeof(DiscoveryServerClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) discovery_server_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(DiscoveryServer),
	    0,
	    (GInstanceInitFunc) discovery_server_init,
	};

	anim_type = g_type_register_static(G_TYPE_OBJECT, "DiscoveryServer", &dj_info, 0);
    }

    return anim_type;
}

static void discovery_server_class_init(DiscoveryServerClass *klass)
{
    GObjectClass *object_class;
    object_class = (GObjectClass*) klass;

    object_class->dispose = discovery_server_dispose;
}

static void discovery_server_dispose(GObject *gobject)
{
    DiscoveryServer *self = DISCOVERY_SERVER(gobject);

    if (self->service_name) {
	g_free(self->service_name);
	self->service_name = NULL;
    }
    if (self->socket) {
	gnet_udp_socket_delete(self->socket);
	self->socket = NULL;
    }
    if (self->buffer) {
	g_free(self->buffer);
	self->buffer = NULL;
    }
}

static void discovery_server_init(DiscoveryServer *self)
{
    self->socket = gnet_udp_socket_new_with_port(FYRE_DISCOVERY_PORT);
}

DiscoveryServer*  discovery_server_new(const gchar* service_name, int service_port)
{
    DiscoveryServer *self = DISCOVERY_SERVER(g_object_new(discovery_server_get_type(), NULL));
    self->service_name = g_strdup(service_name);
    self->service_port = service_port;

    if (self->socket) {
	/* Create a buffer big enough to hold our incoming and outgoing packets */
	self->buffer_size = sizeof(service_name) + 16;
	self->buffer = g_malloc(self->buffer_size);

	/* Sign up to get notified when new packets arrive */
	g_io_add_watch(gnet_udp_socket_get_io_channel(self->socket),
		       G_IO_IN,  discovery_server_read, self);
    }
    else {
	printf("Warning, can't listen for UDP discovery packets on port %d\n",
	       FYRE_DISCOVERY_PORT);
    }

    return self;
}


/************************************************************************************/
/**************************************************************** Network Callbacks */
/************************************************************************************/

static gboolean discovery_server_read(GIOChannel*            source,
				      GIOCondition           condition,
				      gpointer               user_data)
{
    DiscoveryServer* self = DISCOVERY_SERVER(user_data);
    gint length;
    GInetAddr *src;

    /* Receive the packet waiting for us */
    length = gnet_udp_socket_receive(self->socket, self->buffer,
				     self->buffer_size, &src);
    self->buffer[self->buffer_size - 1] = '\0';

    /* Ignore it if it doesn't exactly match our service */
    if (length != strlen(self->service_name) + 1)
	return TRUE;
    if (strncmp(self->service_name, self->buffer, self->buffer_size))
	return TRUE;

    /* Yay, someone cares about us. Tack our port number
     * onto the end and send a reply.
     */
    self->buffer[length++] = self->service_port >> 8;
    self->buffer[length++] = self->service_port & 0xFF;

    gnet_udp_socket_send(self->socket, self->buffer, length, src);

    return TRUE;
}

/* The End */
