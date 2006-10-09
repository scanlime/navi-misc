/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * parameter-editor.h - Automatically constructs a GUI for editing the
 *                      parameters of a ParameterHolder instance.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#ifndef __PARAMETER_EDITOR_H__
#define __PARAMETER_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkvbox.h>
#include "parameter-holder.h"

G_BEGIN_DECLS

#define PARAMETER_EDITOR_TYPE            (parameter_editor_get_type ())
#define PARAMETER_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PARAMETER_EDITOR_TYPE, ParameterEditor))
#define PARAMETER_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PARAMETER_EDITOR_TYPE, ParameterEditorClass))
#define IS_PARAMETER_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PARAMETER_EDITOR_TYPE))
#define IS_PARAMETER_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PARAMETER_EDITOR_TYPE))

typedef struct _ParameterEditor      ParameterEditor;
typedef struct _ParameterEditorClass ParameterEditorClass;

struct _ParameterEditor {
    GtkVBox parent;

    ParameterHolder *holder;
    GtkSizeGroup *label_sizegroup;
    gchar *previous_group;

    gboolean suppress_notify;
    gboolean suppress_changed;
};

struct _ParameterEditorClass {
    GtkVBoxClass parent_class;

    void (* parameter_editor) (ParameterEditor *cb);
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType      parameter_editor_get_type(void);
GtkWidget* parameter_editor_new(ParameterHolder *holder);

G_END_DECLS

#endif /* __PARAMETER_EDITOR_H__ */

/* The End */
