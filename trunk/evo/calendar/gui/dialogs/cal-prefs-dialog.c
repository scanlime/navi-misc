/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Authors:
 *  David Trowbridge <trowbrds@cs.colorado.edu>
 *  Damon Chaplin <damon@ximian.com>
 *  Ettore Perazzoli <ettore@ximian.com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../e-cal-config.h"
#include "../e-timezone-entry.h"
#include "cal-prefs-dialog.h"
#include <widgets/misc/e-dateedit.h>

static GtkVBoxClass *parent_class = NULL;

static void
calendar_prefs_dialog_finalize (GObject *obj)
{
	CalendarPrefsDialog *prefs = (CalendarPrefsDialog *) obj;

	g_object_unref (prefs->gui);

	((GObjectClass *)(parent_class))->finalize (obj);
}

static void
calendar_prefs_dialog_class_init (CalendarPrefsDialogClass *klass)
{
	GObjectClass *object_class;

	object_class = (GObjectClass *) klass;
	parent_class = g_type_class_ref (GTK_TYPE_VBOX);

	object_class->finalize = calendar_prefs_dialog_finalize;
}

static void
calendar_prefs_dialog_init (CalendarPrefsDialog *dialog)
{
}

static GtkWidget *
eccp_widget_glade (EConfig *ec, EConfigItem *item, GtkWidget *parent, GtkWidget *old, void *data)
{
	CalendarPrefsDialog *prefs = data;

	return glade_xml_get_widget (prefs->gui, item->label);
}

/* plugin meta-data */
static ECalConfigItem eccp_items[] = {
	{ E_CONFIG_BOOK,          "",                             "toplevel-notebook",   eccp_widget_glade },
	{ E_CONFIG_PAGE,          "00.general",                   "general",             eccp_widget_glade },
	{ E_CONFIG_SECTION_TABLE, "00.general/00.time",           "time",                eccp_widget_glade },
	{ E_CONFIG_SECTION_TABLE, "00.general/10.workWeek",       "workWeek",            eccp_widget_glade },
	{ E_CONFIG_SECTION,       "00.general/20.alerts",         "alerts",              eccp_widget_glade },
	{ E_CONFIG_PAGE,          "10.display",                   "display",             eccp_widget_glade },
	{ E_CONFIG_SECTION,       "10.display/00.general",        "displayGeneral",      eccp_widget_glade },
	{ E_CONFIG_SECTION,       "10.display/10.taskList",       "taskList",            eccp_widget_glade },
	{ E_CONFIG_PAGE,          "20.freeBusy",                  "freebusy",            eccp_widget_glade },
	{ E_CONFIG_SECTION,       "20.freeBusy/00.publishing",    "publishing",          eccp_widget_glade },
	{ E_CONFIG_SECTION,       "20.freeBusy/10.defaultServer", "defaultFBServer",     eccp_widget_glade },
};

static void
eccp_free (EConfig *ec, GSList *items, void *data)
{
	g_slist_free (items);
}

static void
calendar_prefs_dialog_construct (CalendarPrefsDialog *prefs)
{
	GladeXML *gui;
	ECalConfig *ec;
	ECalConfigTargetPrefs *target;
	int i;
	GtkWidget *toplevel;
	GSList *l;

	gui = glade_xml_new (EVOLUTION_GLADEDIR "/cal-prefs-dialog.glade", "toplevel-notebook", NULL);
	prefs->gui = gui;

	prefs->gconf = gconf_client_get_default ();

	/** @HookPoint-ECalConfig: Calendar Preferences Page
	 * @Id: org.gnome.evolution.calendar.prefs
	 * @Type: E_CONFIG_BOOK
	 * @Class: org.gnome.evolution.calendar.config:1.0
	 * @Target: ECalConfigTargetPrefs
	 *
	 * The mail calendar preferences page
	 */
	ec = e_cal_config_new (E_CONFIG_BOOK, "org.gnome.evolution.calendar.prefs");
	l = NULL;
	for (i = 0; i < G_N_ELEMENTS (eccp_items); i++)
		l = g_slist_prepend (l, &eccp_items[i]);
	e_config_add_items ((EConfig *) ec, l, NULL, NULL, eccp_free, prefs);

	target = e_cal_config_target_new_prefs (ec, prefs->gconf);
	e_config_set_target ((EConfig *)ec, (EConfigTarget *) target);
	toplevel = e_config_create_widget ((EConfig *)ec);
	gtk_container_add (GTK_CONTAINER (prefs), toplevel);
}

GType
calendar_prefs_dialog_get_type (void)
{
	static GType type = 0;

	if (!type) {
		static GTypeInfo type_info = {
			sizeof (CalendarPrefsDialogClass),
			NULL, NULL,
			(GClassInitFunc) calendar_prefs_dialog_class_init,
			NULL, NULL,
			sizeof (CalendarPrefsDialog),
			0,
			(GInstanceInitFunc) calendar_prefs_dialog_init,
		};

		type = g_type_register_static (GTK_TYPE_VBOX, "CalendarPrefsDialog", &type_info, 0);
	}

	return type;
}

GtkWidget *
calendar_prefs_dialog_new (void)
{
	CalendarPrefsDialog *dialog;

	dialog = (CalendarPrefsDialog *) g_object_new (calendar_prefs_dialog_get_type (), NULL);
	calendar_prefs_dialog_construct (dialog);

	return (GtkWidget *) dialog;
}

/* called by libglade to create our custom EDateEdit widgets. */
GtkWidget *
cal_prefs_dialog_create_time_edit (void)
{
	GtkWidget *dedit;

	dedit = e_date_edit_new ();

	e_date_edit_set_use_24_hour_format (E_DATE_EDIT (dedit), calendar_config_get_24_hour_format ());
	e_date_edit_set_time_popup_range (E_DATE_EDIT (dedit), 0, 24);
	e_date_edit_set_show_date (E_DATE_EDIT (dedit), FALSE);

	return dedit;
}
