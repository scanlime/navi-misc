#include "textgui.h"
#include "palette.h"
#include "../common/text.h"

int check_word(GtkWidget *xtext, char *word);
void clicked_word(GtkWidget *xtext, char *word, GdkEventButton *even, session *sess);

void initialize_text_gui() {
	GtkBox *box;

	gui.xtext = gtk_xtext_new(colors, TRUE);
	box = GTK_BOX(glade_xml_get_widget(gui.xml, "main gui vbox"));
	gtk_box_pack_start(box, GTK_WIDGET(gui.xtext), TRUE, TRUE, 0);
	gtk_box_reorder_child(box, GTK_WIDGET(gui.xtext), 1);

	gtk_xtext_set_palette(gui.xtext, colors);
	gtk_xtext_set_max_lines(gui.xtext, 3000);
	gtk_xtext_set_show_separator(gui.xtext, TRUE);
	gtk_xtext_set_indent(gui.xtext, TRUE);
	gtk_xtext_set_max_indent(gui.xtext, 500);
	gtk_xtext_set_thin_separator(gui.xtext, TRUE);
	gtk_xtext_set_wordwrap(gui.xtext, TRUE);
//	gtk_xtext_set_urlcheck_function(gui.xtext, check_url);
//	g_signal_connect(G_OBJECT(gui.xtext), "word_click", G_CALLBACK(clicked_word), NULL);

	if(!gtk_xtext_set_font(gui.xtext, "Bitstream Vera Sans Mono 9"))
		g_print("Failed to open BV Sans Mono font!\n");

	gtk_widget_show_all(GTK_WIDGET(gui.xtext));
}

void text_gui_add_text_buffer(struct session *sess) {
	session_gui *tgui;

	tgui = malloc(sizeof(session_gui));
	tgui->buffer = gtk_xtext_buffer_new(gui.xtext);
	sess->gui = tgui;

	gtk_xtext_buffer_show(gui.xtext, tgui->buffer, TRUE);
	gtk_xtext_set_time_stamp(tgui->buffer, TRUE);
	gui.current_session = sess;
}

void text_gui_print_line(xtext_buffer *buf, unsigned char *text, int len, gboolean indent) {
	int leftlen;
	unsigned char *tab;
	if(len == 0)
		len = 1;

	/* FIXME: do timestamp */
	if(!indent) {
		gtk_xtext_append(buf, text, len);
		return;
	}

	tab = strchr(text, '\t');
	if(tab && tab < (text + len)) {
		leftlen = tab - text;
		gtk_xtext_append_indent(buf, text, leftlen, tab + 1, len - (leftlen + 1));
	} else {
		gtk_xtext_append_indent(buf, 0, 0, text, len);
	}
}

void text_gui_print(xtext_buffer *buf, unsigned char *text, gboolean indent) {
	char *last_text = text;
	int len = 0;

	/* split the text into separate lines */
	while(1) {
		switch(*text) {
		case '\0':
			text_gui_print_line(buf, last_text, len, indent);
			return;
		case '\n':
			text_gui_print_line(buf, last_text, len, indent);
			text++;
			if(*text == '\0')
				return;
			last_text = text;
			len = 0;
			break;
		case ATTR_BEEP:
			*text = ' ';
			gdk_beep();
		default:
			text++;
			len++;
		}
	}
}

void set_nickname(struct server *serv, char *newnick) {
	if(serv == gui.current_session->server) {
		GtkWidget *nick = glade_xml_get_widget(gui.xml, "nickname");
		if(newnick == NULL)
			gtk_label_set_text(GTK_LABEL(nick), serv->nick);
		else
			gtk_label_set_text(GTK_LABEL(nick), newnick);
	}
}

void set_gui_topic(struct session *sess, char *topic) {
	if(sess == gui.current_session) {
		GtkWidget *topicw = glade_xml_get_widget(gui.xml, "topic entry");
		if(topic == NULL)
			if(sess->topic == NULL)
				gtk_entry_set_text(GTK_ENTRY(topicw), "");
			else
				gtk_entry_set_text(GTK_ENTRY(topicw), sess->topic);
		else
			gtk_entry_set_text(GTK_ENTRY(topicw), topic);
	}
}

void clear_buffer(struct session *sess) {
	session_gui *sgui = sess->gui;
	gtk_xtext_clear(sgui->buffer);
}

int check_word(GtkWidget *xtext, char *word) {
	return text_word_check(word);
}

void clicked_word(GtkWidget *xtext, char *word, GdkEventButton *event, session *sess) {
	sess = gui.current_session;

	if(event->button == 1) {
		/* left click */
		return;
	}
	if(event->button == 2) {
		/* middle click */
		return;
	}
	switch(check_word(xtext, word)) {
	case 0:
		/* FIXME: show default context menu */
		return;
	case WORD_URL:
	case WORD_HOST:
		/* FIXME: show url context menu */
		g_print("its a url!\n");
		return;
	case WORD_NICK:
		/* FIXME: show nickname context menu */
		g_print("its a nickname!\n");
		return;
	case WORD_CHANNEL:
		/* FIXME: show nickname context menu */
		g_print("its a channel!\n");
		return;
	case WORD_EMAIL:
		/* FIXME: show nickname context menu */
		g_print("its a email address!\n");
		return;
	case WORD_DIALOG:
		/* FIXME: show nickname context menu */
		g_print("its a nickname dialog!\n");
		return;
	}
}
