/*
 *
 *
 * Copyright (C) 2004 David Trowbridge
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

#include <gtk/gtk.h>
#include <e-util/e-config.h>
#include <calendar/gui/e-cal-config.h>
#include <libedataserver/e-source.h>
#include <libedataserver/e-url.h>
#include <libgnome/gnome-i18n.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

GtkWidget *e_calendar_weather_location (EPlugin *epl, EConfigHookItemFactoryData *data);
GtkWidget *e_calendar_weather_refresh (EPlugin *epl, EConfigHookItemFactoryData *data);
gboolean   e_calendar_weather_check (EPlugin *epl, EConfigHookPageCheckData *data);

static void
parse_subtree (GtkTreeStore *store, GtkTreeIter *parent, xmlNode *node)
{
	GtkTreeIter iter;
	xmlNode *child;

	if (node->type == XML_ELEMENT_NODE) {
		gtk_tree_store_append (store, &iter, parent);
		if (strcmp (node->name, "location") == 0) {
			xmlAttr *attr;

			child = node->children;
			g_assert (child->type == XML_TEXT_NODE);
			gtk_tree_store_set (store, &iter, 0, child->content, -1);

			for (attr = node->properties; attr; attr = attr->next) {
			}
		} else {
			xmlAttr *attr;

			for (child = node->children; child; child = child->next)
				parse_subtree (store, &iter, child);

			for (attr = node->properties; attr; attr = attr->next) {
				if (strcmp (attr->name, "name") == 0)
					gtk_tree_store_set (store, &iter, 0, attr->children->content, -1);
				else if (strcmp (attr->name, "code") == 0)
					gtk_tree_store_set (store, &iter, 1, attr->children->content, -1);
				else if (strcmp (attr->name, "url") == 0)
					gtk_tree_store_set (store, &iter, 2, attr->children->content, -1);
				else if (strcmp (attr->name, "type") == 0)
					gtk_tree_store_set (store, &iter, 3, attr->children->content, -1);
			}
		}
	}
}

static GtkTreeStore *
load_locations ()
{
	xmlDoc *doc;
	xmlNode *root, *child;
	GtkTreeStore *store;

	LIBXML_TEST_VERSION

	doc = xmlParseFile("/home/jupiter/navi-misc/evo/Locations.xml.in");
	if (doc == NULL) {
		g_warning ("failed to read locations file");
		return NULL;
	}

	store = gtk_tree_store_new (4,
		G_TYPE_STRING,	/* name */
		G_TYPE_STRING,	/* code */
		G_TYPE_STRING,	/* URL  */
		G_TYPE_STRING);	/* type */

	root = xmlDocGetRootElement (doc);
	for (child = root->children; child; child = child->next)
		parse_subtree (store, NULL, child);
	xmlFreeDoc (doc);
	return store;
}

static void
selection_changed (GtkTreeSelection *selection, GtkDialog *dialog)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gchar *code;
		gtk_tree_model_get (model, &iter, 1, code, -1);
		if (code != NULL)
			gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, TRUE);
	} else {
		gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);
	}
}

static GtkDialog *
create_source_selector ()
{
	GtkWidget *dialog, *treeview, *scrolledwindow;
	GtkCellRenderer *text;
	GtkTreeStore *store;
	GtkTreeSelection *selection;

	store = load_locations ();
	if (store == NULL)
		return NULL;

	dialog = gtk_dialog_new_with_buttons (
	    		_("Select a location"),
	    		NULL, GTK_DIALOG_MODAL,
			GTK_STOCK_OK, GTK_RESPONSE_OK,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_show (scrolledwindow);
	treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
	gtk_widget_show (treeview);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolledwindow), treeview);

	text = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview), -1, "location", text, "text", 0, NULL);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), scrolledwindow);

	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 420, 340);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
	g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK (selection_changed), dialog);

	return GTK_DIALOG (dialog);
}

static void
location_clicked (GtkButton *button, gpointer data)
{
	GtkDialog *dialog = create_source_selector ();
	gint response;

	response = gtk_dialog_run (dialog);

	if (response == GTK_RESPONSE_OK) {
	}

	gtk_widget_destroy (dialog);
}

GtkWidget *
e_calendar_weather_location (EPlugin *epl, EConfigHookItemFactoryData *data)
{
	static GtkWidget *label;
	GtkWidget *button, *parent, *text;
	int row;
	ECalConfigTargetSource *t = (ECalConfigTargetSource *) data->target;
	ESource *source = t->source;
	EUri *uri;
	char *uri_text;
	static GtkWidget *hidden;

	if (!hidden)
		hidden = gtk_label_new ("");

	if (data->old)
		gtk_widget_destroy (label);

	uri_text = e_source_get_uri (t->source);
	uri = e_uri_new (uri_text);
	if (strcmp (uri->protocol, "weather")) {
		e_uri_free (uri);
		return hidden;
	}
	e_uri_free (uri);

	parent = data->parent;

	row = ((GtkTable*)parent)->nrows;

	label = gtk_label_new_with_mnemonic (_("_Location:"));
	gtk_widget_show (label);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (parent), label, 0, 1, row, row+1, GTK_FILL, 0, 0, 0);

	button = gtk_button_new ();
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (location_clicked), NULL);
	gtk_widget_show (button);

	text = gtk_label_new (_("None"));
	gtk_widget_show (text);
#if (GTK_CHECK_VERSION(2, 6, 0))
	gtk_label_set_ellipsize (GTK_LABEL (text), PANGO_ELLIPSIZE_START);
