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
#include "../e-pub-utils.h"
#include "../calendar-config.h"
#include "cal-prefs-dialog.h"
#include "url-editor-dialog.h"
#include <widgets/misc/e-dateedit.h>
#include <e-util/e-dialog-widgets.h>
#include <libgnome/gnome-i18n.h>
#include <string.h>

static const int week_start_day_map[] = {
	1, 2, 3, 4, 5, 6, 0, -1
};

static const int time_division_map[] = {
	60, 30, 15, 10, 5, -1
};

/* The following two are kept separate in case we need to re-order each menu individually */
static const int hide_completed_units_map[] = {
	CAL_MINUTES, CAL_HOURS, CAL_DAYS, -1
};

static const int default_reminder_units_map[] = {
	CAL_MINUTES, CAL_HOURS, CAL_DAYS, -1
};

static GtkVBoxClass *parent_class = NULL;

GtkWidget *cal_prefs_dialog_create_time_edit (void);



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

/* Returns a pointer to a static string with an X color spec for the current
 * value of a color picker.
 */
static const char *
spec_from_picker (GtkWidget *picker)
{
	static char spec[8];
	GdkColor color;

	gtk_color_button_get_color (GTK_COLOR_BUTTON (picker), &color);
	g_snprintf (spec, sizeof (spec), "#%02x%02x%02x", color.red, color.green, color.blue);

	return spec;
}

static void
working_days_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	CalWeekdays working_days = 0;
	guint32 mask = 1;
	int day;

	for (day = 0; day < 7; day++) {
		if (e_dialog_toggle_get (prefs->working_days[day]))
			working_days |= mask;
		mask <<= 1;
	}

	calendar_config_set_working_days (working_days);
}

static void
timezone_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	icaltimezone *zone;

	zone = e_timezone_entry_get_timezone (E_TIMEZONE_ENTRY (prefs->timezone));
	calendar_config_set_timezone (icaltimezone_get_location (zone));
}

static void
start_of_day_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	int start_hour, start_minute, end_hour, end_minute;
	EDateEdit *start, *end;

	start = E_DATE_EDIT (prefs->start_of_day);
	end = E_DATE_EDIT (prefs->end_of_day);

	e_date_edit_get_time_of_day (start, &start_hour, &start_minute);
	e_date_edit_get_time_of_day (end, &end_hour, &end_minute);

	if ((start_hour > end_hour) || (start_hour == end_hour && start_minute > end_minute)) {
		if (start_hour < 23)
			e_date_edit_set_time_of_day (end, start_hour + 1, start_minute);
		else
			e_date_edit_set_time_of_day (end, 23, 59);

		return;
	}

	calendar_config_set_day_start_hour (start_hour);
	calendar_config_set_day_start_minute (start_minute);
}

static void
end_of_day_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	int start_hour, start_minute, end_hour, end_minute;
	EDateEdit *start, *end;

	start = E_DATE_EDIT (prefs->start_of_day);
	end = E_DATE_EDIT (prefs->end_of_day);

	e_date_edit_get_time_of_day (start, &start_hour, &start_minute);
	e_date_edit_get_time_of_day (end, &end_hour, &end_minute);

	if ((end_hour < start_hour) || (end_hour == start_hour && end_minute < start_minute)) {
		if (end_hour < 1)
			e_date_edit_set_time_of_day (start, 0, 0);
		else
			e_date_edit_set_time_of_day (start, end_hour - 1, end_minute);

		return;
	}

	calendar_config_set_day_end_hour (end_hour);
	calendar_config_set_day_end_minute (end_minute);
}
static void
week_start_day_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	int week_start_day;

	week_start_day = e_dialog_option_menu_get (prefs->week_start_day, week_start_day_map);
	calendar_config_set_week_start_day (week_start_day);
}

static void
use_24_hour_toggled (GtkToggleButton *toggle, CalendarPrefsDialog *prefs)
{
	gboolean use_24_hour;

	use_24_hour = gtk_toggle_button_get_active (toggle);

	e_date_edit_set_use_24_hour_format (E_DATE_EDIT (prefs->start_of_day), use_24_hour);
	e_date_edit_set_use_24_hour_format (E_DATE_EDIT (prefs->end_of_day), use_24_hour);

	calendar_config_set_24_hour_format (use_24_hour);
}

