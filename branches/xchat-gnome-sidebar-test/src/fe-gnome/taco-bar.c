/*
 * Based around Jonathan Blandford's sidebar from evince.  And then the
 * evolution button selecto-tron thingy.
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

#include <gtk/gtk.h>
#include "taco-bar.h"

typedef struct
{
	const gchar *id;
	gint	     index;
	GtkWidget   *label;
	GtkWidget   *child;
	GtkWidget   *icon;
	GtkWidget   *toggle;
	TacoBar     *taco_bar;
} TacoBarPage;

struct _TacoBarPrivate
{
	GtkWidget   *notebook;
	GtkWidget   *top_box;
	GtkWidget   *bottom_box;

	GHashTable  *page_hash;
	
	TacoBarPage *visible_page;
	TacoBarPage *default_page;
};

G_DEFINE_TYPE (TacoBar, taco_bar, GTK_TYPE_VBOX)

#define TACO_BAR_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), TYPE_TACO_BAR, TacoBarPrivate))

/* Private */
static void taco_bar_class_init 	(TacoBarClass *taco_bar_class);
static void taco_bar_init		(TacoBar *taco_bar);
void	    taco_bar_switch_to_page	(TacoBar *taco_bar, TacoBarPage *page);
static void taco_bar_destroy		(GtkObject *object);

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

static void
taco_bar_init (TacoBar *taco_bar)
{
	taco_bar->priv = TACO_BAR_GET_PRIVATE (taco_bar);
	taco_bar->priv->page_hash = g_hash_table_new (g_direct_hash, g_str_equal);

	taco_bar->priv->visible_page = NULL;
	taco_bar->priv->default_page = NULL;

	gtk_box_set_spacing (GTK_BOX (taco_bar), 0);
 
	// Top Box
	taco_bar->priv->top_box = gtk_vbox_new (FALSE, 3);
	gtk_box_pack_start (GTK_BOX (taco_bar), taco_bar->priv->top_box,
			   FALSE, FALSE, 0);
 
	// Notebook
	taco_bar->priv->notebook = gtk_notebook_new ();
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (taco_bar->priv->notebook),
				    FALSE);
	gtk_box_pack_start (GTK_BOX (taco_bar), taco_bar->priv->notebook,
			    TRUE, TRUE, 0);
	
	// Bottom Box
	taco_bar->priv->bottom_box = gtk_vbox_new (FALSE, 3);
	gtk_box_pack_start (GTK_BOX (taco_bar), taco_bar->priv->bottom_box,
			    FALSE, FALSE, 0);
	
	gtk_widget_show (taco_bar->priv->top_box);
	gtk_widget_show (taco_bar->priv->notebook);
	gtk_widget_show (taco_bar->priv->bottom_box);
}

void
taco_bar_switch_to_page (TacoBar *taco_bar, TacoBarPage *page)
{
	GtkWidget *widget;
	TacoBarPage *prev_visible = taco_bar->priv->visible_page;
	
	// This needs to be set now, because unselecting the last button will
	// emit a signal that needs to know it's not active any more.
	taco_bar->priv->visible_page = page;

	// Deselect previous page
	if (prev_visible != NULL)
	{
		widget = prev_visible->toggle;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget),
					      FALSE);
	}

	// Select the new page
	gtk_notebook_set_current_page (GTK_NOTEBOOK (taco_bar->priv->notebook),
				       page->index);
	gtk_toggle_button_set_active  (GTK_TOGGLE_BUTTON (page->toggle), TRUE);
}

// FIXME: taco_bar_destroy leaks like a stick of butter in the Sahara
static void
taco_bar_destroy (GtkObject *object)
{
/*	TacoBar *taco_bar = TACO_BAR (object);

	g_free (taco_bar->priv->current);
	taco_bar->priv->current = NULL;

	if (taco_bar->priv->menu) {
		gtk_menu_detach (GTK_MENU (taco_bar->priv->menu));
		taco_bar->priv->menu = NULL;
	}

	(* GTK_OBJECT_CLASS (taco_bar_parent_class)->destroy) (object);*/
}

