#include "gui.h"
#include "textgui.h"

#ifndef XCHAT_GNOME_USERLIST_H
#define XCHAT_GNOME_USERLIST_H

void initialize_userlist();
void create_userlist(session *sess);
void userlist_insert(session *sess, struct User *newuser, int row, int sel);
gboolean userlist_remove(session *sess, struct User *user);
void userlist_change(session *sess, struct User *user);
void userlist_display(session_gui *sess);

#endif
