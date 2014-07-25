/*
 * editor.h - Automatically constructs a GUI for editing the
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

#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "parameter-editor.h"
#include "scene.h"
#include "view.h"

G_BEGIN_DECLS

#define EDITOR_TYPE            (editor_get_type ())
#define EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDITOR_TYPE, Editor))
#define EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EDITOR_TYPE, EditorClass))
#define IS_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EDITOR_TYPE))
#define IS_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EDITOR_TYPE))

typedef struct _Editor      Editor;
typedef struct _EditorClass EditorClass;

struct _Editor
{
  GObject object;

  GladeXML *xml;
  GtkWidget *window;
  GtkWidget *glarea;
  GtkWidget *eventbox;

  GtkStatusbar *statusbar;
  guint editor_status_context;

  GtkTreeView *element_list;

  Scene *scene;
  View *view;

  SceneObject *selected;
  GtkWidget *pe;
};

struct _EditorClass
{
  GObjectClass parent_class;
};

GType   editor_get_type (void) G_GNUC_CONST;
Editor* editor_new      (void);
void    editor_selected (SceneObject *object, Editor *editor);

G_END_DECLS

#endif
