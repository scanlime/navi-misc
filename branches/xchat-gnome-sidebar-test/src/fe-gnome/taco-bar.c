/*
 * Based around Jonathan Blandford's sidebar from evince.
 *
 *  Copyright (C) 2004-2007 xchat-gnome team
 *
 * xchat-gnome is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xchat-gnome is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "taco-side-bar.h"

typedef struct
{
	char *id;
	char *title;
	GtkWidget *child;
	GtkWidget *toggle;
	TacoBar *taco_bar;
} TacoBarEntry;

struct _TacoBarPrivate {
	TacoBarEntry *active;
	GHashTable *entry_hash;
};

G_DEFINE_TYPE (TacoBar, taco_bar, GTK_TYPE_VBOX)

#define TACO_BAR_GET_PRIVATE(object) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((object), TYPE_TACO_BAR, TacoBarPrivate))

/* Private functions */

static void
taco_bar_destroy (GtkObject *object)
{
    	// FIXME: Leakahol
/*	TacoBar *taco_bar = TACO_BAR (object);

	g_free (taco_bar->priv->current);
	taco_bar->priv->current = NULL;

	if (taco_bar->priv->menu) {
		gtk_menu_detach (GTK_MENU (taco_bar->priv->menu));
		taco_bar->priv->menu = NULL;
	}

	(* GTK_OBJECT_CLASS (taco_bar_parent_class)->destroy) (object);*/
}

static void
taco_bar_class_init (TacoBarClass *taco_bar_class)
{
	GObjectClass *g_object_class;
	GtkWidgetClass *widget_class;
	GtkObjectClass *gtk_object_klass;
 
	g_object_class = G_OBJECT_CLASS (taco_bar_class);
	widget_class = GTK_WIDGET_CLASS (taco_bar_class);
	gtk_object_klass = GTK_OBJECT_CLASS (taco_bar_class);
	   
	g_type_class_add_private (g_object_class, sizeof (TacoBarPrivate));
	   
	gtk_object_klass->destroy = taco_bar_destroy;
}

void
taco_bar_activate (TacoBar *taco_bar, TacoBarEntry *new_active)
{
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (new_active->button), TRUE);
	gtk_widget_set_sensitive (new_active->button, FALSE);
	gtk_widget_show (new_active->child);
	taco_bar->priv->active = new_active;
}

void
taco_bar_deactivate_current (TacoBar *taco_bar)
{
    	TacoBarEntry *old_active = taco_bar->active;
	
	g_return_if_fail (old_active != NULL);

	gtk_widget_hide (old_active->child);
	old_active = taco_bar->priv->active;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (old_active->button), FALSE);
	gtk_widget_set_sensitive (old_active->button, TRUE);
}

static void
taco_bar_on_toggle (GtkWidget *widget,
		    TacoBarEntry *entry)
{
    	TacoBar *taco_bar = entry->taco_bar;

	g_return_if_fail (entry != taco_bar->active);

	taco_bar_deactivate_current (taco_bar);
	taco_bar_activate (taco_bar, entry);
}

static void
taco_bar_init (TacoBar *taco_bar)
{
	GtkWidget *hbox;
	GtkWidget *close_button;
	GtkWidget *select_button;
	GtkWidget *select_hbox;
	GtkWidget *arrow;
	GtkWidget *image;

	taco_bar->priv = TACO_BAR_GET_PRIVATE (taco_bar);
	taco_bar->priv->entry_hash = g_hash_table_new (g_direct_hash, g_str_equal);
	taco_bar->priv->active = NULL;
}

/* Public functions */

GtkWidget *
taco_bar_new (void)
{
	GtkWidget *taco_bar;

	taco_bar = g_object_new (TYPE_TACO_BAR, NULL);

	return taco_bar;
}

const char *
taco_bar_get_active (TacoBar *taco_bar)
{
	g_return_val_if_fail (IS_TACO_BAR (taco_bar), NULL);
	g_return_val_if_fail (taco_bar->priv != NULL, NULL);

	return taco_bar->priv->active->id;
}

void
taco_bar_set_active (TacoBar *taco_bar, const char *new_id)
{
	TacoBarEntry *new_active;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (new_page_id != NULL);

	new_active = g_hash_table_lookup (taco_bar->priv->entry_hash, new_id);
	
	g_return_if_fail (new_active != NULL);

	taco_bar_deactivate_current (taco_bar);
	taco_bar_activate (taco_bar, new_active);
}

void
taco_bar_add_entry (TacoBar	*taco_bar,
		   const gchar	*page_id,
		   const gchar	*title,
		   GdkPixmap	*icon,
		   GtkWidget   	*child)
{
	GtkWidget *toggle;
	GtkWidget *toggle_box;
    	TacoBarEntry *new_entry;
	   
	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (page_id != NULL);
	g_return_if_fail (title != NULL);
	g_return_if_fail (GTK_IS_PIXMAP (icon));
	g_return_if_fail (GTK_IS_WIDGET (child));

	// Create the pretty label text and pictures and stuff
	toggle_box = gtk_hbox_new (FALSE, 3);
	gtk_box_pack_start (GTK_BOX (toggle_box), icon);
	gtk_box_pack_end (GTK_BOX (toggle_box), gtk.Label (title));

	// Create the actual toggle button
	toggle = gtk_toggle_button_new ();
	gtk_container_add (GTK_CONTAINER(toggle), toggle_box);
	gtk_widget_show_all (toggle);

	// Create and store a copy of the entry struct
	new_entry = g_malloc (sizeof (TacoBarEntry));
	new_entry->id = page_id;
	new_entry->title = title;
	new_entry->button = toggle;
	new_entry->child = child;
	new_entry->taco_bar = taco_bar;
	g_hash_table_insert (taco_bar->priv->entry_hash, page_id, new_entry);

	// Signal everything up, yo
	g_signal_connect (select_button, "toggled",
			  G_CALLBACK (taco_bar_on_toggle), new_entry);
}

