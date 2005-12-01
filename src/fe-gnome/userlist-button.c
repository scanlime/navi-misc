/*
 * userlist-button.c - Widget encapsulating the userlist
 *
 * Copyright (C) 2005 David Trowbridge
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

static void userlist_button_class_init (UserlistButtonClass *klass);
static void userlist_button_init       (UserlistButton      *button);
static void userlist_button_finalize   (GObject             *object);

static GtkToggleButtonClass *parent_class;

G_DEFINE_TYPE (UserlistButton, userlist_button, GTK_TYPE_TOGGLE_BUTTON);

struct _UserlistButtonPriv
{
	GtkWidget *hbox;
	GtkWidget *image;
	GtkWidget *label;
	GtkWidget *alignment;
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
	button->priv = g_new0(UserlistButtonPriv, 1);
	button->priv->hbox      = gtk_hbox_new (FALSE, 3);
	button->priv->label     = gtk_label_new ("Users");
	button->priv->alignment = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);

	if (g_file_test ("../../data/users.png", G_FILE_TEST_EXISTS))
		button->priv->image = gtk_image_new_from_file ("../../data/users.png");
	else
		button->priv->image = gtk_image_new_from_file (XCHATSHAREDIR "/users.png");

	gtk_box_pack_start (GTK_BOX (button->priv->hbox), button->priv->image, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (button->priv->hbox), button->priv->label, FALSE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (button->priv->alignment), button->priv->hbox);
	gtk_container_add (GTK_CONTAINER (button), button->priv->alignment);

	gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);

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

	g_free (button->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

GtkWidget *
userlist_button_new (void)
{
	return GTK_WIDGET (g_object_new (userlist_button_get_type (), NULL));
}
