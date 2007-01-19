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

#include <gtk/gtk.h>
#include "taco-bar.h"

typedef struct
{
	const gchar *id;
	const gchar *title;
	gint	     index;
	GtkWidget   *child;
	GtkWidget   *icon;
	GtkWidget   *toggle;
	TacoBar     *taco_bar;
	gboolean     active;
} TacoBarPage;

struct _TacoBarPrivate
{
	GtkWidget   *notebook;
	GtkWidget   *top_box;
	GtkWidget   *bottom_box;

	GHashTable  *page_hash;
	
	TacoBarPage *active;
	TacoBarPage *default_page;
};

G_DEFINE_TYPE (TacoBar, taco_bar, GTK_TYPE_VBOX)

#define TACO_BAR_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), TYPE_TACO_BAR, TacoBarPrivate))

/* Private */
static void taco_bar_class_init (TacoBarClass *taco_bar_class);
static void taco_bar_init	(TacoBar *taco_bar);
void	    taco_bar_activate	(TacoBar *taco_bar, TacoBarPage *new_active);
void	    taco_bar_deactivate_current (TacoBar *taco_bar);
static void taco_bar_destroy	(GtkObject *object);

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

	taco_bar->priv->active = NULL;
	taco_bar->priv->default_page = NULL;

	gtk_box_set_spacing (GTK_BOX (taco_bar), 0);
 
	// Top Box
	taco_bar->priv->top_box = gtk_vbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (taco_bar), taco_bar->priv->top_box,
			   FALSE, FALSE, 0);
	gtk_widget_show (taco_bar->priv->top_box);
 
	// Notebook
	taco_bar->priv->notebook = gtk_notebook_new ();
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (taco_bar->priv->notebook),
				    FALSE);
	gtk_widget_show (taco_bar->priv->notebook);
	gtk_box_pack_start (GTK_BOX (taco_bar), taco_bar->priv->notebook,
			    TRUE, TRUE, 0);
	
	// Bottom Box
	taco_bar->priv->bottom_box = gtk_vbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (taco_bar), taco_bar->priv->bottom_box,
			    FALSE, FALSE, 0);
	gtk_widget_show (taco_bar->priv->bottom_box);
}

void
taco_bar_activate (TacoBar *taco_bar, TacoBarPage *new_active)
{
	gtk_notebook_set_current_page (GTK_NOTEBOOK (taco_bar->priv->notebook),
				       new_active->index);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (new_active->toggle), TRUE);
	taco_bar->priv->active = new_active;
	new_active->active = TRUE;
}

void
taco_bar_deactivate_current (TacoBar *taco_bar)
{
	TacoBarPage *old_active = taco_bar->priv->active;
	
	g_return_if_fail (old_active != NULL);

	old_active = taco_bar->priv->active;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (old_active->toggle),
				      FALSE);
//	gtk_widget_set_sensitive (old_active->toggle, TRUE);
	gtk_widget_set_state (old_active->toggle, GTK_STATE_NORMAL);
	old_active->active = FALSE;
}

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

/* Callbacks */
static gboolean	taco_bar_button_press_event_cb	(GtkWidget *widget, GdkEventButton *event, TacoBarPage *page);
static void	taco_bar_button_activate_cb (GtkWidget *widget, TacoBarPage *page);

static gboolean
taco_bar_button_press_event_cb	(GtkWidget *widget,
				 GdkEventButton *event,
				 TacoBarPage *page)
{
	TacoBar *taco_bar = page->taco_bar;

	if (widget == taco_bar->priv->active->toggle)
	{
		printf("same widget\n");
		return TRUE;
	}

	if (page == taco_bar->priv->active)
		return FALSE;

	taco_bar_deactivate_current (taco_bar);
	taco_bar_activate (taco_bar, page);
	gtk_container_resize_children (GTK_CONTAINER(taco_bar));

	return FALSE;
}

static void
taco_bar_button_activate_cb (GtkWidget *widget,
			     TacoBarPage *page)
{
	printf("activate\n");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget),
				      widget == page->taco_bar->priv->active->toggle);
	taco_bar_button_press_event_cb (widget, NULL, page);
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
	TacoBarPage *new_active;

	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (new_id != NULL);

	new_active = g_hash_table_lookup (taco_bar->priv->page_hash, new_id);

	g_return_if_fail (new_active != NULL);

	taco_bar_deactivate_current (taco_bar);
	taco_bar_activate (taco_bar, new_active);
	gtk_container_resize_children (GTK_CONTAINER(taco_bar));
}

void
taco_bar_add_page (TacoBar	*taco_bar,
		   const gchar	*page_id,
		   const gchar	*title,
		   GtkWidget	*icon,
		   GtkWidget   	*child,
		   GtkPackType	packing)
{
	GtkWidget *toggle;
	TacoBarPage *new_page;
	gint index;
	   
	g_return_if_fail (IS_TACO_BAR (taco_bar));
	g_return_if_fail (page_id != NULL);
	g_return_if_fail (title != NULL);
	g_return_if_fail (GTK_IS_WIDGET (icon));
	g_return_if_fail (GTK_IS_WIDGET (child));

	// Create & add the button
	toggle = gtk_toggle_button_new_with_label (title);
	gtk_button_set_image (GTK_BUTTON (toggle), icon);
	if (packing == GTK_PACK_START)
		gtk_box_pack_start (GTK_BOX (taco_bar->priv->top_box),
				    toggle, TRUE, TRUE, 0);
	else
	{
		gtk_button_set_relief (GTK_BUTTON (toggle), GTK_RELIEF_NONE);
		gtk_box_pack_start (GTK_BOX (taco_bar->priv->bottom_box),
				    toggle, FALSE, FALSE, 0);
	}
	gtk_widget_show (toggle);
	gtk_widget_show (icon);

	// Update the notebook
	index = gtk_notebook_append_page (GTK_NOTEBOOK (taco_bar->priv->notebook), child,
					  gtk_label_new(page_id));
	gtk_widget_show (child);

	// Create and store a copy of the page struct
	new_page = g_malloc (sizeof (TacoBarPage));
	new_page->id = page_id;
	new_page->title = title;
	new_page->toggle = toggle;
	new_page->child = child;
	new_page->index = index;
	new_page->taco_bar = taco_bar;
	new_page->active = FALSE;
	g_hash_table_insert (taco_bar->priv->page_hash, page_id, new_page);

	if (taco_bar->priv->active == NULL)
	{
		printf("first page is %s\n", title);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (toggle),TRUE);
		
		taco_bar->priv->active = new_page;
		taco_bar->priv->default_page = new_page;
	}
	
	// Signal everything up, yo
	g_signal_connect (toggle, "activate", G_CALLBACK (taco_bar_button_activate_cb), new_page);
	g_signal_connect (toggle, "button-press-event",
			  G_CALLBACK (taco_bar_button_press_event_cb),
			  new_page);
}