/* (Private) Callbacks */
static gboolean	taco_bar_on_button_press_event	(GtkWidget *widget,
						 GdkEventButton *event,
						 TacoBarPage *page);
static void	taco_bar_on_toggle 		(GtkWidget *widget,
						 TacoBarPage *page);

/* Supress toggle-off signals from being emitted for the default button */
static gboolean
taco_bar_on_button_press_event	(GtkWidget *widget,
				 GdkEventButton *event,
				 TacoBarPage *page)
{
	TacoBar *taco_bar = page->taco_bar;

	/* Don't process further? */
	if (page == taco_bar->priv->visible_page &&
	    page == taco_bar->priv->default_page)
		return TRUE;

	return FALSE;
}

static void
taco_bar_on_toggle (GtkWidget *widget,
		    TacoBarPage *page)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	{
		if (page != page->taco_bar->priv->visible_page)
			taco_bar_switch_to_page  (page->taco_bar, page);
	}
	
	/* A toggle signal should be emitted in one of two inactive states:
	 * 	- The button was active, and just got toggled off by the user
	 * 	  directly.  Now activate the default toggle.
	 * 	- The user clicked a different button, and this one has
	 * 	  been deactivated by the signal handler.  Now do nothing.
	 */
	else
		if (page == page->taco_bar->priv->visible_page)
			taco_bar_switch_to_page (page->taco_bar,
						 page->taco_bar->priv->default_page);
}

/* Public functions */
GtkWidget *
taco_bar_new (void)
{
	GtkWidget *taco_bar;

	taco_bar = g_object_new (TYPE_TACO_BAR, NULL);

	return taco_bar;
}

void
taco_bar_set_visible_page (TacoBar *taco_bar, const char *new_id)
{
	TacoBarPage *new_active;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (new_id != NULL);

	// Try to get the page
	new_active = g_hash_table_lookup (taco_bar->priv->page_hash, new_id);
	g_return_if_fail (new_active != NULL);

	taco_bar_switch_to_page (taco_bar, new_active);
	gtk_container_resize_children (GTK_CONTAINER(taco_bar));
}

const char *
taco_bar_get_visible_page (TacoBar *taco_bar)
{
	g_return_val_if_fail (IS_TACO_BAR (taco_bar), NULL);
	g_return_val_if_fail (taco_bar->priv != NULL, NULL);

	return taco_bar->priv->visible_page->id;
}

void
taco_bar_set_label_markup (TacoBar *taco_bar,
			   const char *id,
			   const char *text)
{
	TacoBarPage *page;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (id != NULL);

	// Try to get the page
	page = g_hash_table_lookup (taco_bar->priv->page_hash, id);
	g_return_if_fail (page != NULL);

	// Set the text
	gtk_label_set_markup_with_mnemonic (GTK_LABEL (page->label), text);
}

const char *
taco_bar_get_label_markup (TacoBar *taco_bar, const char *id)
{
	TacoBarPage *page;

	g_return_val_if_fail (IS_TACO_BAR (taco_bar), NULL);
	g_return_val_if_fail (taco_bar->priv != NULL, NULL);
	
	page = g_hash_table_lookup (taco_bar->priv->page_hash, id);
	g_return_val_if_fail (page != NULL, NULL);

	return gtk_label_get_label (GTK_LABEL (page->label));
}

void
taco_bar_set_default_page (TacoBar *taco_bar, const char *new_id)
{
	TacoBarPage *page;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (new_id != NULL);

	// Try to get the page
	page = g_hash_table_lookup (taco_bar->priv->page_hash, new_id);
	g_return_if_fail (page != NULL);

	// Set the page
	taco_bar->priv->default_page = page;
}

const char *
taco_bar_get_default_page (TacoBar *taco_bar)
{
	g_return_val_if_fail (IS_TACO_BAR (taco_bar), NULL);
	g_return_val_if_fail (taco_bar->priv != NULL, NULL);

	return taco_bar->priv->default_page->id;
}

