#include "gui.h"

#ifndef XCHAT_GNOME_USERLIST_H
#define XCHAT_GNOME_USERLIST_H

void initialize_userlist();
void userlist_insert(session *sess, struct User *newuser, int row, int sel);

#endif
