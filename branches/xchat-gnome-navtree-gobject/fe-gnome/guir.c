/*
 * guir.c - the main gui object
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#include "guir.h"
#include <gconf/gconf-client.h>
#include "palette.h"
#include "xtext.h"

static void gui_class_init (GuiClass *klass);
static void gui_init       (Gui *gui);
static void gui_dispose    (GObject *object);

GType
gui_get_type (void)
{
  static GType gui_type = 0;
  if (!gui_type)
  {
    static const GTypeInfo gui_info =
    {
      sizeof (GuiClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    gui_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Gui),
      0,                  /* n preallocs */
      (GInstanceInitFunc) gui_init,
    };

    gui_type = g_type_register_static (G_TYPE_OBJECT, "Gui", &gui_info, 0);
  }

  return gui_type;
}

static void
gui_class_init (GuiClass *klass)
{
  GObjectClass *object_class = (GObjectClass*) klass;

  object_class->dispose = gui_dispose;
}

static void
gui_init (Gui *gui)
{
  GtkWidget *frame, *scrollbar;
  GConfClient *client;
  gchar *font;

  gui->priv.xml = glade_xml_new ("xchat-gnome.glade", NULL, NULL);
  if (!gui->priv.xml)
    gui->priv.xml = glade_xml_new (XCHATSHAREDIR "/xchat-gnome.glade", NULL, NULL);
  if (!gui->priv.xml)
    return;
  gui->priv.current_session = FALSE;

  gui->priv.xtext = gtk_xtext_new (colors, TRUE);
  frame     = glade_xml_get_widget (gui->priv.xml, "text area frame");
  scrollbar = glade_xml_get_widget (gui->priv.xml, "text area scrollbar");
  gtk_container_add (GTK_CONTAINER (frame), gui->priv.xtext);
  gtk_range_set_adjustment (GTK_RANGE (scrollbar), GTK_XTEXT (gui->priv.xtext)->adj);

  palette_alloc (gui->priv.xtext);
  gtk_xtext_set_palette           (GTK_XTEXT (gui->priv.xtext), colors);
  gtk_xtext_set_max_lines         (GTK_XTEXT (gui->priv.xtext), 3000);
  gtk_xtext_set_show_separator    (GTK_XTEXT (gui->priv.xtext), TRUE);
  gtk_xtext_set_indent            (GTK_XTEXT (gui->priv.xtext), TRUE);
  gtk_xtext_set_max_indent        (GTK_XTEXT (gui->priv.xtext), 500);
  gtk_xtext_set_thin_separator    (GTK_XTEXT (gui->priv.xtext), TRUE);
  gtk_xtext_set_wordwrap          (GTK_XTEXT (gui->priv.xtext), TRUE);
//  gtk_xtext_set_urlcheck_function (GTK_XTEXT (gui->priv.xtext, check_word);

  client = gconf_client_get_default ();
  font = gconf_client_get_string (client, "/desktop/gnome/interface/monospace_font_name", NULL);
  if (!font)
  {
    font = gconf_client_get_string (client, "/desktop/gnome/interface/font_name", NULL);
    if (!font)
      font = g_strdup ("fixed 11");
  }
  gtk_xtext_set_font (GTK_XTEXT (gui->priv.xtext), font);
  g_object_unref (client);
  g_free (font);

  gtk_widget_show_all (gui->priv.xtext);
}

static void
gui_dispose (GObject *object)
{
  Gui *gui = GUI (object);

  if (gui->priv.xtext)
  {
    g_object_unref (gui->priv.xtext);
    gui->priv.xtext = NULL;
  }
  if (gui->priv.xml)
  {
    g_object_unref (gui->priv.xml);
    gui->priv.xml = NULL;
  }
}

Gui*
gui_new (void)
{
  return GUI (g_object_new (gui_get_type (), NULL));
}
