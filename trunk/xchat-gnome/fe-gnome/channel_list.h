#include "../common/xchat.h"
#include <glade/glade.h>

#ifndef XCHAT_GNOME_CHANNEL_LIST_H
#define XCHAT_GNOME_CHANNEL_LIST_H

typedef struct {
	GtkListStore *store;
	GladeXML *xml;
	struct server *server;
} channel_list_window;

void initialize_channel_lists();
void create_channel_list(session *sess);
void repopulate_channel_list(channel_list_window *win);

#endif