void
taco_bar_set_page_sensitive (TacoBar *taco_bar, const char *id, gboolean state)
{
	TacoBarPage *page;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (id != NULL);

	// Try to get the page
	page = g_hash_table_lookup (taco_bar->priv->page_hash, id);
	g_return_if_fail (page != NULL);

	// Set the page
	gtk_widget_set_sensitive (page->toggle, state);
}

void
taco_bar_toggle_page_state (TacoBar *taco_bar, const char *id)
{
	TacoBarPage *page;
	GtkWidget *widget;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (id != NULL);

	// Try to get the page
	page = g_hash_table_lookup (taco_bar->priv->page_hash, id);
	g_return_if_fail (page != NULL);

	// Set the page
	widget = page->toggle;
	if (!taco_bar_on_button_press_event (widget, NULL, page))
		taco_bar_on_toggle (widget , page);
}

void
taco_bar_pack_page (TacoBar		*taco_bar,
		    const gchar		*page_id,
		    const gchar		*title,
		    GtkWidget		*icon,
		    GtkWidget		*child,
		    TacoBarPlacementType page_type)
{
	GtkWidget *toggle;
	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *pad1, *pad2;
	TacoBarPage *new_page;
	gint index;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (page_id != NULL);
	g_return_if_fail (title != NULL);
	g_return_if_fail (GTK_IS_WIDGET (icon));
	g_return_if_fail (GTK_IS_WIDGET (child));

	// Create & add the button
	toggle = gtk_toggle_button_new ();
	label = gtk_label_new (NULL);
	pad1 = gtk_label_new (NULL);	//FIXME: there must be a better way..
	pad2 = gtk_label_new (NULL);
	gtk_label_set_markup_with_mnemonic (GTK_LABEL (label), title);
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (hbox), pad1, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), icon, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), pad2, TRUE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (toggle), hbox);
	GTK_WIDGET_UNSET_FLAGS (toggle, GTK_CAN_FOCUS);
	gtk_button_set_relief (GTK_BUTTON (toggle), GTK_RELIEF_NONE);

	switch (page_type)
	{
		case TACO_BAR_TOP:
			gtk_box_pack_start (GTK_BOX (taco_bar->priv->top_box),
				    	    toggle, TRUE, TRUE, 0);
			break;

		case TACO_BAR_BOTTOM:
			gtk_box_pack_start (GTK_BOX (taco_bar->priv->bottom_box),
					    toggle, FALSE, FALSE, 0);
			break;
	}

	gtk_widget_show (toggle);
	gtk_widget_show (hbox);
	gtk_widget_show (label);
	gtk_widget_show (icon);
	gtk_widget_show (child);
	gtk_widget_show (pad1);
	gtk_widget_show (pad2);

	// Update the notebook
	index = gtk_notebook_append_page (GTK_NOTEBOOK (taco_bar->priv->notebook), child,
					  gtk_label_new(page_id));

	// Hash page metadata
	new_page = g_malloc (sizeof (TacoBarPage));
	new_page->id = page_id;
	new_page->label = label;
	new_page->toggle = toggle;
	new_page->child = child;
	new_page->index = index;
	new_page->taco_bar = taco_bar;
	g_hash_table_insert (taco_bar->priv->page_hash, page_id, new_page);

	// If this is the first page, set it as visible and default
	if (taco_bar->priv->visible_page == NULL)
		taco_bar_switch_to_page (taco_bar, new_page);	
	if (taco_bar->priv->default_page == NULL)
		taco_bar->priv->default_page = new_page;
	
	// Signal everything up, yo
	g_signal_connect (toggle, "toggled", G_CALLBACK (taco_bar_on_toggle),
			  new_page);
	g_signal_connect (toggle, "button-press-event",
			  G_CALLBACK (taco_bar_on_button_press_event),
			  new_page);
}
