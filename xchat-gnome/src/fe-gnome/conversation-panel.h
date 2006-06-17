/*
 * conversation-panel.h - Widget encapsulating the conversation panel
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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
#ifndef XCHAT_GNOME_CONVERSATION_PANEL_H
#define XCHAT_GNOME_CONVERSATION_PANEL_H

#include <gtk/gtkhbox.h>
#include "../common/xchat.h"

G_BEGIN_DECLS

typedef struct _ConversationPanel      ConversationPanel;
typedef struct _ConversationPanelClass ConversationPanelClass;
typedef struct _ConversationPanelPriv  ConversationPanelPriv;

#define CONVERSATION_PANEL_TYPE            (conversation_panel_get_type ())
#define CONVERSATION_PANEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONVERSATION_PANEL_TYPE, ConversationPanel))
#define CONVERSATION_PANEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CONVERSATION_PANEL_TYPE, ConversationPanelClass))
#define IS_CONVERSATION_PANEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONVERSATION_PANEL_TYPE))
#define IS_CONVERSATION_PANEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONVERSATION_PANEL_TYPE))

struct _ConversationPanel
{
	GtkHBox                parent;

	ConversationPanelPriv *priv;
};

struct _ConversationPanelClass
{
	GtkHBoxClass parent_class;
};

GType      conversation_panel_get_type        (void) G_GNUC_CONST;
GtkWidget *conversation_panel_new             (void);
void       conversation_panel_update_colors   (ConversationPanel *panel);
void       conversation_panel_add_session     (ConversationPanel *panel, struct session *sess, gboolean focus);
void       conversation_panel_set_current     (ConversationPanel *panel, struct session *sess);
void       conversation_panel_save_current    (ConversationPanel *panel);
void       conversation_panel_clear           (ConversationPanel *panel, struct session *sess);
void       conversation_panel_print           (ConversationPanel *panel, struct session *sess, guchar *text, gboolean indent);
void       conversation_panel_remove_session  (ConversationPanel *panel, struct session *sess);
void       conversation_panel_lastlog         (ConversationPanel *panel, struct session *sess, struct session *lsess, char *sstr);

void       conversation_panel_clear_selection (ConversationPanel *panel);
gpointer   conversation_panel_search          (ConversationPanel *panel, const gchar *text, gpointer start, gboolean casem, gboolean reverse);
void       conversation_panel_page_up         (ConversationPanel *panel);
void       conversation_panel_page_down       (ConversationPanel *panel);
void	   conversation_panel_copy_selection  (ConversationPanel *panel);
void       conversation_panel_queue_tdraw     (ConversationPanel *panel);

G_END_DECLS

#endif
