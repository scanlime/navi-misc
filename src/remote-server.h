/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * remote-server.h - Remote control mode, an interface for automating
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

#ifndef __REMOTE_SERVER_H__
#define __REMOTE_SERVER_H__

#include "platform.h"
#include <gtk/gtk.h>
#include "animation.h"
#include "iterative-map.h"

G_BEGIN_DECLS

/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

void              remote_server_main_loop     (int        port_number,
					       gboolean   have_gtk,
					       gboolean   verbose);


/************************************************************************************/
/******************************************************************* Protocol *******/
/************************************************************************************/

#define FYRE_DEFAULT_PORT           7931
#define FYRE_DEFAULT_SERVICE        "Fyre Server 1"


#define FYRE_RESPONSE_READY         220  /* Server is ready for commands */
#define FYRE_RESPONSE_OK            250  /* Successfully executed the last command */
#define FYRE_RESPONSE_PROGRESS      251  /* Progress in calculation */
#define FYRE_RESPONSE_FALSE         252  /* Command succeeded, but the result was negative */
#define FYRE_RESPONSE_BINARY        380  /* Length, in bytes, is the first token of
					  * the response message. Binary data follows
					  * after the message's newline.
					  */
#define FYRE_RESPONSE_UNRECOGNIZED  500  /* Command not recognized */
#define FYRE_RESPONSE_BAD_VALUE     501  /* Inappropriate parameter value */
#define FYRE_RESPONSE_UNSUPPORTED   502  /* Command was recognized, but is not currently supported */

G_END_DECLS

#endif /* __REMOTE_SERVER_H__ */

/* The End */
