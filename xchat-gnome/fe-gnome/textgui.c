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
	session_gui *gui;

	gui = malloc(sizeof(session_gui));
	sess->gui = gui;
}
