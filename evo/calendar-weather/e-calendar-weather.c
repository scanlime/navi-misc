/*
 * 
 *
 * Copyright (C) 2004 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
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
#include <calendar/ec-config.h>
#include <libedataserver/e-source.h>
#include <libedataserver/e-url.h>
#include <libgnome/gnome-i18n.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static GtkTreeStore *store = NULL;

static void
parse_subtree (GtkTreeIter *parent, xmlNode *node)
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
		}
		else {
			xmlAttr *attr;

			for (child = node->children; child; child = child->next)
				parse_subtree (&iter, child);

			for (attr = node->properties; attr; attr = attr->next) {
				if (strcmp (attr->name, "name") == 0) {
					gtk_tree_store_set (store, &iter, 0, attr->children->content, -1);
				}
			}
		}
	}

}

static void
load_locations ()
{
	xmlDoc *doc;
	xmlNode *root, *child;

	LIBXML_TEST_VERSION

	doc = xmlParseFile("/home/jupiter/navi-misc/evo/Locations.xml.in");
	if (doc == NULL) {
		g_warning ("failed to read locations file");
		return;
	}

	store = gtk_tree_store_new (4,
	    G_TYPE_STRING,		/* name */
	    G_TYPE_STRING,		/* code */
	    G_TYPE_STRING,		/* URL */
	    G_TYPE_STRING);		/* type */

	root = xmlDocGetRootElement (doc);

	for (child = root->children; child; child = child->next)
		parse_subtree (NULL, child);

	xmlFreeDoc (doc);
}

GtkWidget *
e_calendar_weather_location (EPlugin *epl, EConfigHookItemFactoryData *data)
{
	GtkWidget *label, *swin, *treeview, *vbox, *parent;
	int row;
	ECConfigTargetSource *t = (ECConfigTargetSource *) data->target;
	ESource *source = t->source;
	ESourceGroup *group = e_source_peek_group (source);
	static GtkWidget *hidden = NULL;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	if (!hidden)
		hidden = gtk_label_new ("");

	if (strcmp (e_source_group_peek_name (group), _("Weather")))
		return hidden;

	if (data->old)
		return data->old;

	if (!store)
		load_locations ();

	parent = data->parent;

	row = ((GtkTable*)parent)->nrows;

	vbox = gtk_vbox_new (FALSE, 6);
	gtk_widget_show (vbox);

	label = gtk_label_new (_("Select a location:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);

	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request (swin, 100, 250); /* BLAH */
	gtk_widget_show (swin);
	gtk_box_pack_end (GTK_BOX (vbox), swin, TRUE, TRUE, 0);

	treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	gtk_widget_show (treeview);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), treeview);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Location", renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

	gtk_table_attach (GTK_TABLE (parent), vbox, 0, 2, row, row+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	return treeview;
}

static void
set_refresh_time (ESource *source, GtkWidget *spin, GtkWidget *option)
{
	int time;
	int item_num = 0;
	const char *refresh_str = e_source_get_property (source, "refresh");
	time = refresh_str ? atoi (refresh_str) : 30;

	if (time  && !(time % 10080)) {
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

GtkWidget *
e_calendar_weather_refresh (EPlugin *epl, EConfigHookItemFactoryData *data)
{
	GtkWidget *label, *spin, *option, *menu, *hbox, *parent;
	GtkWidget *times[4];
	int row, i;
	ECConfigTargetSource *t = (ECConfigTargetSource *) data->target;
	ESource *source = t->source;
	ESourceGroup *group = e_source_peek_group (source);
	static GtkWidget *hidden = NULL;

	if (!hidden)
		hidden = gtk_label_new ("");

	if (strcmp (e_source_group_peek_name (group), _("Weather")))
		return hidden;

	if (data->old)
		return data->old;

	parent = data->parent;

	row = ((GtkTable*)parent)->nrows;

	label = gtk_label_new (_("Refresh:"));
	gtk_widget_show (label);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (parent), label, 0, 1, row, row+1, GTK_FILL, 0, 0, 0);

	hbox = gtk_hbox_new (FALSE, 6);
	gtk_widget_show (hbox);

	spin = gtk_spin_button_new_with_range (0, 100, 1);
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

	gtk_table_attach (GTK_TABLE (parent), hbox, 1, 2, row, row+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	g_object_set_data (G_OBJECT (epl), "calendar.weather.refresh.spin", spin);
	g_object_set_data (G_OBJECT (epl), "calendar.weather.refresh.option", option);

	return hbox;
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

void
e_calendar_weather_commit (EPlugin *epl, ECConfigTargetSource *t)
{
	GtkWidget *spin, *option;
	ESource *source = t->source;
	char *refresh_str;
	ESourceGroup *group = e_source_peek_group (source);

	if (strcmp (e_source_group_peek_name (group), _("Weather")))
		return;

	spin = g_object_get_data (G_OBJECT (epl), "calendar.weather.refresh.spin");
	option = g_object_get_data (G_OBJECT (epl), "calendar.weather.refresh.option");

	refresh_str = get_refresh_minutes (spin, option);
	e_source_set_property (source, "refresh", refresh_str);
	g_free (refresh_str);
}
