/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* 
 * Author : 
 *  Gary Ekker <gekker@novell.com>
 *
 * Copyright 2004, Ximian, Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of version 2 of the GNU General Public 
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/*
 * UrlEditorDialog - a GtkObject which handles a libglade-loaded dialog
 * to edit the calendar preference settings.
 */

#ifndef _URL_EDITOR_DIALOG_H_
#define _URL_EDITOR_DIALOG_H_

G_BEGIN_DECLS

#include <gtk/gtk.h>
#include <glade/glade.h>

#include "../e-pub-utils.h"
#include <libedataserverui/e-source-selector.h>

enum {
	URL_LIST_ENABLED_COLUMN,
	URL_LIST_LOCATION_COLUMN,
	URL_LIST_FREE_BUSY_URL_COLUMN,
	URL_LIST_N_COLUMNS
};

struct _DialogData {
	/* Glade XML data */
	GladeXML *xml;
	
	GConfClient *gconf;

	GtkWidget *page;

	GtkWidget *timezone;
	GtkWidget *working_days[7];
	GtkWidget *week_start_day;
	GtkWidget *start_of_day;
	GtkWidget *end_of_day;
	GtkWidget *use_12_hour;
	GtkWidget *use_24_hour;
	GtkWidget *time_divisions;
	GtkWidget *show_end_times;
	GtkWidget *compress_weekend;
	GtkWidget *dnav_show_week_no;

	/* Widgets for the task list options */
	GtkWidget *tasks_due_today_color;
	GtkWidget *tasks_overdue_color;

	GtkWidget *tasks_hide_completed_checkbutton;
	GtkWidget *tasks_hide_completed_spinbutton;
	GtkWidget *tasks_hide_completed_optionmenu;
	
	/* Widgets for the Free/Busy options */
	GtkWidget *url_add;
	GtkWidget *url_edit;
	GtkWidget *url_remove;
	GtkWidget *url_enable;
	GtkTreeView *url_list;
	gboolean url_editor;
	GtkWidget* url_editor_dlg;
	guint destroyed : 1;

	/* widgets for the Free/Busy template */
	GtkWidget *template_url;
		
	/* Other page options */
	GtkWidget *confirm_delete;
	GtkWidget *default_reminder;
	GtkWidget *default_reminder_interval;
	GtkWidget *default_reminder_units;
};
typedef struct _DialogData DialogData;

struct _UrlDialogData {
	/* Glade XML data */
	GladeXML *xml;
	GtkWidget *url_editor;
	GtkWidget *url_dialog;

	GtkEntry *url_entry;
	GtkWidget *daily;
	GtkWidget *weekly;
	GtkWidget *user_publish;
	
	GtkWidget *calendar_list_label;
	GtkWidget *scrolled_window;
	
	GtkEntry *username_entry;
	GtkEntry *password_entry;
	GtkWidget *remember_pw;
	
	GtkWidget *cancel;
	GtkWidget *ok;
	EPublishUri *url_data;
};
typedef struct _UrlDialogData UrlDialogData;

gboolean			
url_editor_dialog_new (DialogData *dialog_data, EPublishUri *pub_uri);

G_END_DECLS

#endif /* _URL_EDITOR_DIALOG_H_ */
