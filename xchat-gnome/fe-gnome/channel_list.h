#include "../common/xchat.h"
#include <glade/glade.h>

#ifndef XCHAT_GNOME_CHANNEL_LIST_H
#define XCHAT_GNOME_CHANNEL_LIST_H

typedef struct {
	GtkListStore *store;
	GladeXML *xml;
	struct server *server;
} channel_list_window;

gboolean channel_list_exists(server *serv);
void create_channel_list(session *sess);
void channel_list_append(server *serv, char *channel, char *users, char *topic);
void repopulate_channel_list(channel_list_window *win);

#endif
