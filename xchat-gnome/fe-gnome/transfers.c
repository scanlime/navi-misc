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

#include <gnome.h>
#include <libgnomevfs/gnome-vfs-mime-utils.h>
#include "transfers.h"
#include "preferences.h"

TransferGui transfer_gui;

static gchar *get_eta_string(struct DCC *dcc);
static float get_divided_size(int value);
static gchar *get_proper_units(int value);
static int get_file_divisor(int value);
static gchar *get_transfer_status(struct DCC *dcc);
static gchar *get_start_time_string(struct DCC *dcc);
static gchar *get_pretty_size_string(struct DCC *dcc);

static void expanded(GtkExpander *exp, GParamSpec *param_spec, gpointer data) {
	GtkWidget *details = glade_xml_get_widget(transfer_gui.xml, "details");

	if(gtk_expander_get_expanded(exp)) {
		gtk_widget_show_all(details);
	} else {
		gtk_widget_hide_all(details);
	}
}

static gboolean transfers_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
	preferences_set_transfers_window_size(event->width, event->height);
	return FALSE;
}

static void update_details(GtkTreeIter *iter, struct DCC *dcc) {
	GtkWidget *label, *progress;
	gchar *s;

	label = glade_xml_get_widget(transfer_gui.xml, "file label");
	gtk_label_set_text(GTK_LABEL(label), dcc->file);
	label = glade_xml_get_widget(transfer_gui.xml, "from label");
	gtk_label_set_text(GTK_LABEL(label), dcc->nick);
	label = glade_xml_get_widget(transfer_gui.xml, "status label");
	if(dcc->dccstat == STAT_ACTIVE) {
		s = get_pretty_size_string(dcc);
		gtk_label_set_text(GTK_LABEL(label), s);
		g_free(s);
	} else {
		s = get_transfer_status(dcc);
		gtk_label_set_text(GTK_LABEL(label), s);
		g_free(s);
	}
	label = glade_xml_get_widget(transfer_gui.xml, "elapsed label");
	s = get_start_time_string(dcc);
	gtk_label_set_text(GTK_LABEL(label), s);
	g_free(s);
	label = glade_xml_get_widget(transfer_gui.xml, "remaining label");
	s = get_eta_string(dcc);
	gtk_label_set_text(GTK_LABEL(label), s);
	g_free(s);

	label = glade_xml_get_widget(transfer_gui.xml, "fromto");
	if(dcc->type == TYPE_SEND)
		gtk_label_set_text(GTK_LABEL(label), "<span weight=\"bold\">To:</span>");
	if(dcc->type == TYPE_RECV)
		gtk_label_set_text(GTK_LABEL(label), "<span weight=\"bold\">From:</span>");
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);

	progress = glade_xml_get_widget(transfer_gui.xml, "transfer progress");
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), (float)dcc->pos / (float)dcc->size);
}

static void selection_changed(GtkTreeSelection *selection, gpointer data) {
	struct DCC *dcc;
	GtkTreeModel *model;
	GtkTreeIter iter;

	if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 6, &dcc, -1);
		transfer_gui.selected = dcc;
		update_details(&iter, dcc);
	} else {
		transfer_gui.selected = NULL;
	}
}

static gboolean transfers_delete(GtkWidget *window, GdkEvent *event, gpointer data) {
	hide_transfers_window();
	return TRUE;
}