static void
time_divisions_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	int time_divisions;

	time_divisions = e_dialog_option_menu_get (prefs->time_divisions, time_division_map);
	calendar_config_set_time_divisions (time_divisions);
}

static void
show_end_times_toggled (GtkToggleButton *toggle, CalendarPrefsDialog *prefs)
{
	calendar_config_set_show_event_end (gtk_toggle_button_get_active (toggle));
}

static void
compress_weekend_toggled (GtkToggleButton *toggle, CalendarPrefsDialog *prefs)
{
	calendar_config_set_compress_weekend (gtk_toggle_button_get_active (toggle));
}

static void
dnav_show_week_no_toggled (GtkToggleButton *toggle, CalendarPrefsDialog *prefs)
{
	calendar_config_set_dnav_show_week_no (gtk_toggle_button_get_active (toggle));
}

static void
hide_completed_tasks_toggled (GtkToggleButton *toggle, CalendarPrefsDialog *prefs)
{
	gboolean hide;

	hide = gtk_toggle_button_get_active (toggle);

	gtk_widget_set_sensitive (prefs->tasks_hide_completed_interval, hide);
	gtk_widget_set_sensitive (prefs->tasks_hide_completed_units, hide);

	calendar_config_set_hide_completed_tasks (hide);
}

static void
hide_completed_tasks_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	calendar_config_set_hide_completed_tasks_value (e_dialog_spin_get_int (prefs->tasks_hide_completed_interval));
}

static void
hide_completed_tasks_units_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	calendar_config_set_hide_completed_tasks_units (
		e_dialog_combo_box_get (prefs->tasks_hide_completed_units, hide_completed_units_map));
}

static void
tasks_due_today_set_color (GtkColorButton *picker, guint r, guint g, guint b, guint a, CalendarPrefsDialog *prefs)
{
	calendar_config_set_tasks_due_today_color (spec_from_picker (prefs->tasks_due_today_color));
}

static void
tasks_overdue_set_color (GtkColorButton *picker, guint r, guint g, guint b, guint a, CalendarPrefsDialog *prefs)
{
	calendar_config_set_tasks_overdue_color (spec_from_picker (prefs->tasks_overdue_color));
}

static void
confirm_delete_toggled (GtkToggleButton *toggle, CalendarPrefsDialog *prefs)
{
	calendar_config_set_confirm_delete (gtk_toggle_button_get_active (toggle));
}

static void
default_reminder_toggled (GtkToggleButton *toggle, CalendarPrefsDialog *prefs)
{
	calendar_config_set_use_default_reminder (gtk_toggle_button_get_active (toggle));
}

static void
default_reminder_interval_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	calendar_config_set_default_reminder_interval (
		e_dialog_spin_get_int (prefs->default_reminder_interval));
}

static void
default_reminder_units_changed (GtkWidget *widget, CalendarPrefsDialog *prefs)
{
	calendar_config_set_default_reminder_units (
		e_dialog_option_menu_get (prefs->default_reminder_units, default_reminder_units_map));
}

static void
template_url_changed (GtkEntry *entry, CalendarPrefsDialog *prefs)
{
	calendar_config_set_free_busy_template (gtk_entry_get_text (entry));
}

