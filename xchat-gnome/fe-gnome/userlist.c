#include "userlist.h"
#include "pixmaps.h"
#include "textgui.h"
#include "../common/xchat.h"
#include "../common/userlist.h"

GdkPixbuf *get_user_icon(struct server *serv, struct User *user) {
	char *pre;
	int level;

	if(!user)
		return NULL;

	switch(user->prefix[0]) {
	case '\0': return NULL;
	case '@': return pix_op;
	case '%': return pix_hop;
	case '+': return pix_voice;
	}

	/* find out how many levels above Operator this user is */
	pre = strchr(serv->nick_prefixes, '@');
	if(pre && pre != serv->nick_prefixes) {
		pre--;
		level = 0;
		while(1) {
			if(pre[0] == user->prefix[0]) {
				switch(level) {
					case 0: return pix_red; /* 1 level */
					case 1: return pix_purple; /* 2 levels */
				}
				break; /* 3+, no icons */
			}
			level++;
			if(pre == serv->nick_prefixes)
				break;
			pre--;
		}
	}
}

void initialize_userlist() {
	GtkWidget *userlist_view;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;

	userlist_view = glade_xml_get_widget(gui.xml, "userlist");

	icon_renderer = gtk_cell_renderer_pixbuf_new();
	icon_column = gtk_tree_view_column_new_with_attributes("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(userlist_view), icon_column);
	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(userlist_view), text_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(userlist_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	/* FIXME: signale */
}

void create_userlist(session *sess) {
	GtkListStore *store;
	GtkWidget *treeview;
	session_gui *s;

	treeview = glade_xml_get_widget(gui.xml, "userlist");

	store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER);
	s = sess->gui;
	s->userlist_model = GTK_TREE_MODEL(store);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
}

void userlist_insert(session *sess, struct User *newuser, int row, int sel) {
	GtkWidget *userlist_view;
	GtkListStore *store;
	GdkPixbuf *pix;
	GtkTreeIter iter;
	session_gui *s;

	userlist_view = glade_xml_get_widget(gui.xml, "userlist");
	s = sess->gui;
	store = GTK_LIST_STORE(s->userlist_model);

	pix = get_user_icon(sess->server, newuser);

	gtk_list_store_insert(store, &iter, row);
	gtk_list_store_set(store, &iter, 0, pix, 1, newuser->nick, 2, newuser, -1);
	/* FIXME: colors, away status, icons, selection */
}

void userlist_display(session_gui *sess) {
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "userlist");
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), sess->userlist_model);
}
