/*
 * editor.c - Automatically constructs a GUI for editing the
 *            parameters of a ParameterHolder instance.
 *
 * BZEdit
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

#include "editor.h"

static void editor_class_init (EditorClass *klass);
static void editor_init       (Editor      *editor);
static void editor_dispose    (GObject     *object);

GType
editor_get_type (void)
{
  static GType editor_type = 0;

  if (!editor_type)
    {
      static const GTypeInfo editor_info =
      {
	sizeof (EditorClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    editor_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (Editor),
	0,                  /* n preallocs */
	(GInstanceInitFunc) editor_init,
      };

      editor_type = g_type_register_static (G_TYPE_OBJECT, "Editor", &editor_info, 0);
    }

  return editor_type;
}

static void
editor_class_init (EditorClass *klass)
{
  GObjectClass *object_class = (GObjectClass*) klass;

  object_class->dispose = editor_dispose;

  glade_init();
}

static void
editor_init (Editor *editor)
{
  editor->xml = glade_xml_new (GLADEDIR "data/bzedit.glade", NULL, NULL);
  editor->window = glade_xml_get_widget (editor->xml, "editor window");

  editor->statusbar = GTK_STATUSBAR(glade_xml_get_widget(editor->xml, "statusbar"));
  editor->editor_status_context = gtk_statusbar_get_context_id(editor->statusbar, "Editor status");
}

static void
editor_dispose (GObject *object)
{
}

Editor*
editor_new (void)
{
  return g_object_new (EDITOR_TYPE, NULL);
}