void initialize_transfers_window() {
	GtkWidget *expander, *box, *widget, *details, *treeview;
	GtkSizeGroup *group;
	GtkCellRenderer *percent_r, *filename_r, *size_r, *eta_r, *status_r, *icon_r;
	GtkTreeViewColumn *percent_c, *filename_c, *size_c, *eta_c, *status_c;
	GtkTreeSelection *select;
	int width, height;

	transfer_gui.xml = glade_xml_new("transfers.glade", NULL, NULL);
	if(!transfer_gui.xml)
		transfer_gui.xml = glade_xml_new(XCHATSHAREDIR"/transfers.glade", NULL, NULL);
	if(!transfer_gui.xml)
		return;
	transfer_gui.selected = NULL;

	expander = gtk_expander_new("Details");
	gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);
	g_signal_connect(G_OBJECT(expander), "notify::expanded", G_CALLBACK(expanded), NULL);
	box = glade_xml_get_widget(transfer_gui.xml, "expander box");

	gtk_box_pack_start(GTK_BOX(box), expander, FALSE, TRUE, 0);
	gtk_box_reorder_child(GTK_BOX(box), expander, 0);
	gtk_widget_show(expander);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(transfer_gui.xml, "file label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "from label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "status label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "elapsed label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "remaining label");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(transfer_gui.xml, "label2");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "fromto");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "label3");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "label4");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "label5");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	transfer_gui.store = gtk_list_store_new(8, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_POINTER, GDK_TYPE_PIXBUF);
	treeview = glade_xml_get_widget(transfer_gui.xml, "treeview1");
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(transfer_gui.store));
	gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(treeview), TRUE);

	filename_r = gtk_cell_renderer_text_new();
	icon_r = gtk_cell_renderer_pixbuf_new();
	filename_c = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(filename_c, "Filename");
	gtk_tree_view_column_pack_start(filename_c, icon_r, FALSE);
	gtk_tree_view_column_set_attributes(filename_c, icon_r, "pixbuf", 7, NULL);
	gtk_tree_view_column_pack_start(filename_c, filename_r, FALSE);
	gtk_tree_view_column_set_attributes(filename_c, filename_r, "markup", 1, NULL);
	gtk_tree_view_column_set_resizable(filename_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(filename_c, 1);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), filename_c);
	icon_r = gtk_cell_renderer_pixbuf_new();
	/*
	status_r = gtk_cell_renderer_text_new();
	status_c = gtk_tree_view_column_new_with_attributes("Status", status_r, "text", 4, NULL);
	gtk_tree_view_column_set_resizable(status_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(status_c, 4);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), status_c);
	percent_r = gtk_cell_renderer_text_new();
	percent_c = gtk_tree_view_column_new_with_attributes("%", percent_r, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(percent_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(percent_c, 5);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), percent_c);
	size_r = gtk_cell_renderer_text_new();
	size_c = gtk_tree_view_column_new_with_attributes("Size", size_r, "text", 2, NULL);
	gtk_tree_view_column_set_resizable(size_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(size_c, 2);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), size_c);
	eta_r = gtk_cell_renderer_text_new();
	eta_c = gtk_tree_view_column_new_with_attributes("Remaining", eta_r, "text", 3, NULL);
	gtk_tree_view_column_set_resizable(eta_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(eta_c, 3);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), eta_c);
	*/

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(selection_changed), NULL);

	widget = glade_xml_get_widget(transfer_gui.xml, "window1");
	g_signal_connect(G_OBJECT(widget), "delete-event", transfers_delete, NULL);
	details = glade_xml_get_widget(transfer_gui.xml, "details");
	preferences_get_transfers_window_size(&width, &height);
	gtk_widget_show_all(widget);
	gtk_widget_hide_all(details);
	if(!(width == 0 || height == 0))
		gtk_window_resize(GTK_WINDOW(widget), width, height);
	g_signal_connect(G_OBJECT(widget), "configure-event", G_CALLBACK(transfers_resize), NULL);
}

void show_transfers_window() {
	GtkWidget *window;

	window = glade_xml_get_widget(transfer_gui.xml, "window1");
	gtk_widget_show_all(window);
}

void hide_transfers_window() {
	GtkWidget *window;

	window = glade_xml_get_widget(transfer_gui.xml, "window1");
	gtk_widget_hide_all(window);
}

static int get_file_divisor(int value) {
	int mod = 1, i;
	for(i = 0; i <= 3; i++) {
		if(value / (mod * 1024) == 0)
			break;
		mod *= 1024;
	}
	return mod;
}

static gchar *get_proper_units(int value) {
	int divisor = get_file_divisor(value);
	if(divisor == 1)
		return g_strdup("B");
	if(divisor == 1024)
		return g_strdup("KB");
	if(divisor == 1024*1024)
		return g_strdup("MB");
	return g_strdup("GB");
}

static float get_divided_size(int value) {
	return ((float)value / (float)get_file_divisor(value));
}

static gchar *get_file_size(struct DCC *dcc) {
	gchar *ssize;
	int size = dcc->size;
	gchar *unit;

	unit = get_proper_units(size);
	ssize = g_strdup_printf("%.1f%s", get_divided_size(size), unit);
	g_free(unit);
	return ssize;
}

static gchar *get_transfer_status(struct DCC *dcc) {
	switch(dcc->dccstat) {
		case STAT_QUEUED:	return g_strdup("Waiting");
		case STAT_ACTIVE:	return g_strdup("Active");
		case STAT_FAILED:	return g_strdup("Failed");
		case STAT_DONE:		return g_strdup("Done");
		case STAT_CONNECTING:	return g_strdup("Connecting");
		case STAT_ABORTED:	return g_strdup("Aborted");
		default:		return g_strdup("");
	}
}

static gchar *get_eta_string(struct DCC *dcc) {
	int to_go;
	int hours, minutes, seconds;

	if(dcc->cps != 0) {
		to_go = (dcc->size - dcc->pos) / dcc->cps;
		hours = to_go / 3600;
		minutes = (to_go / 60) % 60;
		seconds = to_go % 60;
		if(hours != 0)
			return g_strdup_printf("%.2d:%.2d:%.2d", hours, minutes, seconds);
		if(minutes != 0)
			return g_strdup_printf("%.2d:%.2d", minutes, seconds);
		return g_strdup_printf("0:%.2d", seconds);
	} else {
		return g_strdup("");
	}
}

