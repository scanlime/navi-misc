%module librsvg
%{
	#include <librsvg.h>
%}

GQuark rsvg_error_quark (void);
extern void (*RsvgSizeFunc) (gint *width, gint *height, gpointer user_data);
extern void rsvg_set_default_dpi (double dpi);
extern RsvgHandle* rsvg_handle_new (void);
extern void rsvg_handle_set_dpi (RsvgHandle *handle, gpointer user_data, GDestroyNotify user_data_destroy);
extern void rsvg_handle_set_size_callback (RsvgHandle *handle, RsvgSizeFunc size_func,
																			gpointer user_data, GDestroyNotify user_data_destroy);
extern gboolean rsvg_handle_write (RsvgHandle *handle, const guchar *buf, gsize count, GError **error);
extern gboolean rsvg_handle_close (RsvgHandle *handle, GError **error);
extern GdkPixbuf* rsvg_handle_get_pixbuf (RsvgHandle *handle);
void rsvg_handle_free (RsvgHandle *handle);
