#include "main_window.h"
#include "connect_dialog.h"
#include "about.h"
#include "userlist.h"
#include "../common/xchatc.h"
#include "gui.h"

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_irc_quit_menu_activate(GtkWidget *widget, gpointer data);
void on_irc_connect_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_cut_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_copy_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_paste_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_clear_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_preferences_menu_activate(GtkWidget *widget, gpointer data);
void on_insert_color_code_menu_activate(GtkWidget *widget, gpointer data);
void on_network_information_menu_activate(GtkWidget *widget, gpointer data);
void on_network_reconnect_menu_activate(GtkWidget *widget, gpointer data);
void on_network_disconnect_menu_activate(GtkWidget *widget, gpointer data);
void on_network_file_transfers_menu_activate(GtkWidget *widget, gpointer data);
void on_network_channels_menu_activate(GtkWidget *widget, gpointer data);
void on_network_users_menu_activate(GtkWidget *widget, gpointer data);
void on_discussion_save_activate(GtkWidget *widget, gpointer data);
void on_discussion_save_as_activate(GtkWidget *widget, gpointer data);
void on_discussion_leave_activate(GtkWidget *widget, gpointer data);
void on_discussion_find_activate(GtkWidget *widget, gpointer data);
void on_discussion_find_next_activate(GtkWidget *widget, gpointer data);
void on_discussion_clear_window_activate(GtkWidget *widget, gpointer data);
void on_discussion_bans_activate(GtkWidget *widget, gpointer data);
void on_go_previous_network_activate(GtkWidget *widget, gpointer data);
void on_go_next_network_activate(GtkWidget *widget, gpointer data);
void on_go_previous_discussion_activate(GtkWidget *widget, gpointer data);
void on_go_next_discussion_activate(GtkWidget *widget, gpointer data);
void on_help_about_menu_activate(GtkWidget *widget, gpointer data);

void on_text_entry_activate(GtkWidget *widget, GdkEventKey *event, gpointer data);

void initialize_main_window() {
	GtkWidget *entry;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	/* hook up the menus */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", G_CALLBACK(on_irc_connect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "quit1")), "activate", G_CALLBACK(on_irc_quit_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "cut1")), "activate", G_CALLBACK(on_edit_cut_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "copy1")), "activate", G_CALLBACK(on_edit_copy_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "paste1")), "activate", G_CALLBACK(on_edit_paste_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "clear2")), "activate", G_CALLBACK(on_edit_clear_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "settings1")), "activate", G_CALLBACK(on_edit_preferences_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "insert_color_code1")), "activate", G_CALLBACK(on_insert_color_code_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "information1")), "activate", G_CALLBACK(on_network_information_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "reconnect1")), "activate", G_CALLBACK(on_network_reconnect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "disconnect1")), "activate", G_CALLBACK(on_network_disconnect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "downloads1")), "activate", G_CALLBACK(on_network_file_transfers_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "channel_list1")), "activate", G_CALLBACK(on_network_channels_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "users1")), "activate", G_CALLBACK(on_network_users_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "save_transcript1")), "activate", G_CALLBACK(on_discussion_save_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "save_as1")), "activate", G_CALLBACK(on_discussion_save_as_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "leave1")), "activate", G_CALLBACK(on_discussion_leave_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "find1")), "activate", G_CALLBACK(on_discussion_find_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "find_next1")), "activate", G_CALLBACK(on_discussion_find_next_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "clear_window1")), "activate", G_CALLBACK(on_discussion_clear_window_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "ban_list1")), "activate", G_CALLBACK(on_discussion_bans_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "previous_network1")), "activate", G_CALLBACK(on_go_previous_network_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "next_network1")), "activate", G_CALLBACK(on_go_next_network_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "previous1")), "activate", G_CALLBACK(on_go_previous_discussion_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "next1")), "activate", G_CALLBACK(on_go_next_discussion_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "about1")), "activate", G_CALLBACK(on_help_about_menu_activate), NULL);

	gtk_widget_show_all(GTK_WIDGET(gui.main_window));

	entry = glade_xml_get_widget(gui.xml, "text entry");
	g_signal_connect(G_OBJECT(entry), "key-release-event", G_CALLBACK(on_text_entry_activate), NULL);

#ifdef HAVE_GTKSPELL
	gtkspell_new_attach(GTK_TEXT_VIEW(entry), NULL, NULL);
#endif
}

void on_irc_connect_menu_activate(GtkWidget *widget, gpointer data) {
	display_connection_dialog();
}

void on_irc_quit_menu_activate(GtkWidget *widget, gpointer data) {
	xchat_exit();
}

void on_edit_cut_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_copy_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_paste_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_clear_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_preferences_menu_activate(GtkWidget *widget, gpointer data) {
	gtk_widget_show_all(GTK_WIDGET(gui.preferences_dialog));
}

void on_insert_color_code_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_information_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_reconnect_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_disconnect_menu_activate(GtkWidget *widget, gpointer data) {
	session *s = gui.current_session;
	s->server->disconnect(s, TRUE, -1);
}

void on_network_file_transfers_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_channels_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_users_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_save_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_save_as_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_leave_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_find_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_find_next_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_clear_window_activate(GtkWidget *widget, gpointer data) {
	session *s = gui.current_session;
	clear_buffer(s);
}

void on_discussion_bans_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_go_previous_network_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_go_next_network_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_go_previous_discussion_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_go_next_discussion_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_help_about_menu_activate(GtkWidget *widget, gpointer data) {
	show_about_dialog();
}

void on_text_entry_activate(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	GtkTextBuffer *buffer;
	const gchar *entry_text;
	GtkTextIter start, end;

	if(event->keyval == GDK_Return) {
		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		gtk_text_buffer_get_start_iter(buffer, &start);
		gtk_text_buffer_get_end_iter(buffer, &end);
		entry_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
		handle_multiline(gui.current_session, entry_text, TRUE, FALSE);
		gtk_text_buffer_set_text(buffer, "", -1);
	}
}