static void
setup_changes (CalendarPrefsDialog *prefs)
{
	int i;

	for (i = 0; i < 7; i ++)
		g_signal_connect (G_OBJECT (prefs->working_days[i]), "toggled", G_CALLBACK (working_days_changed), prefs);

	g_signal_connect (G_OBJECT (prefs->timezone), "changed", G_CALLBACK (timezone_changed), prefs);

	g_signal_connect (G_OBJECT (prefs->start_of_day), "changed", G_CALLBACK (start_of_day_changed), prefs);
	g_signal_connect (G_OBJECT (prefs->end_of_day), "changed", G_CALLBACK (end_of_day_changed), prefs);

	g_signal_connect (G_OBJECT (prefs->week_start_day), "changed", G_CALLBACK (week_start_day_changed), prefs);

	g_signal_connect (G_OBJECT (prefs->use_24_hour), "toggled", G_CALLBACK (use_24_hour_toggled), prefs);

	g_signal_connect (G_OBJECT (prefs->time_divisions), "changed", G_CALLBACK (time_divisions_changed), prefs);

	g_signal_connect (G_OBJECT (prefs->show_end_times), "toggled", G_CALLBACK (show_end_times_toggled), prefs);
	g_signal_connect (G_OBJECT (prefs->compress_weekend), "toggled", G_CALLBACK (compress_weekend_toggled), prefs);
	g_signal_connect (G_OBJECT (prefs->dnav_show_week_no), "toggled", G_CALLBACK (dnav_show_week_no_toggled), prefs);

	g_signal_connect (G_OBJECT (prefs->tasks_hide_completed), "toggled",
			  G_CALLBACK (hide_completed_tasks_toggled), prefs);
	g_signal_connect (G_OBJECT (prefs->tasks_hide_completed_interval), "value-changed",
			  G_CALLBACK (hide_completed_tasks_changed), prefs);
	g_signal_connect (G_OBJECT (prefs->tasks_hide_completed_units), "changed", G_CALLBACK (hide_completed_tasks_units_changed), prefs);
	g_signal_connect (G_OBJECT (prefs->tasks_due_today_color), "color-set",
			  G_CALLBACK (tasks_due_today_set_color), prefs);
	g_signal_connect (G_OBJECT (prefs->tasks_overdue_color), "color-set",
			  G_CALLBACK (tasks_overdue_set_color), prefs);

	g_signal_connect (G_OBJECT (prefs->confirm_delete), "toggled", G_CALLBACK (confirm_delete_toggled), prefs);
	g_signal_connect (G_OBJECT (prefs->default_reminder), "toggled", G_CALLBACK (default_reminder_toggled), prefs);
	g_signal_connect (G_OBJECT (prefs->default_reminder_interval), "changed",
			  G_CALLBACK (default_reminder_interval_changed), prefs);
	g_signal_connect (G_OBJECT (prefs->default_reminder_units), "changed", G_CALLBACK (default_reminder_units_changed), prefs);

	g_signal_connect (G_OBJECT (prefs->template_url), "changed", G_CALLBACK (template_url_changed), prefs);
}

/* Sets the color in a color picker from an X color spec */
static void
set_color_picker (GtkWidget *picker, const char *spec)
{
	GdkColor color;

	if (!spec || !gdk_color_parse (spec, &color))
		color.red = color.green = color.blue = 0;

	gtk_color_button_set_color (GTK_COLOR_BUTTON (picker), &color);
}

static void
url_list_changed (CalendarPrefsDialog *prefs)
{
	GtkTreeModel *model = NULL;
	GSList *url_list = NULL;
	GtkTreeIter iter;
	gboolean valid;

	url_list = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (prefs->url_list));

	valid = gtk_tree_model_get_iter_first (model, &iter);
	while (valid) {
		EPublishUri *url;
		char *xml;

		gtk_tree_model_get (model, &iter, URL_LIST_FREE_BUSY_URL_COLUMN, &url, -1);

		if ((xml = e_pub_uri_to_xml (url)))
			url_list = g_slist_append (url_list, xml);

		valid = gtk_tree_model_iter_next (model, &iter);
	}

	calendar_config_set_free_busy (url_list);

	g_slist_free (url_list);
}

static void
url_list_enable_toggled (GtkCellRendererToggle *renderer,
		         const char            *path_string,
			 CalendarPrefsDialog   *prefs)
{
	GtkTreeSelection * selection;
	EPublishUri *url = NULL;
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeIter iter;

	path = gtk_tree_path_new_from_string (path_string);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (prefs->url_list));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (prefs->url_list));

	if (gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_model_get (model, &iter, URL_LIST_FREE_BUSY_URL_COLUMN, &url, -1);

		url->enabled = !url->enabled;
		gtk_list_store_set(GTK_LIST_STORE (model), &iter, URL_LIST_ENABLED_COLUMN, url->enabled, -1);

		if (gtk_tree_selection_iter_is_selected (selection, &iter)) {
			gtk_label_set_text (GTK_LABEL (prefs->url_enable_label),
					    url->enabled ? _("Disable") : _("Enable"));
			gtk_image_set_from_stock (GTK_IMAGE (prefs->url_enable_image),
					          url->enabled ? GTK_STOCK_YES : GTK_STOCK_NO,
						  GTK_ICON_SIZE_BUTTON);
		}

		url_list_changed (prefs);
	}

	gtk_tree_path_free (path);
}

