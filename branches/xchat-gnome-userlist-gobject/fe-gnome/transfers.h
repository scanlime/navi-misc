/*
 * transfers.h - dcc transfers window
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#ifndef XCHAT_GNOME_TRANSFERS_H
#define XCHAT_GNOME_TRANSFERS_H

#include "../common/xchat.h"
#include "../common/dcc.h"
#include <glade/glade.h>

typedef struct {
	GtkListStore *store;
	GladeXML *xml;
	struct DCC *selected;
} TransferGui;

extern TransferGui transfer_gui; 

void initialize_transfers_window();
void show_transfers_window();
void hide_transfers_window();
void add_transfer(struct DCC *dcc);
void update_transfer(struct DCC *dcc);
void remove_transfer(struct DCC *dcc);

#endif