#endif
	gtk_container_add (GTK_CONTAINER (button), text);

	gtk_table_attach (GTK_TABLE (parent), button, 1, 2, row, row+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	return button;
}

static void
set_refresh_time (ESource *source, GtkWidget *spin, GtkWidget *option)
{
	int time;
	int item_num = 0;
	const char *refresh_str = e_source_get_property (source, "refresh");
	time = refresh_str ? atoi (refresh_str) : 30;

	if (time && !(time % 10080)) {
		/* weeks */
		item_num = 3;
		time /= 10080;
	} else if (time && !(time % 1440)) {
		/* days */
		item_num = 2;
		time /= 1440;
	} else if (time && !(time % 60)) {
		/* hours */
		item_num = 1;
		time /= 60;
	}
	gtk_option_menu_set_history (GTK_OPTION_MENU (option), item_num);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin), time);
}

static char *
get_refresh_minutes (GtkWidget *spin, GtkWidget *option)
{
	int setting = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin));
	switch (gtk_option_menu_get_history (GTK_OPTION_MENU (option))) {
	case 0:
		/* minutes */
		break;
	case 1:
		/* hours */
		setting *= 60;
		break;
	case 2:
		/* days */
		setting *= 1440;
		break;
	case 3:
		/* weeks - is this *really* necessary? */
		setting *= 10080;
		break;
	default:
		g_warning ("Time unit out of range");
		break;
	}
	return g_strdup_printf ("%d", setting);
}

static void
spin_changed (GtkSpinButton *spin, ECalConfigTargetSource *t)
{
	char *refresh_str;
	GtkWidget *option;

	option = g_object_get_data (G_OBJECT (spin), "option");

	refresh_str = get_refresh_minutes ((GtkWidget *) spin, option);
	e_source_set_property (t->source, "refresh", refresh_str);
	g_free (refresh_str);
}

static void
option_changed (GtkOptionMenu *option, ECalConfigTargetSource *t)
{
	char *refresh_str;
	GtkWidget *spin;

	spin = g_object_get_data (G_OBJECT (option), "spin");

	refresh_str = get_refresh_minutes (spin, (GtkWidget *) option);
	e_source_set_property (t->source, "refresh", refresh_str);
	g_free (refresh_str);
}

GtkWidget *
e_calendar_weather_refresh (EPlugin *epl, EConfigHookItemFactoryData *data)
{
	static GtkWidget *label;
	GtkWidget *option, *spin, *menu, *hbox, *parent;
	GtkWidget *times[4];
	int row, i;
	ECalConfigTargetSource *t = (ECalConfigTargetSource *) data->target;
	ESource *source = t->source;
	EUri *uri;
	char *uri_text;
	static GtkWidget *hidden = NULL;

	if (!hidden)
		hidden = gtk_label_new ("");

	if (data->old)
		gtk_widget_destroy (label);

	uri_text = e_source_get_uri (t->source);
	uri = e_uri_new (uri_text);
	g_free (uri_text);
	if (strcmp (uri->protocol, "weather")) {
		e_uri_free (uri);
		return hidden;
	}
	e_uri_free (uri);

	parent = data->parent;

	row = ((GtkTable*)parent)->nrows;

	label = gtk_label_new_with_mnemonic (_("_Refresh:"));
	gtk_widget_show (label);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (parent), label, 0, 1, row, row+1, GTK_FILL, 0, 0, 0);

	hbox = gtk_hbox_new (FALSE, 6);
	gtk_widget_show (hbox);

	spin = gtk_spin_button_new_with_range (0, 100, 1);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin);
	gtk_widget_show (spin);
	gtk_box_pack_start (GTK_BOX (hbox), spin, FALSE, TRUE, 0);

	option = gtk_option_menu_new ();
	gtk_widget_show (option);
	times[0] = gtk_menu_item_new_with_label (_("minutes"));
	times[1] = gtk_menu_item_new_with_label (_("hours"));
	times[2] = gtk_menu_item_new_with_label (_("days"));
	times[3] = gtk_menu_item_new_with_label (_("weeks"));
	menu = gtk_menu_new ();
	gtk_widget_show (menu);
	for (i = 0; i < 4; i++) {
		gtk_widget_show (times[i]);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), times[i]);
	}
	gtk_option_menu_set_menu (GTK_OPTION_MENU (option), menu);
	set_refresh_time (source, spin, option);
	gtk_box_pack_start (GTK_BOX (hbox), option, FALSE, TRUE, 0);

	g_object_set_data (G_OBJECT (option), "spin", spin);
	g_signal_connect (G_OBJECT (option), "changed", G_CALLBACK (option_changed), t);
	g_object_set_data (G_OBJECT (spin), "option", option);
	g_signal_connect (G_OBJECT (spin), "value-changed", G_CALLBACK (spin_changed), t);

	gtk_table_attach (GTK_TABLE (parent), hbox, 1, 2, row, row+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	return hbox;
}

gboolean
e_calendar_weather_check (EPlugin *epl, EConfigHookPageCheckData *data)
{
	/* FIXME - check pageid */
	ECalConfigTargetSource *t = (ECalConfigTargetSource *) data->target;
	EUri *uri;
	gboolean ok = FALSE;
	ESourceGroup *group = e_source_peek_group (t->source);

	if (strncmp (e_source_group_peek_base_uri (group), "weather", 7))
		return TRUE;

	uri = e_uri_new (e_source_get_uri (t->source));
	/* FIXME - check location */
	ok = TRUE;
	e_uri_free (uri);

	return ok;
}
