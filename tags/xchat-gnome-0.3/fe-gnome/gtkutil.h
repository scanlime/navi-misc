#ifndef XCHAT_GNOME_GTKUTIL_H
#define XCHAT_GNOME_GTKUTIL_H

typedef void (*filereqcallback) (void *, char *file);

#define FRF_WRITE 1
#define FRF_MULTIPLE 2
#define FRF_ADDFOLDER 4

void gtkutil_file_req (char *title, void *callback, void *userdata, char *filter, int flags);
void gtkutil_destroy (GtkWidget * igad, GtkWidget * dgad);
GtkWidget *gtkutil_simpledialog (char *msg);
GtkWidget *gtkutil_button (GtkWidget *box, char *stock, char *tip, void *callback, void *userdata, char *labeltext);
void gtkutil_label_new (char *text, GtkWidget * box);
GtkWidget *gtkutil_entry_new (int max, GtkWidget * box, void *callback, gpointer userdata);
void add_tip (GtkWidget * wid, char *text);
void show_and_unfocus (GtkWidget * wid);
void gtkutil_copy_to_clipboard (GtkWidget *widget, GdkAtom selection, const gchar *str);

#endif
