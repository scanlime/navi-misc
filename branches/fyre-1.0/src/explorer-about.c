/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer-about.c - Implements our pretty and horribly overengineered
 *                    about box, using our ScreenSaver widget to
 *                    progressively render a tiny animation.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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
#include "explorer.h"
#include "de-jong.h"
#include "animation.h"
#include "prefix.h"

static void     on_about_activate     (GtkWidget *widget, Explorer *self);
static void     on_about_close        (GtkWidget *widget, Explorer *self);
static gboolean on_about_close_window (GtkWidget *window, GdkEvent *event, Explorer *self);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

void explorer_init_about(Explorer *self)
{
    gchar* text;
    GtkWidget *dialog = glade_xml_get_widget (self->xml, "about_window");

    /* Connect signal handlers
     */
    glade_xml_signal_connect_data(self->xml, "on_about_activate", G_CALLBACK(on_about_activate),     self);
    glade_xml_signal_connect_data(self->xml, "on_about_close",    G_CALLBACK(on_about_close),        self);
    g_signal_connect (G_OBJECT (dialog),     "delete_event",      G_CALLBACK(on_about_close_window), self);

    /* Poke our current version number into the about box */
    text = g_strdup_printf("<span size=\"xx-large\" weight=\"bold\">Fyre %s</span>", VERSION);
    gtk_label_set_markup(GTK_LABEL(glade_xml_get_widget(self->xml, "about_version")), text);
    g_free(text);
}


/************************************************************************************/
/******************************************************************** GUI Callbacks */
/************************************************************************************/

static void on_about_close(GtkWidget *widget, Explorer *self)
{
    GtkWidget *dialog = glade_xml_get_widget(self->xml, "about_window");
    GtkWidget* image_container = glade_xml_get_widget(self->xml, "about_image");

    gtk_widget_hide(dialog);

    /* Make sure to delete our incredibly memory and CPU hungry about image */
    if (self->about_image) {
	/* We need to stop the screensaver before invalidating its window reference */
	screensaver_stop(self->about_image);
	gtk_container_remove(GTK_CONTAINER(image_container), self->about_image->view);
	g_object_unref(self->about_image);
	self->about_image = NULL;
    }

    /* Restart the main animation if appropriate */
    explorer_restore_pause(self);
}

static gboolean on_about_close_window(GtkWidget *window, GdkEvent *event, Explorer *self)
{
    on_about_close(window, self);
    return TRUE;
}

static void on_about_activate(GtkWidget *widget, Explorer *self)
{
    GtkWidget* dialog = glade_xml_get_widget (self->xml, "about_window");
    GtkWidget* image_container = glade_xml_get_widget(self->xml, "about_image");

    /* Pause calculation so the CPU can go toward our wonderful about image :) */
    explorer_force_pause(self);

    if (!self->about_image) {
	IterativeMap* map;
	Animation* animation;

	map = ITERATIVE_MAP(de_jong_new());
	animation = animation_new();

	parameter_holder_set(PARAMETER_HOLDER(map), "size", "300x150");

	/* Load the animation from our datadir, possibly using binreloc */
	animation_load_file(animation, FYRE_DATADIR "/about-box.fa");
	if (!animation_get_length(animation))
	    animation_load_file(animation, BR_DATADIR("/fyre/about-box.fa"));

	if (animation_get_length(animation)) {
	    self->about_image = screensaver_new(map, animation);
	    gtk_container_add(GTK_CONTAINER(image_container), self->about_image->view);
	}

	g_object_unref(map);
	g_object_unref(animation);
    }

    /* Hmm, didn't this about box involve a dialog somewhere? */
    gtk_widget_show_all(dialog);
}

/* The End */
