/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Authors:
 *  David Trowbridge <trowbrds@cs.colorado.edu>
 *  Damon Chaplin <damon@ximian.com>
 *
 * Copyright (C) 2005 Novell, Inc. (www.novell.com)
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

#ifndef _CAL_PREFS_DIALOG_H_
#define _CAL_PREFS_DIALOG_H_

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <gconf/gconf-client.h>
#include "evolution-config-control.h"

G_BEGIN_DECLS

typedef struct _CalendarPrefsDialog CalendarPrefsDialog;
typedef struct _CalendarPrefsDialogClass CalendarPrefsDialogClass;

struct _CalendarPrefsDialog {
	GtkVBox parent;

	GladeXML *gui;

	GConfClient *gconf;

	GtkWidget *timezone;
	GtkWidget *working_days[7];
};

struct _CalendarPrefsDialogClass {
	GtkVBoxClass parent;
};

GType      calendar_prefs_dialog_get_type (void);
GtkWidget *calendar_prefs_dialog_new (void);

G_END_DECLS

#endif /* _CAL_PREFS_DIALOG_H_ */
