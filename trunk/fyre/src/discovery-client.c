/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * discovery-client.c - This is a client that looks for DiscoveryServer
 *                      objects on remote machines. Requests can be sent out,
 *                      and a callback will be invoked when any servers
 *                      reply.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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

#include "discovery-client.h"

static void       discovery_client_class_init    (DiscoveryClientClass*    klass);
static void       discovery_client_init          (DiscoveryClient*         self);
static void       discovery_client_dispose       (GObject*              gobject);


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

    if (self->service_name) {
	g_free(self->service_name);
	self->service_name = NULL;
    }
}

static void discovery_client_init(DiscoveryClient *self)
{

}

DiscoveryClient*  discovery_client_new(const gchar* service_name)
{
    DiscoveryClient *self = DISCOVERY_CLIENT(g_object_new(discovery_client_get_type(), NULL));
    self->service_name = g_strdup(service_name);

    return self;
}


/************************************************************************************/
/**************************************************************** Network Callbacks */
/************************************************************************************/

/* The End */
