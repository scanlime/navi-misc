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

#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtktable.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkhbox.h>
#include <e-util/e-config.h>
#include <calendar/gui/ec-config.h>
#include <libedataserver/e-source.h>
#include <libedataserver/e-url.h>
#include <libgnome/gnome-i18n.h>
#include <string.h>

GtkWidget *e_calendar_http_url (EPlugin *epl, EConfigHookItemFactoryData *data);
GtkWidget *e_calendar_http_refresh (EPlugin *epl, EConfigHookItemFactoryData *data);
void       e_calendar_http_commit (EPlugin *epl, ECConfigTargetSource *t);
gboolean   e_calendar_http_check (EPlugin *epl, EConfigHookPageCheckData *data);

GtkWidget *
e_calendar_http_url (EPlugin *epl, EConfigHookItemFactoryData *data)
{
	GtkWidget *label, *entry, *parent;
	int row;
	ECConfigTargetSource *t = (ECConfigTargetSource *) data->target;
	ESource *source = t->source;
	EUri *uri;
	static GtkWidget *hidden = NULL;

	if (!hidden)
		hidden = gtk_label_new ("");

	uri = e_uri_new (e_source_get_uri (source));
	if (strcmp (uri->protocol, "webcal") &&
	    strcmp (uri->protocol, "http"))
		return hidden;
	e_uri_free (uri);

	if (data->old)
		return data->old;

	parent = data->parent;

	row = ((GtkTable*)parent)->nrows;

	label = gtk_label_new_with_mnemonic (_("_URL:"));
	gtk_widget_show (label);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (parent), label, 0, 1, row, row+1, GTK_FILL, 0, 0, 0);

	entry = gtk_entry_new ();
	gtk_widget_show (entry);
	gtk_entry_set_text (GTK_ENTRY (entry), e_source_get_uri (source));
	gtk_table_attach (GTK_TABLE (parent), entry, 1, 2, row, row+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);

	g_object_set_data (G_OBJECT (source), "calendar.http.uri", entry);

	return entry;
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
e_calendar_http_refresh (EPlugin *epl, EConfigHookItemFactoryData *data)
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

	if (strcmp (e_source_group_peek_name (group), _("On The Web")))
		return hidden;

	if (data->old)
		return data->old;

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

	gtk_table_attach (GTK_TABLE (parent), hbox, 1, 2, row, row+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	g_object_set_data (G_OBJECT (source), "calendar.http.refresh.spin", spin);
	g_object_set_data (G_OBJECT (source), "calendar.http.refresh.option", option);

	return hbox;
}

static gchar *
print_uri_noproto (EUri *uri)
{
	gchar *uri_noproto;

	if (uri->port != 0)
		uri_noproto = g_strdup_printf (
			"%s%s%s%s%s%s%s:%d%s%s%s",
			uri->user ? uri->user : "",
			uri->authmech ? ";auth=" : "",
			uri->authmech ? uri->authmech : "",
			uri->passwd ? ":" : "",
			uri->passwd ? uri->passwd : "",
			uri->user ? "@" : "",
			uri->host ? uri->host : "",
			uri->port,
			uri->path ? uri->path : "",
			uri->query ? "?" : "",
			uri->query ? uri->query : "");
	else
		uri_noproto = g_strdup_printf (
			"%s%s%s%s%s%s%s%s%s%s",
			uri->user ? uri->user : "",
			uri->authmech ? ";auth=" : "",
			uri->authmech ? uri->authmech : "",
			uri->passwd ? ":" : "",
			uri->passwd ? uri->passwd : "",
			uri->user ? "@" : "",
			uri->host ? uri->host : "",
			uri->path ? uri->path : "",
			uri->query ? "?" : "",
			uri->query ? uri->query : "");
	return uri_noproto;
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
e_calendar_http_commit (EPlugin *epl, ECConfigTargetSource *t)
{
	GtkWidget *entry, *spin, *option;
	ESource *source = t->source;
	EUri *uri;
	char *relative_uri;
	char *refresh_str;
	ESourceGroup *group = e_source_peek_group (source);

	if (strcmp (e_source_group_peek_name (group), _("On The Web")))
		return;

	entry = g_object_get_data (G_OBJECT (source), "calendar.http.uri");
	spin = g_object_get_data (G_OBJECT (source), "calendar.http.refresh.spin");
	option = g_object_get_data (G_OBJECT (source), "calendar.http.refresh.option");

	uri = e_uri_new (gtk_entry_get_text (GTK_ENTRY (entry)));
	relative_uri = print_uri_noproto (uri);
	e_source_set_relative_uri (source, relative_uri);
	g_free (relative_uri);
	g_free (uri);

	refresh_str = get_refresh_minutes (spin, option);
	e_source_set_property (source, "refresh", refresh_str);
	g_free (refresh_str);
}

gboolean
e_calendar_http_check (EPlugin *epl, EConfigHookPageCheckData *data)
{
	ECConfigTargetSource *t = (ECConfigTargetSource *) data->target;
	ESource *source = t->source;
	GtkWidget *entry = g_object_get_data (G_OBJECT (source), "calendar.http.uri");
	EUri *uri;
	gboolean ok = FALSE;

	/* FIXME - check pageid */
	uri = e_uri_new (gtk_entry_get_text (GTK_ENTRY (entry)));
	ok = ((!strcmp (uri->protocol, "webcal")) || (!strcmp (uri->protocol, "http")));
	e_uri_free (uri);
	return ok;
}
