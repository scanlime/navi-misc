#include "../common/xchat.h"

#ifndef XCHAT_GNOME_CHANNEL_LIST_H
#define XCHAT_GNOME_CHANNEL_LIST_H

typedef struct {
	GtkWidget *window;
	GtkWidget *listview;
	GtkListStore *store;
	struct server *server;
} channel_list_window;

channel_list_window *create_channel_list(session *sess);

#endif
