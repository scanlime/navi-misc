/*
 * userlist-button.c - Widget encapsulating the userlist toggle button
 *
 * Copyright (C) 2005 xchat-gnome team
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
#include <config.h>
#include <gtk/gtkalignment.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkimage.h>
#include <gtk/gtklabel.h>
#include "userlist-button.h"
#include "userlist-window.h"

static void userlist_button_class_init (UserlistButtonClass *klass);
static void userlist_button_init       (UserlistButton      *button);
static void userlist_button_finalize   (GObject             *object);
static void userlist_button_toggled    (UserlistButton      *button,
                                        gpointer             data);

static GtkToggleButtonClass *parent_class;

G_DEFINE_TYPE (UserlistButton, userlist_button, GTK_TYPE_TOGGLE_BUTTON);

struct _UserlistButtonPriv
{
	GtkWidget      *hbox;
	GtkWidget      *image;
	GtkWidget      *label;
	GtkWidget      *alignment;
	GtkWidget      *userlist;

	GHashTable     *numbers;
	struct session *current;
};

static void
userlist_button_class_init (UserlistButtonClass *klass)
{
	GObjectClass *gobject_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = userlist_button_finalize;
}

static void
userlist_button_init (UserlistButton *button)
{
	button->priv            = g_new0 (UserlistButtonPriv, 1);
	button->priv->hbox      = gtk_hbox_new (FALSE, 3);
	button->priv->label     = gtk_label_new (_("Users"));
	button->priv->alignment = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);

	button->priv->numbers   = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);

	if (g_file_test ("../../data/users.png", G_FILE_TEST_EXISTS))
		button->priv->image = gtk_image_new_from_file ("../../data/users.png");
	else
		button->priv->image = gtk_image_new_from_file (XCHATSHAREDIR "/users.png");

	button->priv->userlist = userlist_window_new ();

	gtk_box_pack_start (GTK_BOX (button->priv->hbox), button->priv->image, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (button->priv->hbox), button->priv->label, FALSE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (button->priv->alignment), button->priv->hbox);
	gtk_container_add (GTK_CONTAINER (button), button->priv->alignment);

	gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);

	g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (userlist_button_toggled), NULL);

	gtk_widget_show (button->priv->alignment);
	gtk_widget_show (button->priv->hbox);
	gtk_widget_show (button->priv->image);
	gtk_widget_show (button->priv->label);
	gtk_widget_show (GTK_WIDGET (button));
}

static void
userlist_button_finalize (GObject *object)
{
	UserlistButton *button;

	button = USERLIST_BUTTON (object);

	g_object_unref (button->priv->userlist);
	g_hash_table_destroy (button->priv->numbers);
	g_free (button->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

GtkWidget *
userlist_button_new (void)
{
	return GTK_WIDGET (g_object_new (userlist_button_get_type (), NULL));
}

void
userlist_button_set_users (UserlistButton *button, struct session *sess, int users)
{
	gchar *string;

	string = g_strdup_printf (_("%d Users"), users);
	g_hash_table_insert (button->priv->numbers, sess, string);

	if (sess == button->priv->current)
		gtk_label_set_text (GTK_LABEL (button->priv->label), string);
}

void
userlist_button_set_current (UserlistButton *button, struct session *sess)
{
	if (sess->type == SESS_CHANNEL) {
		gchar *users;

		users = g_hash_table_lookup (button->priv->numbers, sess);
		gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
		if (users)
			gtk_label_set_text (GTK_LABEL (button->priv->label), users);
		else
			gtk_label_set_text (GTK_LABEL (button->priv->label), _("Users"));
	} else {
		gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
		gtk_label_set_text (GTK_LABEL (button->priv->label), _("Users"));
	}
}

void
userlist_button_remove_session (UserlistButton *button, struct session *sess)
{
	g_hash_table_remove (button->priv->numbers, sess);
	if (sess == button->priv->current)
		userlist_button_set_current (button, NULL);
}

static void
userlist_button_toggled (UserlistButton *button, gpointer data)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
		userlist_window_show (USERLIST_WINDOW (button->priv->userlist));
	else
		userlist_window_hide (USERLIST_WINDOW (button->priv->userlist));
}
