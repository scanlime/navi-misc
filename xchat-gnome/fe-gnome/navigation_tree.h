#include "gui.h"
#include "../common/xchat.h"

#ifndef XCHAT_GNOME_NAVIGATION_TREE_H
#define XCHAT_GNOME_NAVIGATION_TREE_H

void initialize_navigation_tree();
void navigation_tree_create_new_network_entry(struct session *sess);
void navigation_tree_create_new_channel_entry(struct session *sess);
void navigation_tree_set_channel_name(struct session *sess);
void navigation_tree_set_hilight(struct session *sess);

#endif
