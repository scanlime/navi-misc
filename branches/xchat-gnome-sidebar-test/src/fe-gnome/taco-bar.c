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

#include "taco-bar.h"

typedef struct
{
	char *id;
	char *title;
	GtkWidget *child;
	GtkWidget *toggle;
	GtkWidget *box;
	GtkWidget *alignment;
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
    	// FIXME: Leaks like a stick of butter in the Sahara
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
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (new_active->toggle), 1);
	gtk_widget_set_sensitive (new_active->toggle, FALSE);
//	gtk_box_set_child_packing (GTK_BOX(new_active->box), new_active->child,
//				   TRUE, TRUE, 0, GTK_PACK_START);
	gtk_alignment_set (GTK_ALIGNMENT(new_active->alignment), 0, 0, 1, 1);
	taco_bar->priv->active = new_active;
}

void
taco_bar_deactivate_current (TacoBar *taco_bar)
{
    	TacoBarEntry *old_active = taco_bar->priv->active;
	
	g_return_if_fail (old_active != NULL);

//	gtk_box_set_child_packing (GTK_BOX(old_active->box), old_active->child,
//				   FALSE, FALSE, 0, GTK_PACK_START);
	gtk_alignment_set (GTK_ALIGNMENT(old_active->alignment), 0, 0, 1, 0);
	old_active = taco_bar->priv->active;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (old_active->toggle), 0);
	gtk_widget_set_sensitive (old_active->toggle, TRUE);
}

static void
taco_bar_on_toggle (GtkWidget *widget,
		    TacoBarEntry *entry)
{
	static char lock=0;
    	TacoBar *taco_bar = entry->taco_bar;

	if (lock)
	    return;

	lock = 1;
	g_return_if_fail (entry != taco_bar->priv->active);

	taco_bar_deactivate_current (taco_bar);
	taco_bar_activate (taco_bar, entry);
	gtk_container_resize_children (GTK_CONTAINER(taco_bar));
	lock = 0;
}

static void
taco_bar_init (TacoBar *taco_bar)
{
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
	g_return_if_fail (new_id != NULL);

	new_active = g_hash_table_lookup (taco_bar->priv->entry_hash, new_id);

	g_return_if_fail (new_active != NULL);

	taco_bar_deactivate_current (taco_bar);
	taco_bar_activate (taco_bar, new_active);
	gtk_container_resize_children (GTK_CONTAINER(taco_bar));
}

void
taco_bar_add_entry (TacoBar	*taco_bar,
		   const gchar	*page_id,
		   const gchar	*title,
		   GtkWidget	*icon,
		   GtkWidget   	*child)
{
	GtkWidget *toggle;
	GtkWidget *vbox;
	GtkWidget *alignment;
    	TacoBarEntry *new_entry;
	   
	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (page_id != NULL);
	g_return_if_fail (title != NULL);
	g_return_if_fail (GTK_IS_WIDGET (icon));
	g_return_if_fail (GTK_IS_WIDGET (child));

	gtk_box_set_homogeneous (GTK_BOX(taco_bar), FALSE);

	// Create the actual toggle button
	toggle = gtk_toggle_button_new_with_label (title);
	gtk_button_set_image (GTK_BUTTON (toggle), icon);
	gtk_button_set_relief (GTK_BUTTON (toggle), GTK_RELIEF_NONE);
	gtk_widget_show (toggle);

	// Create the button + child's container vbox
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(vbox), toggle, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX(vbox), child, TRUE, TRUE, 0);

	// Align it up, yeah!
	alignment = gtk_alignment_new (0, 0, 1, 1);
	gtk_container_add (GTK_CONTAINER(alignment), vbox);

	// Finish packing
	gtk_box_pack_start (GTK_BOX(taco_bar), alignment, FALSE, TRUE, 0);

	// Only the button should be visible at first
	gtk_widget_show_all (toggle);

	// Create and store a copy of the entry struct
	new_entry = g_malloc (sizeof (TacoBarEntry));
	new_entry->id = page_id;
	new_entry->title = title;
	new_entry->toggle = toggle;
	new_entry->child = child;
	new_entry->box = vbox;
	new_entry->alignment = alignment;
	new_entry->taco_bar = taco_bar;
	g_hash_table_insert (taco_bar->priv->entry_hash, page_id, new_entry);

	// Signal everything up, yo
	g_signal_connect (toggle, "toggled", G_CALLBACK (taco_bar_on_toggle), new_entry);
}