static void
url_edit_clicked (GtkWidget *button, CalendarPrefsDialog *prefs)
{
	GtkTreeSelection *selection;
	EPublishUri *url = NULL;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkWidget *url_editor;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (prefs->url_list));
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
		gtk_tree_model_get (model, &iter, URL_LIST_FREE_BUSY_URL_COLUMN, &url, -1);

	if (url) {
		url_editor = url_editor_dialog_new (model, url);
		url_editor_dialog_run ((UrlEditorDialog *) url_editor);

		gtk_list_store_set (GTK_LIST_STORE (model), &iter, URL_LIST_LOCATION_COLUMN,
				    g_strdup (url->location), URL_LIST_ENABLED_COLUMN,
				    url->enabled, URL_LIST_FREE_BUSY_URL_COLUMN, url, -1);
		url_list_changed (prefs);
		if (!GTK_WIDGET_SENSITIVE (prefs->url_remove)) {
			selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (prefs->url_list));
			gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), &iter);
			gtk_widget_set_sensitive (prefs->url_remove, TRUE);
			gtk_tree_selection_select_iter (selection, &iter);
		}
		gtk_widget_destroy (url_editor);
	}
}

static void
url_list_double_click (GtkTreeView *treeview,
		       GtkTreePath *path,
		       GtkTreeViewColumn *column,
		       CalendarPrefsDialog *prefs)
{
	url_edit_clicked  (NULL, prefs);
}

static void
url_add_clicked (GtkWidget *button, CalendarPrefsDialog *prefs)
{
	EPublishUri *url = NULL;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkWidget *url_editor;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (prefs->url_list));
	url = g_new0 (EPublishUri, 1);
	url->enabled = TRUE;
	url->location = "";

	url_editor = url_editor_dialog_new (model, url);
	url_editor_dialog_run ((UrlEditorDialog *) url_editor);
	if (strlen (url->location)) {
		gtk_list_store_append(GTK_LIST_STORE (model), &iter);
		gtk_list_store_set (GTK_LIST_STORE(model), &iter, URL_LIST_ENABLED_COLUMN,
				    url->enabled, URL_LIST_LOCATION_COLUMN, g_strdup (url->location),
				    URL_LIST_FREE_BUSY_URL_COLUMN, url, -1);

		url_list_changed (prefs);
		if (!GTK_WIDGET_SENSITIVE (prefs->url_remove)) {
			selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (prefs->url_list));
			gtk_tree_model_get_iter_first (model, &iter);
			gtk_widget_set_sensitive (prefs->url_remove, TRUE);
			gtk_tree_selection_select_iter (selection, &iter);
		}
	}
	gtk_widget_destroy (url_editor);
}

static void
url_remove_clicked (GtkWidget *button, CalendarPrefsDialog *prefs)
{
	EPublishUri *url = NULL;
	GtkTreeSelection * selection;
	GtkTreeModel *model;
	GtkWidget *confirm;
	GtkTreeIter iter;
	int ans;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (prefs->url_list));
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
		gtk_tree_model_get (model, &iter, URL_LIST_FREE_BUSY_URL_COLUMN, &url, -1);

	confirm = gtk_message_dialog_new (GTK_WINDOW (prefs),
					  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					  GTK_MESSAGE_QUESTION,
					  GTK_BUTTONS_NONE,
					  _("Are you sure you want to remove this URL?"));

	button = gtk_button_new_from_stock (GTK_STOCK_YES);
	gtk_button_set_label (GTK_BUTTON (button), _("Remove"));
	gtk_dialog_add_action_widget (GTK_DIALOG (confirm), button, GTK_RESPONSE_YES);
	gtk_widget_show (button);

	button = gtk_button_new_from_stock (GTK_STOCK_NO);
	gtk_button_set_label (GTK_BUTTON (button), _("Don't Remove"));
	gtk_dialog_add_action_widget (GTK_DIALOG (confirm), button, GTK_RESPONSE_NO);

	gtk_widget_show (button);

	ans = gtk_dialog_run (GTK_DIALOG (confirm));
	gtk_widget_destroy (confirm);

	if (ans == GTK_RESPONSE_YES) {
		int len;

		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);

		len = gtk_tree_model_iter_n_children (model, NULL);
		if (len > 0) {
			gtk_tree_selection_select_iter (selection, &iter);
		} else {
			gtk_widget_set_sensitive (GTK_WIDGET (prefs->url_edit), FALSE);
			gtk_widget_set_sensitive (GTK_WIDGET (prefs->url_remove), FALSE);
			gtk_widget_set_sensitive (GTK_WIDGET (prefs->url_enable), FALSE);
		}
		g_free (url);
		url_list_changed (prefs);
	}
}

