#include "textgui.h"
#include "palette.h"

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
