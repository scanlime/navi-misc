/*
 * guir.h - the main gui object
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

#include <gnome.h>
#include <glade/glade.h>
#include "../common/xchat.h"

#ifndef __XCHAT_GNOME_GUIR_H__
#define __XCHAT_GNOME_GUIR_H__

G_BEGIN_DECLS

#define GUI_TYPE            (gui_get_type ())
#define GUI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GUI_TYPE, Gui))
#define GUI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass, GUI_TYPE, GuiClass))
#define IS_GUI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GUI_TYPE))
#define IS_GUI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GUI_TYPE))

typedef struct _Gui      Gui;
typedef struct _GuiClass GuiClass;

typedef struct
{
  GladeXML *xml;
  GtkWidget *xtext;
  session *current_session;
} GuiPrivate;

struct _Gui
{
  GObject parent;

  GuiPrivate priv;
};

struct _GuiClass
{
  GObjectClass parent_class;
};

GType gui_get_type (void) G_GNUC_CONST;
Gui*  gui_new      (void);

G_END_DECLS

#endif