static void
url_enable_clicked (GtkWidget *button, CalendarPrefsDialog *prefs)
{
	EPublishUri *url = NULL;
	GtkTreeSelection * selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (prefs->url_list));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, URL_LIST_FREE_BUSY_URL_COLUMN, &url, -1);
		url->enabled = !url->enabled;
		gtk_tree_selection_select_iter (selection, &iter);
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, URL_LIST_ENABLED_COLUMN, url->enabled, -1);
		gtk_label_set_text (GTK_LABEL (prefs->url_enable_label),
				    url->enabled ? _("Disable") : _("Enable"));
		gtk_image_set_from_stock (GTK_IMAGE (prefs->url_enable_image),
				          url->enabled ? GTK_STOCK_YES : GTK_STOCK_NO,
					  GTK_ICON_SIZE_BUTTON);
		url_list_changed (prefs);
	}
}

/* Shows the current Free/Busy settings in the dialog */
static void
show_fb_config (CalendarPrefsDialog *prefs)
{
	GSList *url_config_list = NULL;
	GtkListStore *model;
	GtkTreeIter iter;
	gchar *template_url;

	model = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (prefs->url_list)));
	gtk_list_store_clear (model);

	/* restore urls from gconf */
	url_config_list = calendar_config_get_free_busy();

	if (!url_config_list) {
		/* list is empty-disable edit, remove, and enable buttons */
		gtk_widget_set_sensitive (prefs->url_edit, FALSE);
		gtk_widget_set_sensitive (prefs->url_remove, FALSE);
		gtk_widget_set_sensitive (prefs->url_enable, FALSE);
	}	else {
		gtk_widget_set_sensitive (prefs->url_edit, TRUE);
		gtk_widget_set_sensitive (prefs->url_remove, TRUE);
		gtk_widget_set_sensitive (prefs->url_enable, TRUE);
	}

	while (url_config_list) {
		gchar *xml = (gchar *)url_config_list->data;
		EPublishUri *url;
		url = g_new0 (EPublishUri, 1);

		e_pub_uri_from_xml (url, xml);
		if (url->location) {
			gtk_list_store_append (model, &iter);
			gtk_list_store_set (model, &iter, URL_LIST_ENABLED_COLUMN, url->enabled,
					   URL_LIST_LOCATION_COLUMN, url->location,
					   URL_LIST_FREE_BUSY_URL_COLUMN, url, -1);
		}

		url_config_list = g_slist_next (url_config_list);
		g_free (xml);
	}

	g_slist_foreach (url_config_list, (GFunc) g_free, NULL);
	g_slist_free (url_config_list);

	template_url = calendar_config_get_free_busy_template ();
	gtk_entry_set_text (GTK_ENTRY (prefs->template_url), template_url);

	g_free (template_url);
}