static gchar *get_start_time_string(struct DCC *dcc) {
	int past = dcc->lasttime - dcc->starttime;
	int hours, minutes, seconds;

	hours = past / 3600;
	minutes = (past / 60) % 60;
	seconds = past % 60;
	if(hours != 0)
		return g_strdup_printf("%.2d:%.2d:%.2d", hours, minutes, seconds);
	if(minutes != 0)
		return g_strdup_printf("%.2d:%.2d", minutes, seconds);
	return g_strdup_printf("0:%.2d", seconds);
}

static GdkPixbuf *get_file_icon(char *filename) {
	char *mime = gnome_vfs_get_mime_type(filename);
	GnomeIconTheme *theme;
	GdkPixbuf *p;
	char *icon;

	if(mime == NULL)
		return NULL;

	theme = gnome_icon_theme_new();
	gnome_icon_theme_set_allow_svg(theme, TRUE);

	icon = gnome_icon_lookup(theme, NULL, NULL, NULL, NULL, mime, GNOME_ICON_LOOKUP_FLAGS_NONE, NULL);

	if(!g_path_is_absolute(icon)) {
		char *path;

		path = gnome_icon_theme_lookup_icon(theme, icon, 48, NULL, NULL);
		g_free(icon);
		icon = path;
	}
	g_object_unref(theme);

	p = gdk_pixbuf_new_from_file(icon, NULL);

	g_free(icon);
	g_free(mime);
	return p;
}

static gchar *get_markedup_name(struct DCC *dcc) {
	gchar *s, *t, *u;
	if(dcc->dccstat == STAT_ACTIVE) {
		t = get_pretty_size_string(dcc);
		u = get_eta_string(dcc);
		s = g_strdup_printf("<span weight=\"bold\">%s</span>\n    <span size=\"small\" weight=\"bold\">Progress: </span><span size=\"small\">%s</span>\n    <span size=\"small\" weight=\"bold\">Time Remaining: </span><span size=\"small\">%s</span>", dcc->file, t, u);
		g_free(t);
		g_free(u);
	} else {
		t = get_transfer_status(dcc);
		s = g_strdup_printf("<span weight=\"bold\">%s</span>\n    <span size=\"small\">%s</span>\n", dcc->file, t);
		g_free(t);
	}
	return s;
}

static gchar *get_pretty_size_string(struct DCC *dcc) {
	float d = get_file_divisor(dcc->size);
	float t = (float)dcc->pos / d;
	gchar *u = get_proper_units(dcc->size);
	gchar *v = get_proper_units(dcc->cps);
	gchar *s = g_strdup_printf("%1.1f of %1.1f%s at %1.1f%s/s", t, get_divided_size(dcc->size), u, get_divided_size(dcc->cps), v);
	g_free(u);
	g_free(v);
	return s;
}

static void update_transfer_info(GtkTreeIter *iter, struct DCC *dcc) {
	int per;
	gchar *markedupname, *percent, *size, *eta;
	GdkPixbuf *icon;

	per = (int) ((dcc->pos * 100.0) / dcc->size);
	markedupname = get_markedup_name(dcc);
	percent = g_strdup_printf("%d%%", per);
	size = get_file_size(dcc);
	eta = get_eta_string(dcc);
	icon = get_file_icon(dcc->destfile);
	gtk_list_store_set(transfer_gui.store, iter, 0, percent, 1, markedupname, 2, size, 3, eta, 4, stat, 5, per, 6, dcc, 7, icon, -1);
	g_free(percent);
	g_free(size);
	g_free(markedupname);
	g_free(eta);
	if(G_IS_OBJECT(icon))
		gdk_pixbuf_unref(icon);
	if(transfer_gui.selected == dcc)
		update_details(iter, dcc);
}

void add_transfer(struct DCC *dcc) {
	GtkTreeIter iter;
	GtkWidget *treeview;
	GtkTreeSelection *select;

	gtk_list_store_append(transfer_gui.store, &iter);
	update_transfer_info(&iter, dcc);

	treeview = glade_xml_get_widget(transfer_gui.xml, "treeview1");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_select_iter(select, &iter);
	transfer_gui.selected = dcc;
}

void update_transfer(struct DCC *dcc) {
	GtkTreeIter iter;
	struct DCC *d;

	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(transfer_gui.store), &iter)) {
		do {
			gtk_tree_model_get(GTK_TREE_MODEL(transfer_gui.store), &iter, 6, &d, -1);
			if(d == dcc) {
				update_transfer_info(&iter, dcc);
				return;
			}
		} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(transfer_gui.store), &iter));
	}
}

void remove_transfer(struct DCC *dcc) {
}
