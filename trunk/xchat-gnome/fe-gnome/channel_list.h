#include "../common/xchat.h"
#include <glade/glade.h>

#ifndef XCHAT_GNOME_CHANNEL_LIST_H
#define XCHAT_GNOME_CHANNEL_LIST_H

typedef struct {
	GtkListStore *store;
	GladeXML *xml;
	struct server *server;
} channel_list_window;

channel_list_window *create_channel_list(session *sess);

#endif