/* Shows the current task list settings in the dialog */
static void
show_task_list_config (CalendarPrefsDialog *prefs)
{
	CalUnits units;
	gboolean hide_completed_tasks;

	set_color_picker (prefs->tasks_due_today_color, calendar_config_get_tasks_due_today_color ());
	set_color_picker (prefs->tasks_overdue_color, calendar_config_get_tasks_overdue_color ());

	/* Hide Completed Tasks. */
	e_dialog_toggle_set (prefs->tasks_hide_completed, calendar_config_get_hide_completed_tasks ());

	/* Hide Completed Tasks Units. */
	units = calendar_config_get_hide_completed_tasks_units ();
	e_dialog_combo_box_set (prefs->tasks_hide_completed_units, units, hide_completed_units_map);

	/* Hide Completed Tasks Value. */
	e_dialog_spin_set (prefs->tasks_hide_completed_interval, calendar_config_get_hide_completed_tasks_value ());

	gtk_widget_set_sensitive (prefs->tasks_hide_completed_interval, hide_completed_tasks);
	gtk_widget_set_sensitive (prefs->tasks_hide_completed_units, hide_completed_tasks);
}

/* Shows the current config settings in the dialog. */
static void
show_config (CalendarPrefsDialog *prefs)
{
	CalWeekdays working_days;
	gint mask, day, week_start_day, time_divisions;
	icaltimezone *zone;
	gboolean sensitive;

	/* Timezone. */
	zone = calendar_config_get_icaltimezone ();
	e_timezone_entry_set_timezone (E_TIMEZONE_ENTRY (prefs->timezone), zone);

	/* Working Days. */
	working_days = calendar_config_get_working_days ();
	mask = 1 << 0;
	for (day = 0; day < 7; day++) {
		e_dialog_toggle_set (prefs->working_days[day], (working_days & mask) ? TRUE : FALSE);
		mask <<= 1;
	}

	/* Week Start Day. */
	week_start_day = calendar_config_get_week_start_day ();
	e_dialog_combo_box_set (prefs->week_start_day, week_start_day, week_start_day_map);

	/* Start of Day. */
	e_date_edit_set_time_of_day (E_DATE_EDIT (prefs->start_of_day), calendar_config_get_day_start_hour (), calendar_config_get_day_start_minute ());

	/* End of Day. */
	e_date_edit_set_time_of_day (E_DATE_EDIT (prefs->end_of_day), calendar_config_get_day_end_hour (), calendar_config_get_day_end_minute ());

	/* 12/24 Hour Format. */
	if (calendar_config_get_24_hour_format ())
		e_dialog_toggle_set (prefs->use_24_hour, TRUE);
	else
		e_dialog_toggle_set (prefs->use_12_hour, TRUE);

	sensitive = calendar_config_locale_supports_12_hour_format ();
	gtk_widget_set_sensitive (prefs->use_12_hour, sensitive);
	gtk_widget_set_sensitive (prefs->use_24_hour, sensitive);


	/* Time Divisions. */
	time_divisions = calendar_config_get_time_divisions ();
	e_dialog_combo_box_set (prefs->time_divisions, time_divisions, time_division_map);

	/* Show Appointment End Times. */
	e_dialog_toggle_set (prefs->show_end_times, calendar_config_get_show_event_end ());

	/* Compress Weekend. */
	e_dialog_toggle_set (prefs->compress_weekend, calendar_config_get_compress_weekend ());

	/* Date Navigator - Show Week Numbers. */
	e_dialog_toggle_set (prefs->dnav_show_week_no, calendar_config_get_dnav_show_week_no ());

	/* Task list */
	show_task_list_config (prefs);

	/* Free/Busy */
	show_fb_config (prefs);

	/* Other page */
	e_dialog_toggle_set (prefs->confirm_delete, calendar_config_get_confirm_delete ());
	e_dialog_toggle_set (prefs->default_reminder, calendar_config_get_use_default_reminder ());
	e_dialog_spin_set (prefs->default_reminder_interval, calendar_config_get_default_reminder_interval ());
	e_dialog_combo_box_set (prefs->default_reminder_units, calendar_config_get_default_reminder_units (), default_reminder_units_map);
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
	GtkCellRenderer *renderer = NULL;
	GtkTreeSelection *selection;
	GtkListStore *model;
	const char *working_day_names[] = {
		"sun_button",
		"mon_button",
		"tue_button",
		"wed_button",
		"thu_button",
		"fri_button",
		"sat_button",
	};

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

	/* General tab */
	prefs->timezone = glade_xml_get_widget (gui, "timezone");
	for (i = 0; i < 7; i++)
		prefs->working_days[i] = glade_xml_get_widget (gui, working_day_names[i]);
	prefs->week_start_day = glade_xml_get_widget (gui, "week_start_day");
	prefs->start_of_day = glade_xml_get_widget (gui, "start_of_day");
	prefs->end_of_day = glade_xml_get_widget (gui, "end_of_day");
	prefs->use_12_hour = glade_xml_get_widget (gui, "use_12_hour");
	prefs->use_24_hour = glade_xml_get_widget (gui, "use_24_hour");
	prefs->confirm_delete = glade_xml_get_widget (gui, "confirm_delete");
	prefs->default_reminder = glade_xml_get_widget (gui, "default_reminder");
	prefs->default_reminder_interval = glade_xml_get_widget (gui, "default_reminder_interval");
	prefs->default_reminder_units = glade_xml_get_widget (gui, "default_reminder_units");

	/* Display tab */
	prefs->time_divisions = glade_xml_get_widget (gui, "time_divisions");
	prefs->show_end_times = glade_xml_get_widget (gui, "show_end_times");
	prefs->compress_weekend = glade_xml_get_widget (gui, "compress_weekend");
	prefs->dnav_show_week_no = glade_xml_get_widget (gui, "dnav_show_week_no");
	prefs->tasks_due_today_color = glade_xml_get_widget (gui, "tasks_due_today_color");
	prefs->tasks_overdue_color = glade_xml_get_widget (gui, "tasks_overdue_color");
	prefs->tasks_hide_completed = glade_xml_get_widget (gui, "tasks_hide_completed");
	prefs->tasks_hide_completed_interval = glade_xml_get_widget (gui, "tasks_hide_completed_interval");
	prefs->tasks_hide_completed_units = glade_xml_get_widget (gui, "tasks_hide_completed_units");

	/* Free/Busy tab */
	prefs->url_add = glade_xml_get_widget (gui, "url_add");
	prefs->url_edit = glade_xml_get_widget (gui, "url_edit");
	prefs->url_remove = glade_xml_get_widget (gui, "url_remove");
	prefs->url_enable = glade_xml_get_widget (gui, "url_enable");
	prefs->url_enable_label = glade_xml_get_widget (gui, "url_enable_label");
	prefs->url_enable_image = glade_xml_get_widget (gui, "url_enable_image");
	prefs->url_list = glade_xml_get_widget (gui, "url_list");
	prefs->template_url = glade_xml_get_widget (gui, "template_url");
	g_signal_connect (G_OBJECT (prefs->url_add), "clicked", G_CALLBACK (url_add_clicked), prefs);
	g_signal_connect (G_OBJECT (prefs->url_edit), "clicked", G_CALLBACK (url_edit_clicked), prefs);
	g_signal_connect (G_OBJECT (prefs->url_remove), "clicked", G_CALLBACK (url_remove_clicked), prefs);
	g_signal_connect (G_OBJECT (prefs->url_enable), "clicked", G_CALLBACK (url_enable_clicked), prefs);
	renderer = gtk_cell_renderer_toggle_new ();
	g_object_set (G_OBJECT (renderer), "activatable", TRUE, NULL);
	model = gtk_list_store_new (URL_LIST_N_COLUMNS, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW (prefs->url_list), GTK_TREE_MODEL (model));
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (prefs->url_list),
			                             -1, _("Enabled"), renderer, "active",
			                             URL_LIST_ENABLED_COLUMN, NULL);
	g_signal_connect (renderer, "toggled", G_CALLBACK (url_list_enable_toggled), prefs);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (prefs->url_list),
			                             -1, _("Location"), renderer, "text",
			                             URL_LIST_LOCATION_COLUMN, NULL);
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (prefs->url_list));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (prefs->url_list), TRUE);
	g_signal_connect (prefs->url_list, "row-activated", G_CALLBACK (url_list_double_click), prefs);

	target = e_cal_config_target_new_prefs (ec, prefs->gconf);
	e_config_set_target ((EConfig *)ec, (EConfigTarget *) target);
	toplevel = e_config_create_widget ((EConfig *)ec);
	gtk_container_add (GTK_CONTAINER (prefs), toplevel);

	show_config (prefs);
	/* FIXME: weakref? */
	setup_changes (prefs);
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
