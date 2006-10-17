/* X-Chat
 * Copyright (C) 1998 Peter Zelezny.
 * Copyright (C) 2006 xchat-gnome team.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 * =========================================================================
 *
 * xtext, the text widget used by X-Chat.
 * By Peter Zelezny <zed@xchat.org>.
 *
 */

#define GDK_MULTIHEAD_SAFE

#define MARGIN 2						/* dont touch. */
#define REFRESH_TIMEOUT 20
#define WORDWRAP_LIMIT 24

#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkselection.h>
#include <gtk/gtkclipboard.h>
#include <gtk/gtkversion.h>
#include <gtk/gtkwindow.h>

#ifdef USE_XLIB
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

#ifdef USE_MMX
#include "mmx_cmod.h"
#endif

#include "xtext.h"

#define charlen(str) g_utf8_skip[*(guchar *)(str)]

#ifdef WIN32
#include <windows.h>
#include <gdk/gdkwin32.h>
#endif

#include "xg-marshal.h"

/* is delimiter */
#define is_del(c) \
	(c == ' ' || c == '\n' || c == ')' || c == '(' || \
	 c == '>' || c == '<' || c == ATTR_RESET || c == ATTR_BOLD || c == 0)

/* force scrolling off */
#define dontscroll(buf) (buf)->last_pixel_pos = 0x7fffffff

static GtkWidgetClass *parent_class = NULL;

struct textentry
{
	struct textentry *next;
	struct textentry *prev;
	unsigned char *str;
	time_t stamp;
	/* FIXME pack these together, saving [11/2]*4 bytes */
	gint16 str_width;
	gint16 str_len;
	gint16 mark_start;
	gint16 mark_end;
	gint16 indent;
	gint16 left_len;
	gint16 lines_taken;
#define RECORD_WRAPS 4
	guint16 wrap_offset[RECORD_WRAPS];
	unsigned int mb:1;	/* is multibyte? */
};

#define XTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), XTEXT_TYPE, XTextPriv))
struct _XTextPriv
{
	/*** Property data ***/

	/* Size of the scrollback buffer */
	int max_lines;

	/* Opacity value, between 0 and 255 */
	int tint;

	/*** Internal state ***/

	/* Used for tracking window moves for fake transparency */
	int last_win_x;
	int last_win_y;

	/* Current rendering color */
	int foreground_color;
	int background_color;

	/* Used to check whether we can use true transparency */
	gboolean has_argb_visual;
	gboolean has_composite;

	/* Source IDs */
	guint refresh_source;
	guint add_source;

	/* Scroll adjustment */
	GtkAdjustment *adj;

	/* Number of pixels offset from a text line (for smooth scrolling) */
	int pixel_offset;

	/*** Drawing data ***/
	GdkDrawable *draw_buffer;

	GdkGC *bgc;
	GdkGC *fgc;
	GdkGC *light_separator_gc;
	GdkGC *thin_gc;
	GdkGC *marker_gc;

#ifdef USE_SHM
	XShmSegmentInfo shminfo;
	gboolean has_shm;
#endif

	/* Image background.  If this is NULL, use a solid color */
	GdkPixmap *pixmap;

	/* Cursors */
	GdkCursor *ibeam_cursor;
	GdkCursor *hand_cursor;
	GdkCursor *resize_cursor;

	/* Pango data */
	struct {
		PangoFontDescription *font_desc;
		int ascent;
		int descent;
	} *font, pango_font;
	PangoLayout *layout;
};

enum
{
	WORD_CLICK,
	WORD_ENTER,
	WORD_LEAVE,
	LAST_SIGNAL
};


static guint xtext_signals[LAST_SIGNAL];

char *nocasestrstr (const char *text, const char *tofind);	/* util.c */
int xtext_get_stamp_str (time_t, char **);
static void xtext_render_page (XText * xtext);
static void xtext_calc_lines (xtext_buffer *buf, int);
#if defined(USE_XLIB) || defined(WIN32)
static void xtext_load_trans (XText * xtext, gboolean recycle);
static void xtext_free_trans (XText * xtext);
#endif /* defined(USE_XLIB) || defined(WIN32) */
static char *xtext_selection_get_text (XText *xtext, xtext_buffer *buf, gsize *len_ret);
static textentry *xtext_nth (XText *xtext, int line, int *subline);
static void xtext_adjustment_changed (GtkAdjustment * adj, XText * xtext);
static int xtext_render_ents (XText * xtext, textentry *, textentry *);
static void xtext_recalc_widths (xtext_buffer *buf, int);
static void xtext_fix_indent (xtext_buffer *buf);
static int xtext_find_subline (XText *xtext, textentry *ent, int line);
static unsigned char *xtext_strip_color (unsigned char *text, int len, unsigned char *outbuf, int *newlen, int *mb_ret);
static void xtext_update_primary_selection (XText *xtext);

/***********************************************************************************
 * !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!! *
 ***********************************************************************************/

/* Drawing backend */
static void backend_init       (XText *xtext);
static void backend_deinit     (XText *xtext);
static void backend_font_close (XText *xtext);

/* GtkWidget overrides */
static void unrealize (GtkWidget *widget);

/* Signal handlers */
static void screen_changed     (GtkWidget *widget,
                                GdkScreen *screen,
                                gpointer   data);
static void composited_changed (GtkWidget *widget,
                                gpointer   data);

/***********************************************************************************
 * !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!! *
 ***********************************************************************************/


/* some utility functions first */

#ifdef WIN32

static void
win32_draw_bg (XText *xtext, int x, int y, int width, int height)
{
	XTextPriv *priv;
	HDC hdc;
	HWND hwnd;
	HRGN rgn;

	priv = XTEXT_GET_PRIVATE (xtext);

	if (xtext->shaded) {
		gdk_draw_image (priv->draw_buffer, priv->bgc, (GdkImage*)priv->pixmap,
							 x, y, x, y, width, height);
	} else {
		hwnd = GDK_WINDOW_HWND (priv->draw_buffer);
		hdc = GetDC (hwnd);

		rgn = CreateRectRgn (x, y, x + width, y + height);
		SelectClipRgn (hdc, rgn);

		PaintDesktop (hdc);

		ReleaseDC (hwnd, hdc);
		DeleteObject (rgn);
	}
}

static void
xtext_draw_bg (XText *xtext, int x, int y, int width, int height)
{
	XTextPriv *priv;

	priv = XTEXT_GET_PRIVATE (xtext);

	if (xtext->transparent) {
		win32_draw_bg (xtext, x, y, width, height);
	} else {
		gdk_draw_rectangle (priv->draw_buffer, priv->bgc, 1, x, y, width, height);
	}
}

#else /* WIN32 */

static void
xtext_draw_bg (XText *xtext, int x, int y, int width, int height)
{
	XTextPriv *priv;

	priv = XTEXT_GET_PRIVATE (xtext);
	gdk_draw_rectangle (priv->draw_buffer, priv->bgc, 1, x, y, width, height);
}

#endif /* WIN32 */

/* ======================================= */
/* ============ PANGO BACKEND ============ */
/* ======================================= */

static PangoFontDescription *
backend_font_open_real (char *name)
{
	PangoFontDescription *font;

	font = pango_font_description_from_string (name);
	if (font && pango_font_description_get_size (font) == 0) {
		pango_font_description_free (font);
		font = pango_font_description_from_string ("sans 11");
	}

	if (!font) {
		font = pango_font_description_from_string ("sans 11");
	}

	return font;
}

static void
backend_font_open (XText *xtext, char *name)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	PangoLanguage *lang;
	PangoContext *context;
	PangoFontMetrics *metrics;

	priv->font = &priv->pango_font;
	priv->font->font_desc = backend_font_open_real (name);
	if (!priv->font->font_desc) {
		priv->font = NULL;
		return;
	}

	backend_init (xtext);
	pango_layout_set_font_description (priv->layout, priv->font->font_desc);

	/* vte does it this way */
	context = gtk_widget_get_pango_context (GTK_WIDGET (xtext));
	lang = pango_context_get_language (context);
	metrics = pango_context_get_metrics (context, priv->font->font_desc, lang);
	priv->font->ascent = pango_font_metrics_get_ascent (metrics) / PANGO_SCALE;
	priv->font->descent = pango_font_metrics_get_descent (metrics) / PANGO_SCALE;
	pango_font_metrics_unref (metrics);
}

static int
backend_get_text_width (XText *xtext, guchar *str, int len, int is_mb)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int width;

	if (*str == 0) {
		return 0;
	}

	pango_layout_set_text (priv->layout, (const char *) str, len);
	pango_layout_get_pixel_size (priv->layout, &width, NULL);

	return width;
}

inline static int
backend_get_char_width (XText *xtext, unsigned char *str, int *mbl_ret)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int width;

	*mbl_ret = charlen (str);
	pango_layout_set_text (priv->layout, (const char *) str, *mbl_ret);
	pango_layout_get_pixel_size (priv->layout, &width, NULL);

	return width;
}

/* simplified version of gdk_draw_layout_line_with_colors() */

static void
xtext_draw_layout_line (GdkDrawable      *drawable,
                        GdkGC            *gc,
                        gint              x,
                        gint              y,
                        PangoLayoutLine  *line)
{
	GSList *tmp_list = line->runs;
	PangoRectangle logical_rect;
	gint x_off = 0;

	while (tmp_list) {
		PangoLayoutRun *run = tmp_list->data;

		pango_glyph_string_extents (run->glyphs, run->item->analysis.font, NULL, &logical_rect);

		gdk_draw_glyphs (drawable, gc, run->item->analysis.font, x + x_off / PANGO_SCALE, y, run->glyphs);

		x_off += logical_rect.width;
		tmp_list = tmp_list->next;
	}
}

static void
backend_draw_text (XText *xtext, int dofill, GdkGC *gc, int x, int y,
                   char *str, int len, int str_width, int is_mb)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	GdkGCValues val;
	GdkColor col;
	PangoLayoutLine *line;

	pango_layout_set_text (priv->layout, str, len);

	if (dofill) {
#ifdef WIN32
		if (xtext->transparent && !xtext->backcolor) {
			win32_draw_bg (xtext, x, y - xtext->font->ascent, str_width, xtext->fontsize);
		} else {
			gdk_gc_get_values (gc, &val);
			col.pixel = val.background.pixel;
			gdk_gc_set_foreground (gc, &col);
			gdk_draw_rectangle (priv->draw_buffer, gc, 1, x, y - xtext->font->ascent, str_width, xtext->fontsize);
			col.pixel = val.foreground.pixel;
			gdk_gc_set_foreground (gc, &col);
		}
#else
		gdk_gc_get_values (gc, &val);
		col.pixel = val.background.pixel;
		gdk_gc_set_foreground (gc, &col);
		gdk_draw_rectangle (priv->draw_buffer, gc, 1, x, y - priv->font->ascent, str_width, xtext->fontsize);
		col.pixel = val.foreground.pixel;
		gdk_gc_set_foreground (gc, &col);
#endif /* WIN32 */
	}

	line = pango_layout_get_lines (priv->layout)->data;

	xtext_draw_layout_line (priv->draw_buffer, gc, x, y, line);

	if (xtext->bold) {
		xtext_draw_layout_line (priv->draw_buffer, gc, x + 1, y, line);
	}

}

static void
xtext_set_fg (XText *xtext, GdkGC *gc, int index)
{
	GdkColor col;

	col.pixel = xtext->palette[index];
	gdk_gc_set_foreground (gc, &col);
}

static void
xtext_set_bg (XText *xtext, GdkGC *gc, int index)
{
	GdkColor col;

	col.pixel = xtext->palette[index];
	gdk_gc_set_background (gc, &col);
}

static void
xtext_init (XText * xtext)
{
	XTextPriv *priv;

	priv = XTEXT_GET_PRIVATE (xtext);

	xtext->scroll_tag = 0;
	priv->background_color = XTEXT_BG;
	priv->foreground_color = XTEXT_FG;
	xtext->nc = 0;
	xtext->bold = FALSE;
	xtext->underline = FALSE;
	xtext->italics = FALSE;
	xtext->jump_out_offset = 0;
	xtext->jump_in_offset = 0;
	xtext->ts_x = 0;
	xtext->ts_y = 0;
	xtext->clip_x = 0;
	xtext->clip_x2 = 1000000;
	xtext->clip_y = 0;
	xtext->clip_y2 = 1000000;
	xtext->urlcheck_function = NULL;
	xtext->color_paste = FALSE;
	xtext->skip_border_fills = FALSE;
	xtext->skip_stamp = FALSE;
	xtext->render_hilights_only = FALSE;
	xtext->un_hilight = FALSE;
	xtext->dont_render = FALSE;
	xtext->dont_render2 = FALSE;

	xtext->current_word = NULL;

	xtext->buffer = xtext_buffer_new (xtext);
	xtext->orig_buffer = xtext->buffer;

	gtk_widget_set_double_buffered (GTK_WIDGET (xtext), FALSE);

	priv->adj = (GtkAdjustment *) gtk_adjustment_new (0, 0, 1, 1, 1, 1);
	g_object_ref_sink (priv->adj);

	xtext->vc_signal_tag = g_signal_connect (G_OBJECT (priv->adj),
				"value_changed", G_CALLBACK (xtext_adjustment_changed), xtext);

	g_signal_connect (G_OBJECT (xtext), "screen-changed", G_CALLBACK (screen_changed), NULL);
	g_signal_connect (G_OBJECT (xtext), "composited-changed", G_CALLBACK (composited_changed), NULL);
	screen_changed (GTK_WIDGET (xtext), NULL, NULL);
	composited_changed (GTK_WIDGET (xtext), NULL);
}

static void
xtext_adjustment_set (xtext_buffer *buf, int fire_signal)
{
	XText *xtext = buf->xtext;
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	GtkAdjustment *adj = priv->adj;

	if (xtext->buffer == buf) {
		adj->lower = 0;
		adj->upper = buf->num_lines;

		if (adj->upper == 0) {
			adj->upper = 1;
		}

		adj->page_size = (GTK_WIDGET (xtext)->allocation.height - priv->font->descent) / xtext->fontsize;
		adj->page_increment = adj->page_size;

		if (adj->value > adj->upper - adj->page_size) {
			adj->value = adj->upper - adj->page_size;
		}

		if (adj->value < 0) {
			adj->value = 0;
		}

		if (fire_signal) {
			gtk_adjustment_changed (adj);
		}
	}
}

static gint
xtext_adjustment_timeout (XText * xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);

	xtext_render_page (xtext);
	priv->refresh_source = 0;
	return 0;
}

static void
xtext_adjustment_changed (GtkAdjustment * adj, XText * xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);

	if (xtext->buffer->old_value != priv->adj->value) {
		if (priv->adj->value >= priv->adj->upper - priv->adj->page_size) {
			xtext->buffer->scrollbar_down = TRUE;
		} else {
			xtext->buffer->scrollbar_down = FALSE;
		}

		if (priv->adj->value + 1 == xtext->buffer->old_value || priv->adj->value - 1 == xtext->buffer->old_value) {
			/* clicked an arrow? */
			if (priv->refresh_source) {
				g_source_remove (priv->refresh_source);
				priv->refresh_source = 0;
			}
			xtext_render_page (xtext);
		} else {
			if (!priv->refresh_source) {
				priv->refresh_source = g_timeout_add (REFRESH_TIMEOUT, (GSourceFunc) xtext_adjustment_timeout, xtext);
			}
		}
	}
	xtext->buffer->old_value = adj->value;
}

static void
xtext_destroy (GtkObject * object)
{
	XText *xtext = XTEXT (object);
	XTextPriv *priv = XTEXT_GET_PRIVATE (object);

	if (priv->add_source) {
		g_source_remove (priv->add_source);
		priv->add_source = 0;
	}

	if (xtext->scroll_tag) {
		g_source_remove (xtext->scroll_tag);
		xtext->scroll_tag = 0;
	}

	if (priv->refresh_source) {
		g_source_remove (priv->refresh_source);
		priv->refresh_source = 0;
	}

	if (priv->pixmap) {
#if defined(USE_XLIB) || defined(WIN32)
		if (xtext->transparent) {
			xtext_free_trans (xtext);
		} else {
			g_object_unref (priv->pixmap);
		}
#else /* defined(USE_XLIB) || defined(WIN32) */
		g_object_unref (priv->pixmap);
#endif /* defined(USE_XLIB) || defined(WIN32) */
		priv->pixmap = NULL;
	}

	if (priv->font) {
		backend_font_close (xtext);
		priv->font = NULL;
	}

	if (priv->adj) {
		g_signal_handlers_disconnect_matched (G_OBJECT (priv->adj), G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, xtext);
		g_object_unref (G_OBJECT (priv->adj));
		priv->adj = NULL;
	}

	if (priv->bgc) {
		g_object_unref (priv->bgc);
		priv->bgc = NULL;
	}

	if (priv->fgc) {
		g_object_unref (priv->fgc);
		priv->fgc = NULL;
	}

	if (priv->light_separator_gc) {
		g_object_unref (priv->light_separator_gc);
		priv->light_separator_gc = NULL;
	}

	if (priv->thin_gc) {
		g_object_unref (priv->thin_gc);
		priv->thin_gc = NULL;
	}

	if (priv->marker_gc) {
		g_object_unref (priv->marker_gc);
		priv->marker_gc = NULL;
	}

	if (xtext->orig_buffer) {
		xtext_buffer_free (xtext->orig_buffer);
		xtext->orig_buffer = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
xtext_realize (GtkWidget * widget)
{
	XText *xtext;
	XTextPriv *priv;
	GdkWindowAttr attributes;
	GdkGCValues val;
	GdkColor col;
	GdkColormap *cmap;
	GdkDisplay *display;
	gint attributes_mask;

	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	xtext = XTEXT (widget);
	priv = XTEXT_GET_PRIVATE (widget);

	display = gtk_widget_get_display (widget);

	priv->ibeam_cursor = gdk_cursor_new_for_display (display, GDK_XTERM);
	priv->hand_cursor = gdk_cursor_new_for_display (display, GDK_HAND1);
	priv->resize_cursor = gdk_cursor_new_for_display (display, GDK_LEFT_SIDE);

	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.event_mask = gtk_widget_get_events (widget) |
	                        GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK |
	                        GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK |
	                        GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
	                        GDK_LEAVE_NOTIFY_MASK;

	attributes.cursor = priv->ibeam_cursor;

	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP | GDK_WA_CURSOR;

	cmap = gtk_widget_get_colormap (widget);
	attributes.colormap = cmap;
	attributes.visual = gtk_widget_get_visual (widget);

	widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);

	gdk_window_set_user_data (widget->window, widget);

	val.subwindow_mode = GDK_INCLUDE_INFERIORS;
	val.graphics_exposures = 0;

	priv->bgc       = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
	priv->fgc       = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
	priv->light_separator_gc = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
	priv->thin_gc   = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
	priv->marker_gc = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);

	/* for the separator bar (light) */
	col.red = 0xffff; col.green = 0xffff; col.blue = 0xffff;
	gdk_colormap_alloc_color (cmap, &col, FALSE, TRUE);
	gdk_gc_set_foreground (priv->light_separator_gc, &col);

	/* for the separator bar (thinline) */
	col.red = 0x8e38; col.green = 0x8e38; col.blue = 0x9f38;
	gdk_colormap_alloc_color (cmap, &col, FALSE, TRUE);
	gdk_gc_set_foreground (priv->thin_gc, &col);

	/* for the marker bar (marker) */
	col.pixel = xtext->palette[XTEXT_MARKER];
	gdk_gc_set_foreground (priv->marker_gc, &col);

	xtext_set_fg (xtext, priv->fgc, XTEXT_FG);
	xtext_set_bg (xtext, priv->fgc, XTEXT_BG);
	xtext_set_fg (xtext, priv->bgc, XTEXT_BG);

	/* draw directly to window */
	priv->draw_buffer = widget->window;

#if defined(USE_XLIB) || defined(WIN32)
	if (xtext->transparent) {
		xtext_load_trans (xtext, FALSE);
	} else {
		if (priv->pixmap) {
			gdk_gc_set_tile (priv->bgc, priv->pixmap);
			gdk_gc_set_ts_origin (priv->bgc, 0, 0);
			xtext->ts_x = xtext->ts_y = 0;
			gdk_gc_set_fill (priv->bgc, GDK_TILED);
		}
	}
#else /* defined(USE_XLIB) || defined(WIN32) */
	if (priv->pixmap) {
		gdk_gc_set_tile (priv->bgc, priv->pixmap);
		gdk_gc_set_ts_origin (priv->bgc, 0, 0);
		xtext->ts_x = xtext->ts_y = 0;
		gdk_gc_set_fill (priv->bgc, GDK_TILED);
	}
#endif /* defined(USE_XLIB) || defined(WIN32) */

	gdk_window_set_back_pixmap (widget->window, NULL, FALSE);
	col.pixel = xtext->palette[XTEXT_BG];
	gdk_window_set_background (widget->window, &col);
	gdk_window_clear (widget->window);
	widget->style = gtk_style_attach (widget->style, widget->window);

	backend_init (xtext);

	xtext_update_primary_selection (xtext);
}

static void
xtext_size_request (GtkWidget * widget, GtkRequisition * requisition)
{
	requisition->width = 200;
	requisition->height = 90;
}

static void
xtext_size_allocate (GtkWidget * widget, GtkAllocation * allocation)
{
	XText *xtext = XTEXT (widget);
	XTextPriv *priv = XTEXT_GET_PRIVATE (widget);
	int height_only = FALSE;
	int do_trans = TRUE;

	if (allocation->width == xtext->buffer->window_width) {
		height_only = TRUE;
	}

	if (allocation->x == widget->allocation.x &&
	    allocation->y == widget->allocation.y && xtext->avoid_trans) {
		do_trans = FALSE;
	}

	xtext->avoid_trans = FALSE;

	widget->allocation = *allocation;
	if (GTK_WIDGET_REALIZED (widget)) {
		xtext->buffer->window_width = allocation->width;
		xtext->buffer->window_height = allocation->height;

		gdk_window_move_resize (widget->window, allocation->x, allocation->y,
										allocation->width, allocation->height);
		dontscroll (xtext->buffer);	/* force scrolling off */
		if (!height_only) {
			xtext_calc_lines (xtext->buffer, FALSE);
		} else {
			xtext->buffer->pagetop_ent = NULL;
			xtext_adjustment_set (xtext->buffer, FALSE);
		}
#if defined(USE_XLIB) || defined(WIN32)
		if (do_trans && xtext->transparent && xtext->shaded) {
			xtext_free_trans (xtext);
			xtext_load_trans (xtext, FALSE);
		}
#endif /* defined(USE_XLIB) || defined(WIN32) */
		if (xtext->buffer->scrollbar_down) {
			gtk_adjustment_set_value (priv->adj, priv->adj->upper - priv->adj->page_size);
		}
	}
}

void
xtext_selection_clear_full (xtext_buffer *buf)
{
	textentry *ent = buf->text_first;
	while (ent) {
		ent->mark_start = -1;
		ent->mark_end = -1;
		ent = ent->next;
	}
}

static int
xtext_selection_clear (xtext_buffer *buf)
{
	textentry *ent;
	int ret = 0;

	ent = buf->last_ent_start;
	while (ent) {
		if (ent->mark_start != -1) {
			ret = 1;
		}
		ent->mark_start = -1;
		ent->mark_end = -1;
		if (ent == buf->last_ent_end) {
			break;
		}
		ent = ent->next;
	}

	return ret;
}

static int
find_x (XText *xtext, textentry *ent, unsigned char *text, int x, int indent)
{
	int xx = indent;
	int i = 0;
	int col = FALSE;
	int nc = 0;
	unsigned char *orig = text;
	int mbl;

	while (*text) {
		mbl = 1;
		if ((col && isdigit (*text) && nc < 2) || (col && *text == ',' && isdigit (*(text+1)) && nc < 3)) {
			nc++;
			if (*text == ',')
				nc = 0;
			text++;
		} else {
			col = FALSE;
			switch (*text) {
			case ATTR_COLOR:
				col = TRUE;
				nc = 0;
			case ATTR_BEEP:
			case ATTR_RESET:
			case ATTR_REVERSE:
			case ATTR_BOLD:
			case ATTR_UNDERLINE:
			case ATTR_ITALICS:
				text++;
				break;
			default:
				xx += backend_get_char_width (xtext, text, &mbl);
				text += mbl;
				if (xx >= x)
					return i + (orig - ent->str);
			}
		}

		i += mbl;
		if (text - orig >= ent->str_len) {
			return ent->str_len;
		}
	}

	return ent->str_len;
}

static int
xtext_find_x (XText * xtext, int x, textentry * ent, int subline, int line, int *out_of_bounds)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int indent;
	unsigned char *str;

	if (subline < 1) {
		indent = ent->indent;
	} else {
		indent = xtext->buffer->indent;
	}

	if (line > priv->adj->page_size || line < 0) {
		return 0;
	}

	if (xtext->buffer->grid_dirty || line > 255) {
		str = ent->str + xtext_find_subline (xtext, ent, subline);
		if (str >= ent->str + ent->str_len) {
			return 0;
		}
	} else {
		if (xtext->buffer->grid_offset[line] > ent->str_len) {
			return 0;
		}
		str = ent->str + xtext->buffer->grid_offset[line];
	}

	if (x < indent) {
		*out_of_bounds = 1;
		return (str - ent->str);
	}

	*out_of_bounds = 0;

	return find_x (xtext, ent, str, x, indent);
}

static textentry *
xtext_find_char (XText * xtext, int x, int y, int *off, int *out_of_bounds)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent;
	int line;
	int subline;

	line = (y + priv->pixel_offset) / xtext->fontsize;
	ent = xtext_nth (xtext, line + (int)priv->adj->value, &subline);
	if (!ent) {
		return 0;
	}

	if (off) {
		*off = xtext_find_x (xtext, x, ent, subline, line, out_of_bounds);
	}

	return ent;
}

static void
xtext_draw_sep (XText * xtext, int y)
{
	XTextPriv *priv;
	int x, height;
	GdkGC *light;

	priv = XTEXT_GET_PRIVATE (xtext);

	if (y == -1) {
		y = 0;
		height = GTK_WIDGET (xtext)->allocation.height;
	} else {
		height = xtext->fontsize;
	}

	/* draw the separator line */
	if (xtext->buffer->indent) {
		light = priv->light_separator_gc;

		x = xtext->buffer->indent - ((xtext->space_width + 1) / 2);
		if (x < 1) {
			return;
		}

		if (xtext->moving_separator) {
			gdk_draw_line (priv->draw_buffer, light, x, y, x, y + height);
		} else {
			gdk_draw_line (priv->draw_buffer, priv->thin_gc, x, y, x, y + height);
		}
	}
}

static void
xtext_draw_marker (XText * xtext, textentry * ent, int y)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int x, width, render_y;

	if (!xtext->marker) {
		return;
	}

	if (xtext->buffer->marker_pos == ent) {
		render_y = y + priv->font->descent;
	} else if (xtext->buffer->marker_pos == ent->next && ent->next != NULL) {
		render_y = y + priv->font->descent + xtext->fontsize * ent->lines_taken;
	} else {
		return;
	}

	x = 0;
	width = GTK_WIDGET (xtext)->allocation.width;

	gdk_draw_line (priv->draw_buffer, priv->marker_gc, x, render_y, x + width, render_y);

	if (gtk_window_has_toplevel_focus (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (xtext))))) {
		xtext->buffer->marker_seen = TRUE;
	}
}

static void
xtext_paint (GtkWidget *widget, GdkRectangle *area)
{
	XText *xtext = XTEXT (widget);
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent_start, *ent_end;
	int x, y;

#if defined(USE_XLIB) || defined(WIN32)
	if (xtext->transparent) {
		gdk_window_get_origin (widget->window, &x, &y);
		/* update transparency only if it moved */
		if (priv->last_win_x != x || priv->last_win_y != y) {
			priv->last_win_x = x;
			priv->last_win_y = y;
#if !defined(USE_SHM) && !defined(WIN32)
			if (xtext->shaded) {
				xtext_load_trans (xtext, TRUE);
			} else {
				xtext_free_trans (xtext);
				xtext_load_trans (xtext, FALSE);
			}
#else /* !defined(USE_SHM) && !defined(WIN32) */
			xtext_free_trans (xtext);
			xtext_load_trans (xtext, FALSE);
#endif /* !defined(USE_SHM) && !defined(WIN32) */
		}
	}
#endif /* defined(USE_XLIB) || defined(WIN32) */

	if (area->x == 0 && area->y == 0 &&
	    area->height == widget->allocation.height &&
	    area->width == widget->allocation.width) {
		dontscroll (xtext->buffer);	/* force scrolling off */
		xtext_render_page (xtext);
		return;
	}

	ent_start = xtext_find_char (xtext, area->x, area->y, NULL, NULL);
	if (!ent_start) {
		xtext_draw_bg (xtext, area->x, area->y, area->width, area->height);
		goto xit;
	}
	ent_end = xtext_find_char (xtext, area->x + area->width, area->y + area->height, NULL, NULL);
	if (!ent_end) {
		ent_end = xtext->buffer->text_last;
	}

	/* can't set a clip here, because fgc/bgc are used to draw the DB too */
	xtext->clip_x = area->x;
	xtext->clip_x2 = area->x + area->width;
	xtext->clip_y = area->y;
	xtext->clip_y2 = area->y + area->height;

	/* y is the last pixel y location it rendered text at */
	y = xtext_render_ents (xtext, ent_start, ent_end);

	if (y && y < widget->allocation.height && !ent_end->next) {
		GdkRectangle rect;

		rect.x = 0;
		rect.y = y;
		rect.width = widget->allocation.width;
		rect.height = widget->allocation.height - y;

		/* fill any space below the last line that also intersects with the exposure rectangle */
		if (gdk_rectangle_intersect (area, &rect, &rect)) {
			xtext_draw_bg (xtext, rect.x, rect.y, rect.width, rect.height);
		}
	}

	xtext->clip_x = 0;
	xtext->clip_x2 = 1000000;
	xtext->clip_y = 0;
	xtext->clip_y2 = 1000000;

xit:
	x = xtext->buffer->indent - ((xtext->space_width + 1) / 2);
	if (area->x <= x) {
		xtext_draw_sep (xtext, -1);
	}
}

static gboolean
xtext_expose (GtkWidget * widget, GdkEventExpose * event)
{
	xtext_paint (widget, &event->area);
	return FALSE;
}

/* render a selection that has extended or contracted upward */

static void
xtext_selection_up (XText *xtext, textentry *start, textentry *end,
								int start_offset)
{
	/* render all the complete lines */
	if (start->next == end) {
		xtext_render_ents (xtext, end, NULL);
	} else {
		xtext_render_ents (xtext, start->next, end);
	}

	/* now the incomplete upper line */
	if (start == xtext->buffer->last_ent_start) {
		xtext->jump_in_offset = xtext->buffer->last_offset_start;
	} else {
		xtext->jump_in_offset = start_offset;
	}
	xtext_render_ents (xtext, start, NULL);
	xtext->jump_in_offset = 0;
}

/* render a selection that has extended or contracted downward */

static void
xtext_selection_down (XText *xtext, textentry *start, textentry *end,
								  int end_offset)
{
	/* render all the complete lines */
	if (end->prev == start) {
		xtext_render_ents (xtext, start, NULL);
	} else {
		xtext_render_ents (xtext, start, end->prev);
	}

	/* now the incomplete bottom line */
	if (end == xtext->buffer->last_ent_end) {
		xtext->jump_out_offset = xtext->buffer->last_offset_end;
	} else {
		xtext->jump_out_offset = end_offset;
	}
	xtext_render_ents (xtext, end, NULL);
	xtext->jump_out_offset = 0;
}

static void
xtext_selection_render (XText *xtext,
                            textentry *start_ent, int start_offset,
                            textentry *end_ent, int end_offset)
{
	textentry *ent;
	int start, end;

	xtext->skip_border_fills = TRUE;
	xtext->skip_stamp = TRUE;

	/* force an optimized render if there was no previous selection */
	if (xtext->buffer->last_ent_start == NULL && start_ent == end_ent) {
		xtext->buffer->last_offset_start = start_offset;
		xtext->buffer->last_offset_end = end_offset;
		goto lamejump;
	}

	/* mark changed within 1 ent only? */
	if (xtext->buffer->last_ent_start == start_ent && xtext->buffer->last_ent_end == end_ent) {
		/* when only 1 end of the selection is changed, we can really save on rendering */
		if (xtext->buffer->last_offset_start == start_offset || xtext->buffer->last_offset_end == end_offset) {
lamejump:
			ent = end_ent;
			/* figure out where to start and end the rendering */
			if (end_offset > xtext->buffer->last_offset_end) {
				end = end_offset;
				start = xtext->buffer->last_offset_end;
			} else if (end_offset < xtext->buffer->last_offset_end) {
				end = xtext->buffer->last_offset_end;
				start = end_offset;
			} else if (start_offset < xtext->buffer->last_offset_start) {
				end = xtext->buffer->last_offset_start;
				start = start_offset;
				ent = start_ent;
			} else if (start_offset > xtext->buffer->last_offset_start) {
				end = start_offset;
				start = xtext->buffer->last_offset_start;
				ent = start_ent;
			} else {
				/* WORD selects end up here */
				end = end_offset;
				start = start_offset;
			}
		} else {
			/* LINE selects end up here */
			/* so which ent actually changed? */
			ent = start_ent;
			if (xtext->buffer->last_offset_start == start_offset) {
				ent = end_ent;
			}

			end = MAX (xtext->buffer->last_offset_end, end_offset);
			start = MIN (xtext->buffer->last_offset_start, start_offset);
		}

		xtext->jump_out_offset = end;
		xtext->jump_in_offset = start;
		xtext_render_ents (xtext, ent, NULL);
		xtext->jump_out_offset = 0;
		xtext->jump_in_offset = 0;
	} else if (xtext->buffer->last_ent_start == start_ent && xtext->buffer->last_offset_start == start_offset) {
		/* marking downward */
		/* find the range that covers both old and new selection */
		ent = start_ent;
		while (ent) {
			if (ent == xtext->buffer->last_ent_end) {
				xtext_selection_down (xtext, ent, end_ent, end_offset);
				break;
			}
			if (ent == end_ent) {
				xtext_selection_down (xtext, ent, xtext->buffer->last_ent_end, end_offset);
				break;
			}
			ent = ent->next;
		}
	} else if (xtext->buffer->last_ent_end == end_ent && xtext->buffer->last_offset_end == end_offset) {
		/* marking upward */
		ent = end_ent;
		while (ent) {
			if (ent == start_ent) {
				xtext_selection_up (xtext, xtext->buffer->last_ent_start, ent, start_offset);
				break;
			}
			if (ent == xtext->buffer->last_ent_start) {
				xtext_selection_up (xtext, start_ent, ent, start_offset);
				break;
			}
			ent = ent->prev;
		}
	} else {
		/* cross-over mark (stretched or shrunk at both ends) */
		/* unrender the old mark */
		xtext_render_ents (xtext, xtext->buffer->last_ent_start, xtext->buffer->last_ent_end);
		/* now render the new mark, but skip overlaps */
		if (start_ent == xtext->buffer->last_ent_start) {
			/* if the new mark is a sub-set of the old, do nothing */
			if (start_ent != end_ent)
				xtext_render_ents (xtext, start_ent->next, end_ent);
		} else if (end_ent == xtext->buffer->last_ent_end) {
			/* if the new mark is a sub-set of the old, do nothing */
			if (start_ent != end_ent)
				xtext_render_ents (xtext, start_ent, end_ent->prev);
		} else {
			xtext_render_ents (xtext, start_ent, end_ent);
		}
	}

	xtext->buffer->last_ent_start = start_ent;
	xtext->buffer->last_ent_end = end_ent;
	xtext->buffer->last_offset_start = start_offset;
	xtext->buffer->last_offset_end = end_offset;

	xtext->skip_border_fills = FALSE;
	xtext->skip_stamp = FALSE;
}

static void
xtext_selection_draw (XText * xtext, GdkEventMotion * event)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent;
	textentry *ent_end;
	textentry *ent_start;
	int offset_start;
	int offset_end;
	int low_x;
	int low_y;
	int high_x;
	int high_y;
	int tmp;

	if (xtext->select_start_y > xtext->select_end_y) {
		low_x = xtext->select_end_x;
		low_y = xtext->select_end_y;
		high_x = xtext->select_start_x;
		high_y = xtext->select_start_y;
	} else {
		low_x = xtext->select_start_x;
		low_y = xtext->select_start_y;
		high_x = xtext->select_end_x;
		high_y = xtext->select_end_y;
	}

	ent_start = xtext_find_char (xtext, low_x, low_y, &offset_start, &tmp);
	if (!ent_start) {
		if (priv->adj->value != xtext->buffer->old_value) {
			xtext_render_page (xtext);
		}
		return;
	}

	ent_end = xtext_find_char (xtext, high_x, high_y, &offset_end, &tmp);
	if (!ent_end) {
		ent_end = xtext->buffer->text_last;
		if (!ent_end) {
			if (priv->adj->value != xtext->buffer->old_value) {
				xtext_render_page (xtext);
			}
			return;
		}
		offset_end = ent_end->str_len;
	}

	/* marking less than a complete line? */
	/* make sure "start" is smaller than "end" (swap them if need be) */
	if (ent_start == ent_end && offset_start > offset_end) {
		tmp = offset_start;
		offset_start = offset_end;
		offset_end = tmp;
	}

	/* has the selection changed? Dont render unless necessary */
	if (xtext->buffer->last_ent_start == ent_start &&
	    xtext->buffer->last_ent_end == ent_end &&
	    xtext->buffer->last_offset_start == offset_start &&
	    xtext->buffer->last_offset_end == offset_end) {
		return;
	}

	/* set all the old mark_ fields to -1 */
	xtext_selection_clear (xtext->buffer);

	ent_start->mark_start = offset_start;
	ent_start->mark_end = offset_end;

	if (ent_start != ent_end) {
		ent_start->mark_end = ent_start->str_len;
		if (offset_end != 0) {
			ent_end->mark_start = 0;
			ent_end->mark_end = offset_end;
		}

		/* set all the mark_ fields of the ents within the selection */
		ent = ent_start->next;
		while (ent && ent != ent_end) {
			ent->mark_start = 0;
			ent->mark_end = ent->str_len;
			ent = ent->next;
		}
	}

	xtext_selection_render (xtext, ent_start, offset_start, ent_end, offset_end);
}

static gint
xtext_scrolldown_timeout (XText * xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int p_y, win_height;

	gdk_window_get_pointer (GTK_WIDGET (xtext)->window, 0, &p_y, 0);
	gdk_drawable_get_size (GTK_WIDGET (xtext)->window, 0, &win_height);

	if (p_y > win_height && priv->adj->value < (priv->adj->upper - priv->adj->page_size)) {
		priv->adj->value++;
		gtk_adjustment_changed (priv->adj);
		xtext_render_page (xtext);
		return 1;
	}

	xtext->scroll_tag = 0;
	return 0;
}

static gint
xtext_scrollup_timeout (XText * xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int p_y;

	gdk_window_get_pointer (GTK_WIDGET (xtext)->window, 0, &p_y, 0);

	if (p_y < 0 && priv->adj->value > 0.0) {
		priv->adj->value--;
		gtk_adjustment_changed (priv->adj);
		xtext_render_page (xtext);
		return 1;
	}

	xtext->scroll_tag = 0;
	return 0;
}

static void
xtext_selection_update (XText * xtext, GdkEventMotion * event, int p_y)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int win_height;
	int moved;

	gdk_drawable_get_size (GTK_WIDGET (xtext)->window, 0, &win_height);

	/* selecting past top of window, scroll up! */
	if (p_y < 0 && priv->adj->value >= 0) {
		if (!xtext->scroll_tag) {
			xtext->scroll_tag = g_timeout_add (100, (GSourceFunc) xtext_scrollup_timeout, xtext);
		}
		return;
	}

	/* selecting past bottom of window, scroll down! */
	if (p_y > win_height && priv->adj->value < (priv->adj->upper - priv->adj->page_size)) {
		if (!xtext->scroll_tag) {
			xtext->scroll_tag = g_timeout_add (100, (GSourceFunc) xtext_scrolldown_timeout, xtext);
		}
		return;
	}

	moved = (int)priv->adj->value - xtext->select_start_adj;
	xtext->select_start_y -= (moved * xtext->fontsize);
	xtext->select_start_adj = priv->adj->value;
	xtext_selection_draw (xtext, event);
}

static char *
xtext_get_word (XText * xtext, int x, int y, textentry ** ret_ent, int *ret_off, int *ret_len)
{
	textentry *ent;
	int offset;
	unsigned char *str;
	unsigned char *word;
	int len;
	int out_of_bounds = 0;

	ent = xtext_find_char (xtext, x, y, &offset, &out_of_bounds);
	if (ent == NULL || out_of_bounds || offset == ent->str_len || offset < 1) {
		return 0;
	}

	str = ent->str + offset;

	while (!is_del (*str) && str != ent->str) {
		str--;
	}
	word = str + 1;

	len = 0;
	str = word;
	while (!is_del (*str) && len != ent->str_len) {
		str++;
		len++;
	}

	if (len > 0 && word[len-1]=='.') {
		len--;
		str--;
	}

	if (ret_ent) {
		*ret_ent = ent;
	}

	if (ret_off) {
		*ret_off = word - ent->str;
	}

	if (ret_len) {
		*ret_len = str - word;
	}

	return xtext_strip_color (word, len, xtext->scratch_buffer, NULL, NULL);
}

static void
xtext_unrender_hilight (XText *xtext)
{
	xtext->render_hilights_only = TRUE;
	xtext->skip_border_fills = TRUE;
	xtext->skip_stamp = TRUE;
	xtext->un_hilight = TRUE;

	xtext_render_ents (xtext, xtext->hilight_ent, NULL);

	xtext->render_hilights_only = FALSE;
	xtext->skip_border_fills = FALSE;
	xtext->skip_stamp = FALSE;
	xtext->un_hilight = FALSE;
}

static gboolean
xtext_leave_notify (GtkWidget * widget, GdkEventCrossing * event)
{
	XText *xtext = XTEXT (widget);
	XTextPriv *priv = XTEXT_GET_PRIVATE (widget);

	if (xtext->cursor_hand) {
		xtext_unrender_hilight (xtext);
		xtext->hilight_start = -1;
		xtext->hilight_end = -1;
		xtext->cursor_hand = FALSE;
		gdk_window_set_cursor (widget->window, priv->ibeam_cursor);
		xtext->hilight_ent = NULL;
	}

	if (xtext->cursor_resize) {
		xtext_unrender_hilight (xtext);
		xtext->hilight_start = -1;
		xtext->hilight_end = -1;
		xtext->cursor_resize = FALSE;
		gdk_window_set_cursor (widget->window, priv->ibeam_cursor);
		xtext->hilight_ent = NULL;
	}

	return FALSE;
}

static gboolean
xtext_motion_notify (GtkWidget * widget, GdkEventMotion * event)
{
	XText *xtext = XTEXT (widget);
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int tmp, x, y, offset, len, line_x;
	unsigned char *word;
	textentry *word_ent;

	gdk_window_get_pointer (widget->window, &x, &y, 0);

	if (xtext->moving_separator) {
		if (x < (3 * widget->allocation.width) / 5 && x > 15) {
			tmp = xtext->buffer->indent;
			xtext->buffer->indent = x;
			xtext_fix_indent (xtext->buffer);
			if (tmp != xtext->buffer->indent) {
				xtext_recalc_widths (xtext->buffer, FALSE);
				if (xtext->buffer->scrollbar_down) {
					gtk_adjustment_set_value (priv->adj, priv->adj->upper - priv->adj->page_size);
				}
				if (!priv->refresh_source) {
					priv->refresh_source = g_timeout_add (REFRESH_TIMEOUT, (GSourceFunc) xtext_adjustment_timeout, xtext);
				}
			}
		}
		return FALSE;
	}

	if (xtext->button_down) {
		gtk_grab_add (widget);
		xtext->select_end_x = x;
		xtext->select_end_y = y;
		xtext_selection_update (xtext, event, y);
		xtext->hilighting = TRUE;
		return FALSE;
	}

	if (xtext->buffer->indent) {
		line_x = xtext->buffer->indent - ((xtext->space_width + 1) / 2);
		if (line_x == x || line_x == x + 1 || line_x == x - 1) {
			if (!xtext->cursor_resize) {
				gdk_window_set_cursor (GTK_WIDGET (xtext)->window, priv->resize_cursor);
				xtext->cursor_resize = TRUE;
			}
			return FALSE;
		}
	}

	if (xtext->urlcheck_function == NULL) {
		return FALSE;
	}

	word = xtext_get_word (xtext, x, y, &word_ent, &offset, &len);

	if (!word || (xtext->current_word && strcmp (word, xtext->current_word) != 0)) {
		if (xtext->current_word) {
			g_signal_emit (G_OBJECT (xtext), xtext_signals[WORD_LEAVE], 0, xtext->current_word);

			g_free (xtext->current_word);
			xtext->current_word = NULL;
		}
	}

	if (word) {
		if (xtext->urlcheck_function (GTK_WIDGET (xtext), word, len) > 0) {
			if (!xtext->cursor_hand ||
			    xtext->hilight_ent != word_ent ||
			    xtext->hilight_start != offset ||
			    xtext->hilight_end != offset + len) {
				if (!xtext->cursor_hand) {
					gdk_window_set_cursor (GTK_WIDGET (xtext)->window, priv->hand_cursor);
					xtext->cursor_hand = TRUE;
				}

				/* un-render the old hilight */
				if (xtext->hilight_ent) {
					xtext_unrender_hilight (xtext);
				}

				xtext->hilight_ent = word_ent;
				xtext->hilight_start = offset;
				xtext->hilight_end = offset + len;

				xtext->skip_border_fills = TRUE;
				xtext->render_hilights_only = TRUE;
				xtext->skip_stamp = TRUE;

				xtext_render_ents (xtext, word_ent, NULL);

				xtext->skip_border_fills = FALSE;
				xtext->render_hilights_only = FALSE;
				xtext->skip_stamp = FALSE;
			}

			if (!xtext->current_word || strcmp (word, xtext->current_word)) {
				xtext->current_word = g_strdup (word);
				g_signal_emit (G_OBJECT (xtext), xtext_signals[WORD_ENTER], 0, xtext->current_word);
			}

			return FALSE;
		}
	}

	xtext_leave_notify (widget, NULL);

	return FALSE;
}

static void
primary_get_cb (GtkClipboard     *clipboard,
                GtkSelectionData *selection_data,
                guint             info,
                gpointer          data)
{
	XText *xtext = XTEXT (data);
	char *str;
	gsize len;

	str = xtext_selection_get_text (xtext, xtext->selection_buffer, &len);
	if (str) {
		gtk_selection_data_set_text (selection_data, str, len);
		g_free (str);
	}
}

static void
xtext_unselect (XText *xtext)
{
	GtkClipboard *clipboard;

	xtext_buffer *buf = xtext->selection_buffer;

	xtext->skip_border_fills = TRUE;
        xtext->skip_stamp = TRUE;

	xtext->jump_in_offset = buf->last_ent_start->mark_start;
	/* just a single ent was marked? */
	if (buf->last_ent_start == buf->last_ent_end) {
		xtext->jump_out_offset = buf->last_ent_start->mark_end;
		buf->last_ent_end = NULL;
	}

	xtext_selection_clear (xtext->buffer);

	/* FIXME: use jump_out on multi-line selects too! */
	xtext_render_ents (xtext, buf->last_ent_start, buf->last_ent_end);

	xtext->jump_in_offset = 0;
	xtext->jump_out_offset = 0;

	xtext->skip_border_fills = FALSE;
	xtext->skip_stamp = FALSE;

	xtext->buffer->last_ent_start = NULL;
	xtext->buffer->last_ent_end = NULL;

	clipboard = gtk_widget_get_clipboard (GTK_WIDGET (xtext), GDK_SELECTION_PRIMARY);
	if (gtk_clipboard_get_owner (clipboard) == G_OBJECT (xtext)) {
		gtk_clipboard_clear (clipboard);
	}
}

static void
primary_clear_cb (GtkClipboard *clipboard,
		  gpointer      data)
{
	GtkWidget *widget = GTK_WIDGET (data);
	XText *xtext = XTEXT (data);
	xtext_buffer *buf;

	/* Can this happen? */
	if (!xtext->selection_buffer) {
		return;
	}

	if (xtext->selection_buffer != xtext->buffer) {
		xtext_selection_clear (xtext->selection_buffer);
		return;
	}

	buf = xtext->buffer;

	if (buf->last_ent_start == NULL) {
		/* The selection has already been cleared */
		return;
	}

	xtext->jump_in_offset = buf->last_ent_start->mark_start;

	/* just a single ent was marked? */
	if (buf->last_ent_start == buf->last_ent_end) {
		xtext->jump_out_offset = buf->last_ent_start->mark_end;
		buf->last_ent_end = NULL;
	}

	xtext_selection_clear (buf);

	xtext->jump_in_offset = 0;
	xtext->jump_out_offset = 0;

	buf->last_ent_start = NULL;
	buf->last_ent_end = NULL;

	if (GTK_WIDGET_VISIBLE (widget)) {
		gtk_widget_queue_draw (widget);
	}
}

/* Adapted from gtkentry.c */
static void
xtext_update_primary_selection (XText *xtext)
{
	static GtkTargetEntry targets[] = {
		{ "UTF8_STRING", 0, 0 },
		{ "STRING", 0, 0 },
		{ "TEXT",   0, 0 },
		{ "COMPOUND_TEXT", 0, 0 },
		{ "text/plain;charset=utf-8",   0, 0 },
		{ NULL,   0, 0 },
		{ "text/plain", 0, 0 }
	};

	GtkWidget *widget = GTK_WIDGET (xtext);
	GtkClipboard *clipboard;

	if (G_UNLIKELY (targets[5].target == NULL)) {
		static char target[64];
		const char *charset;

		g_get_charset (&charset);
		g_snprintf (target, sizeof (target), "text/plain;charset=%s", charset);
		targets[5].target = target;
	}

	if (!GTK_WIDGET_REALIZED (widget)) {
		return;
	}

	clipboard = gtk_widget_get_clipboard (widget, GDK_SELECTION_PRIMARY);

	if (xtext->buffer->last_ent_start) {
		if (xtext->selection_buffer &&
		    xtext->selection_buffer != xtext->buffer) {
			xtext_selection_clear (xtext->selection_buffer);
		}

		xtext->selection_buffer = xtext->buffer;

		if (!gtk_clipboard_set_with_owner (clipboard, targets, G_N_ELEMENTS (targets),
		    				   primary_get_cb, primary_clear_cb, G_OBJECT (xtext))) {
			primary_clear_cb (clipboard, xtext);
		}
	} else {
		if (gtk_clipboard_get_owner (clipboard) == G_OBJECT (xtext)) {
			gtk_clipboard_clear (clipboard);
		}
	}
}

void
xtext_copy_selection (XText *xtext)
{
	GtkClipboard *clipboard;
	char *str;
	gsize len;

	str = xtext_selection_get_text (xtext, xtext->buffer, &len);
	if (str) {
		clipboard = gtk_widget_get_clipboard (GTK_WIDGET (xtext), GDK_SELECTION_CLIPBOARD);
		gtk_clipboard_set_text (clipboard, str, len);
		g_free (str);
	}
}

static gboolean
xtext_button_release (GtkWidget * widget, GdkEventButton * event)
{
	XText *xtext = XTEXT (widget);
	unsigned char *word;
	int old;

	if (xtext->moving_separator) {
		xtext->moving_separator = FALSE;
		old = xtext->buffer->indent;
		if (event->x < (4 * widget->allocation.width) / 5 && event->x > 15) {
			xtext->buffer->indent = event->x;
		}
		xtext_fix_indent (xtext->buffer);
		if (xtext->buffer->indent != old) {
			xtext_recalc_widths (xtext->buffer, FALSE);
			xtext_adjustment_set (xtext->buffer, TRUE);
			xtext_render_page (xtext);
		} else {
			xtext_draw_sep (xtext, -1);
		}
		return FALSE;
	}

	if (xtext->word_or_line_select) {
		xtext->word_or_line_select = FALSE;
		xtext->button_down = FALSE;
		return FALSE;
	}

	if (event->button == 1) {
		xtext->button_down = FALSE;

		gtk_grab_remove (widget);

		xtext_update_primary_selection (xtext);

		if (xtext->select_start_x == event->x &&
		    xtext->select_start_y == event->y &&
		    xtext->buffer->last_ent_start) {
			xtext_unselect (xtext);
			return FALSE;
		}

		if (!xtext->hilighting) {
			word = xtext_get_word (xtext, event->x, event->y, 0, 0, 0);
			g_signal_emit (G_OBJECT (xtext), xtext_signals[WORD_CLICK], 0, word ? word : NULL, event);
		} else {
			xtext->hilighting = FALSE;
		}
	}


	return FALSE;
}

static gboolean
xtext_button_press (GtkWidget * widget, GdkEventButton * event)
{
	XText *xtext = XTEXT (widget);
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent;
	unsigned char *word;
	int line_x, x, y, offset, len;

	gdk_window_get_pointer (widget->window, &x, &y, 0);

	if (event->button == 3 || event->button == 2) {
		/* right/middle click */
		word = xtext_get_word (xtext, x, y, 0, 0, 0);
		if (word) {
			g_signal_emit (G_OBJECT (xtext), xtext_signals[WORD_CLICK], 0, word, event);
		} else {
			g_signal_emit (G_OBJECT (xtext), xtext_signals[WORD_CLICK], 0, "", event);
		}
		return FALSE;
	}

	if (event->button != 1) {
		/* we only want left button */
		return FALSE;
	}

	if (event->type == GDK_2BUTTON_PRESS) {
		/* WORD select */
		if (xtext_get_word (xtext, x, y, &ent, &offset, &len)) {
			if (len == 0) {
				return FALSE;
			}
			xtext_selection_clear (xtext->buffer);
			ent->mark_start = offset;
			ent->mark_end = offset + len;
			xtext_selection_render (xtext, ent, offset, ent, offset + len);
			xtext->word_or_line_select = TRUE;
			xtext_update_primary_selection (xtext);
		}

		return FALSE;
	}

	if (event->type == GDK_3BUTTON_PRESS) {
		/* LINE select */
		if (xtext_get_word (xtext, x, y, &ent, 0, 0)) {
			xtext_selection_clear (xtext->buffer);
			ent->mark_start = 0;
			ent->mark_end = ent->str_len;
			xtext_selection_render (xtext, ent, 0, ent, ent->str_len);
			xtext->word_or_line_select = TRUE;
			xtext_update_primary_selection (xtext);
		}

		return FALSE;
	}

	/* check if it was a separator-bar click */
	if (xtext->buffer->indent) {
		line_x = xtext->buffer->indent - ((xtext->space_width + 1) / 2);
		if (line_x == x || line_x == x + 1 || line_x == x - 1) {
			xtext->moving_separator = TRUE;
			/* draw the separator line */
			xtext_draw_sep (xtext, -1);
			return FALSE;
		}
	}

	xtext->button_down = TRUE;
	xtext->select_start_x = x;
	xtext->select_start_y = y;
	xtext->select_start_adj = priv->adj->value;

	return FALSE;
}

static char *
xtext_selection_get_text (XText *xtext, xtext_buffer *buf, gsize *len_ret)
{
	textentry *ent;
	char *txt;
	char *pos;
	char *stripped;
	gsize len;
	int first = TRUE;

	if (!buf) {
		return NULL;
	}

	/* first find out how much we need to malloc ... */
	len = 0;
	ent = buf->last_ent_start;
	while (ent) {
		if (ent->mark_start != -1) {
			if (ent->mark_end - ent->mark_start > 0) {
				len += (ent->mark_end - ent->mark_start) + 1;
			} else {
				len++;
			}
		}
		if (ent == buf->last_ent_end) {
			break;
		}
		ent = ent->next;
	}

	if (len < 1) {
		return NULL;
	}

	/* now allocate mem and copy buffer */
	pos = txt = g_malloc (len);
	ent = buf->last_ent_start;
	while (ent) {
		if (ent->mark_start != -1) {
			if (!first) {
				*pos = '\n';
				pos++;
			}
			first = FALSE;
			if (ent->mark_end - ent->mark_start > 0) {
				memcpy (pos, ent->str + ent->mark_start, ent->mark_end - ent->mark_start);
				pos += ent->mark_end - ent->mark_start;
			}
		}
		if (ent == buf->last_ent_end) {
			break;
		}
		ent = ent->next;
	}
	*pos = 0;

	if (xtext->color_paste) {
		stripped = txt;
		len = strlen (txt);
	} else {
		stripped = xtext_strip_color (txt, strlen (txt), NULL, &len, 0);
		g_free (txt);
	}

	*len_ret = len;
	return stripped;
}

/* another program is asking for our selection */

static gboolean
xtext_scroll (GtkWidget *widget, GdkEventScroll *event)
{
	XText *xtext = XTEXT (widget);
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	gfloat new_value;

	if (event->direction == GDK_SCROLL_UP) {
		new_value = priv->adj->value - (priv->adj->page_increment / 10);
		if (new_value < priv->adj->lower) {
			new_value = priv->adj->lower;
		}
		gtk_adjustment_set_value (priv->adj, new_value);
	} else if (event->direction == GDK_SCROLL_DOWN) {
		new_value = priv->adj->value + (priv->adj->page_increment / 10);
		if (new_value > (priv->adj->upper - priv->adj->page_size)) {
			new_value = priv->adj->upper - priv->adj->page_size;
		}
		gtk_adjustment_set_value (priv->adj, new_value);
	}

	return FALSE;
}

static void
xtext_class_init (XTextClass * class)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;
	XTextClass *xtext_class;

	object_class = (GtkObjectClass *) class;
	widget_class = (GtkWidgetClass *) class;
	xtext_class = (XTextClass *) class;

	parent_class = gtk_type_class (gtk_widget_get_type ());

	xtext_signals[WORD_CLICK] =
		g_signal_new ("word_click",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		              G_STRUCT_OFFSET (XTextClass, word_click),
		              NULL, NULL,
		              xg_marshal_VOID__POINTER_POINTER,
		              G_TYPE_NONE,
		              2, G_TYPE_POINTER, G_TYPE_POINTER);
	xtext_signals[WORD_ENTER] =
		g_signal_new ("word_enter",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		              G_STRUCT_OFFSET (XTextClass, word_enter),
		              NULL, NULL,
		              xg_marshal_VOID__POINTER,
		              G_TYPE_NONE,
		              1, G_TYPE_POINTER);
	xtext_signals[WORD_LEAVE] =
		g_signal_new ("word_leave",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		              G_STRUCT_OFFSET (XTextClass, word_leave),
		              NULL, NULL,
		              xg_marshal_VOID__POINTER,
		              G_TYPE_NONE,
		              1, G_TYPE_POINTER);

	object_class->destroy = xtext_destroy;

	widget_class->realize = xtext_realize;
	widget_class->unrealize = unrealize;
	widget_class->size_request = xtext_size_request;
	widget_class->size_allocate = xtext_size_allocate;
	widget_class->button_press_event = xtext_button_press;
	widget_class->button_release_event = xtext_button_release;
	widget_class->motion_notify_event = xtext_motion_notify;
	widget_class->expose_event = xtext_expose;
	widget_class->scroll_event = xtext_scroll;
	widget_class->leave_notify_event = xtext_leave_notify;

	xtext_class->word_click = NULL;

	g_type_class_add_private (class, sizeof (XTextPriv));
}

/* strip MIRC colors and other attribs. */

static unsigned char *
xtext_strip_color (unsigned char *text, int len, unsigned char *outbuf, int *newlen, int *mb_ret)
{
	int nc = 0;
	int i = 0;
	int col = FALSE;
	unsigned char *new_str;
	int mb = FALSE;

	if (outbuf == NULL) {
		new_str = g_malloc (len + 2);
	} else {
		new_str = outbuf;
	}

	while (len > 0) {
		if (*text >= 128) {
			mb = TRUE;
		}

		if ((col && isdigit (*text) && nc < 2) || (col && *text == ',' && isdigit (*(text+1)) && nc < 3)) {
			nc++;
			if (*text == ',')
				nc = 0;
		} else {
			col = FALSE;
			switch (*text) {
			case ATTR_COLOR:
				col = TRUE;
				nc = 0;
				break;
			case ATTR_BEEP:
			case ATTR_RESET:
			case ATTR_REVERSE:
			case ATTR_BOLD:
			case ATTR_UNDERLINE:
			case ATTR_ITALICS:
				break;
			default:
				new_str[i] = *text;
				i++;
			}
		}
		text++;
		len--;
	}

	new_str[i] = 0;

	if (newlen != NULL) {
		*newlen = i;
	}

	if (mb_ret != NULL) {
		*mb_ret = mb;
	}

	return new_str;
}

/* gives width of a string, excluding the mIRC codes */

static int
xtext_text_width (XText *xtext, unsigned char *text, int len, int *mb_ret)
{
	unsigned char *new_buf;
	int new_len, mb;

	new_buf = xtext_strip_color (text, len, xtext->scratch_buffer, &new_len, &mb);

	if (mb_ret) {
		*mb_ret = mb;
	}

	return backend_get_text_width (xtext, new_buf, new_len, mb);
}

/* actually draw text to screen (one run with the same color/attribs) */

static int
xtext_render_flush (XText * xtext, int x, int y, unsigned char *str, int len, GdkGC *gc, int is_mb)
{
	XTextPriv *priv;
	int str_width, dofill;
	GdkDrawable *pix = NULL;
	int dest_x = 0, dest_y = 0;

	priv = XTEXT_GET_PRIVATE (xtext);

	if (xtext->dont_render || len < 1) {
		return 0;
	}

	str_width = backend_get_text_width (xtext, str, len, is_mb);

	if (xtext->dont_render2) {
		return str_width;
	}

	/* roll-your-own clipping (avoiding XftDrawString is always good!) */
	if (x > xtext->clip_x2 || x + str_width < xtext->clip_x) {
		return str_width;
	}
	if (y - priv->font->ascent > xtext->clip_y2 || (y - priv->font->ascent) + xtext->fontsize < xtext->clip_y) {
		return str_width;
	}

	if (xtext->render_hilights_only) {
		if (!xtext->in_hilight) {
			/* is it a hilight prefix? */
			return str_width;
		}
		if (!xtext->un_hilight) {
			/* doing a hilight? no need to draw the text */
			goto dounder;
		}
	}

#ifdef WIN32
	if (!xtext->transparent)
#endif /* WIN32 */
	{
		pix = gdk_pixmap_new (priv->draw_buffer, str_width, xtext->fontsize,
		                      gdk_drawable_get_visual (GTK_WIDGET (xtext)->window)->depth);
		if (pix) {
			dest_x = x;
			dest_y = y - priv->font->ascent;

			gdk_gc_set_ts_origin (priv->bgc, xtext->ts_x - x, xtext->ts_y - dest_y);

			x = 0;
			y = priv->font->ascent;
			priv->draw_buffer = pix;
		}
	}

	dofill = TRUE;

	/* backcolor is always handled by XDrawImageString */
	if (!xtext->backcolor && priv->pixmap) {
		/* draw the background pixmap behind the text - CAUSES FLICKER HERE!! */
		xtext_draw_bg (xtext, x, y - priv->font->ascent, str_width, xtext->fontsize);
		dofill = FALSE;	/* already drawn the background */
	}

	backend_draw_text (xtext, dofill, gc, x, y, str, len, str_width, is_mb);

	if (pix) {
		GdkRectangle clip;
		GdkRectangle dest;

		gdk_gc_set_ts_origin (priv->bgc, xtext->ts_x, xtext->ts_y);
		priv->draw_buffer = GTK_WIDGET (xtext)->window;
		clip.x = xtext->clip_x;
		clip.y = xtext->clip_y;
		clip.width = xtext->clip_x2 - xtext->clip_x;
		clip.height = xtext->clip_y2 - xtext->clip_y;

		dest.x = dest_x;
		dest.y = dest_y;
		dest.width = str_width;
		dest.height = xtext->fontsize;

		if (gdk_rectangle_intersect (&clip, &dest, &dest)) {
			/* dump the DB to window, but only within the clip_x/x2/y/y2 */
			gdk_draw_drawable (priv->draw_buffer, priv->bgc, pix,
			                   dest.x - dest_x, dest.y - dest_y,
			                   dest.x, dest.y, dest.width, dest.height);
		}
		g_object_unref (pix);
	}

	if (xtext->underline) {
dounder:

		if (pix) {
			y = dest_y + priv->font->ascent + 1;
		} else {
			y++;
			dest_x = x;
		}
		/* draw directly to window, it's out of the range of our DB */
		gdk_draw_line (priv->draw_buffer, gc, dest_x, y, dest_x + str_width - 1, y);
	}

	return str_width;
}

static void
xtext_reset (XText * xtext, int mark, int attribs)
{
	XTextPriv *priv;

	priv = XTEXT_GET_PRIVATE (xtext);

	if (attribs) {
		xtext->underline = FALSE;
		xtext->bold = FALSE;
		xtext->italics = FALSE;
	}
	if (!mark) {
		xtext->backcolor = FALSE;
		if (priv->foreground_color != XTEXT_FG) {
			xtext_set_fg (xtext, priv->fgc, XTEXT_FG);
		}
		if (priv->background_color != XTEXT_BG) {
			xtext_set_bg (xtext, priv->fgc, XTEXT_BG);
		}
	}
	priv->foreground_color = XTEXT_FG;
	priv->background_color = XTEXT_BG;
	xtext->parsing_color = FALSE;
	xtext->parsing_backcolor = FALSE;
	xtext->nc = 0;
}

/* render a single line, which WONT wrap, and parse mIRC colors */

static int
xtext_render_str (XText * xtext, int y, textentry * ent,
                      unsigned char *str, int len, int win_width, int indent,
                      int line, int left_only)
{
	GdkGC *gc;
	int i = 0, x = indent, j = 0;
	unsigned char *pstr = str;
	int col_num, tmp;
	int offset;
	int mark = FALSE;
	int ret = 1;
	XTextPriv *priv;

	priv = XTEXT_GET_PRIVATE (xtext);

	xtext->in_hilight = FALSE;

	offset = str - ent->str;

	if (line < 255 && line >= 0) {
		xtext->buffer->grid_offset[line] = offset;
	}

	gc = priv->fgc;

	if (ent->mark_start != -1 && ent->mark_start <= i + offset && ent->mark_end > i + offset) {
		xtext_set_bg (xtext, gc, XTEXT_MARK_BG);
		xtext_set_fg (xtext, gc, XTEXT_MARK_FG);
		xtext->backcolor = TRUE;
		mark = TRUE;
	}

	if (xtext->hilight_ent == ent &&
	    xtext->hilight_start <= i + offset && xtext->hilight_end > i + offset) {
		if (!xtext->un_hilight) {
			xtext->underline = TRUE;
		}
		xtext->in_hilight = TRUE;
	}

	if (!xtext->skip_border_fills && !xtext->dont_render) {
		/* draw background to the left of the text */
		if (str == ent->str && indent > MARGIN && xtext->buffer->time_stamp) {
			/* don't overwrite the timestamp */
			if (indent > xtext->stamp_width) {
				xtext_draw_bg (xtext, xtext->stamp_width, y - priv->font->ascent,
				               indent - xtext->stamp_width, xtext->fontsize);
			}
		} else {
			/* fill the indent area with background gc */
			if (indent >= xtext->clip_x) {
				xtext_draw_bg (xtext, 0, y - priv->font->ascent,
				               MIN (indent, xtext->clip_x2), xtext->fontsize);
			}
		}
	}

	if (xtext->jump_in_offset > 0 && offset < xtext->jump_in_offset) {
		xtext->dont_render2 = TRUE;
	}

	while (i < len) {
		if (xtext->hilight_ent == ent && xtext->hilight_start == (i + offset)) {
			x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
			pstr += j;
			j = 0;
			if (!xtext->un_hilight) {
				xtext->underline = TRUE;
			}

			xtext->in_hilight = TRUE;
		}

		if ((xtext->parsing_color && isdigit (str[i]) && xtext->nc < 2) ||
		    (xtext->parsing_color && str[i] == ',' && isdigit (str[i+1]) && xtext->nc < 3)) {
			pstr++;
			if (str[i] == ',') {
				xtext->parsing_backcolor = TRUE;
				if (xtext->nc) {
					xtext->num[xtext->nc] = 0;
					xtext->nc = 0;
					col_num = atoi (xtext->num);
					if (col_num == 99) {
						/* mIRC lameness */
						col_num = XTEXT_FG;
					} else {
						col_num = col_num % XTEXT_MIRC_COLORS;
					}
					priv->foreground_color = col_num;
					if (!mark) {
						xtext_set_fg (xtext, gc, col_num);
					}
				}
			} else {
				xtext->num[xtext->nc] = str[i];
				if (xtext->nc < 7) {
					xtext->nc++;
				}
			}
		} else {
			if (xtext->parsing_color) {
				xtext->parsing_color = FALSE;
				if (xtext->nc) {
					xtext->num[xtext->nc] = 0;
					xtext->nc = 0;
					col_num = atoi (xtext->num);
					if (xtext->parsing_backcolor) {
						if (col_num == 99) {
							/* mIRC lameness */
							col_num = XTEXT_BG;
						} else {
							col_num = col_num % XTEXT_MIRC_COLORS;
						}
						if (col_num == XTEXT_BG) {
							xtext->backcolor = FALSE;
						} else {
							xtext->backcolor = TRUE;
						}
						if (!mark) {
							xtext_set_bg (xtext, gc, col_num);
						}
						priv->background_color = col_num;
					} else {
						if (col_num == 99) {
							/* mIRC lameness */
							col_num = XTEXT_FG;
						} else {
							col_num = col_num % XTEXT_MIRC_COLORS;
						}
						if (!mark) {
							xtext_set_fg (xtext, gc, col_num);
						}
						priv->foreground_color = col_num;
					}
					xtext->parsing_backcolor = FALSE;
				} else {
					/* got a \003<non-digit>... i.e. reset colors */
					x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
					pstr += j;
					j = 0;
					xtext_reset (xtext, mark, FALSE);
				}
			}

			switch (str[i]) {
			case '\n':
			/*case ATTR_BEEP:*/
				break;
			case ATTR_REVERSE:
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				pstr += j + 1;
				j = 0;
				tmp = priv->foreground_color;
				priv->foreground_color = priv->background_color;
				priv->background_color = tmp;
				if (!mark) {
					xtext_set_fg (xtext, gc, priv->foreground_color);
					xtext_set_bg (xtext, gc, priv->background_color);
				}
				if (priv->background_color != XTEXT_BG) {
					xtext->backcolor = TRUE;
				} else {
					xtext->backcolor = FALSE;
				}
				break;
			case ATTR_BOLD:
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				xtext->bold = !xtext->bold;
				pstr += j + 1;
				j = 0;
				break;
			case ATTR_UNDERLINE:
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				xtext->underline = !xtext->underline;
				pstr += j + 1;
				j = 0;
				break;
			case ATTR_ITALICS:
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				xtext->italics = !xtext->italics;
				pstr += j + 1;
				j = 0;
				break;
			case ATTR_RESET:
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				pstr += j + 1;
				j = 0;
				xtext_reset (xtext, mark, !xtext->in_hilight);
				break;
			case ATTR_COLOR:
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				xtext->parsing_color = TRUE;
				pstr += j + 1;
				j = 0;
				break;
			default:
				tmp = charlen (str + i);
				/* invalid utf8 safe guard */
				if (tmp + i > len) {
					tmp = len - i;
				}
				j += tmp;	/* move to the next utf8 char */
			}
		}
		i += charlen (str + i);	/* move to the next utf8 char */
		/* invalid utf8 safe guard */
		if (i > len) {
			i = len;
		}

		/* Separate the left part, the space and the right part
		   into separate runs, and reset bidi state inbetween.
		   Perform this only on the first line of the message.
                */
		if (offset == 0) {
			/* we've reached the end of the left part? */
			if ((pstr-str)+j == ent->left_len) {
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				pstr += j;
				j = 0;
			} else if ((pstr-str)+j == ent->left_len+1) {
				x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
				pstr += j;
				j = 0;
			}
		}

		/* have we been told to stop rendering at this point? */
		if (xtext->jump_out_offset > 0 && xtext->jump_out_offset <= (i + offset)) {
			xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
			ret = 0;	/* skip the rest of the lines, we're done. */
			j = 0;
			break;
		}

		if (xtext->jump_in_offset > 0 && xtext->jump_in_offset == (i + offset)) {
			x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
			pstr += j;
			j = 0;
			xtext->dont_render2 = FALSE;
		}

		if (xtext->hilight_ent == ent && xtext->hilight_end == (i + offset)) {
			x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
			pstr += j;
			j = 0;
			xtext->underline = FALSE;
			xtext->in_hilight = FALSE;
			if (xtext->render_hilights_only) {
				/* stop drawing this ent */
				ret = 0;
				break;
			}
		}

		if (!mark && ent->mark_start == (i + offset)) {
			x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
			pstr += j;
			j = 0;
			xtext_set_bg (xtext, gc, XTEXT_MARK_BG);
			xtext_set_fg (xtext, gc, XTEXT_MARK_FG);
			xtext->backcolor = TRUE;
			mark = TRUE;
		}

		if (mark && ent->mark_end == (i + offset)) {
			x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
			pstr += j;
			j = 0;
			xtext_set_bg (xtext, gc, priv->background_color);
			xtext_set_fg (xtext, gc, priv->foreground_color);
			if (priv->background_color != XTEXT_BG) {
				xtext->backcolor = TRUE;
			} else {
				xtext->backcolor = FALSE;
			}
			mark = FALSE;
		}

	}

	if (j) {
		x += xtext_render_flush (xtext, x, y, pstr, j, gc, ent->mb);
	}

	if (mark) {
		xtext_set_bg (xtext, gc, priv->background_color);
		xtext_set_fg (xtext, gc, priv->foreground_color);
		if (priv->background_color != XTEXT_BG) {
			xtext->backcolor = TRUE;
		} else {
			xtext->backcolor = FALSE;
		}
	}

	/* draw background to the right of the text */
	if (!left_only && !xtext->dont_render) {
		/* draw separator now so it doesn't appear to flicker */
		xtext_draw_sep (xtext, y - priv->font->ascent);
		if (!xtext->skip_border_fills && xtext->clip_x2 >= x) {
			int xx = MAX (x, xtext->clip_x);
			int yy = y - priv->font->ascent;
			int ww = MIN (xtext->clip_x2 - xx, (win_width + MARGIN) - xx);
			int hh = xtext->fontsize;

			xtext_draw_bg (xtext, xx, yy, ww, hh);
		}
	}

	xtext->dont_render2 = FALSE;

	return ret;
}

#ifdef USE_XLIB

/* get the desktop/root window */

static Window desktop_window = None;

static Window
get_desktop_window (Display *xdisplay, Window the_window)
{
	Atom prop, type;
	int format;
	unsigned long length, after;
	unsigned char *data;
	unsigned int nchildren;
	Window w, root, *children, parent;

	prop = XInternAtom (xdisplay, "_XROOTPMAP_ID", True);
	if (prop == None) {
		prop = XInternAtom (xdisplay, "_XROOTCOLOR_PIXEL", True);
		if (prop == None) {
			return None;
		}
	}

	for (w = the_window; w; w = parent) {
		if ((XQueryTree (xdisplay, w, &root, &parent, &children, &nchildren)) == False) {
			return None;
		}

		if (nchildren) {
			XFree (children);
		}

		XGetWindowProperty (xdisplay, w, prop, 0L, 1L, False, AnyPropertyType, &type, &format, &length, &after, &data);
		if (data) {
			XFree (data);
		}

		if (type != None) {
			return (desktop_window = w);
		}
	}

	return (desktop_window = None);
}

/* find the root window (backdrop) Pixmap */

static Pixmap
get_pixmap_prop (Display *xdisplay, Window the_window)
{
	Atom type;
	int format;
	unsigned long length, after;
	unsigned char *data;
	Pixmap pix = None;
	static Atom prop = None;

	if (desktop_window == None) {
		desktop_window = get_desktop_window (xdisplay, the_window);
	}
	if (desktop_window == None) {
		desktop_window = DefaultRootWindow (xdisplay);
	}

	if (prop == None) {
		prop = XInternAtom (xdisplay, "_XROOTPMAP_ID", True);
	}
	if (prop == None) {
		return None;
	}

	XGetWindowProperty (xdisplay, desktop_window, prop, 0L, 1L, False,
	                    AnyPropertyType, &type, &format, &length, &after,
	                    &data);
	if (data) {
		if (type == XA_PIXMAP) {
			pix = *((Pixmap *) data);
		}

		XFree (data);
	}

	return pix;
}

/* slow generic routine, for the depths/bpp we don't know about */

static void
shade_ximage_generic (GdkVisual *visual, XImage *ximg, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	int x, y;
	int bgr = (256 - rm) * (bg & visual->red_mask);
	int bgg = (256 - gm) * (bg & visual->green_mask);
	int bgb = (256 - bm) * (bg & visual->blue_mask);

	for (x = 0; x < w; x++) {
		for (y = 0; y < h; y++) {
			unsigned long pixel = XGetPixel (ximg, x, y);
			int r, g, b;

			r = rm * (pixel & visual->red_mask) + bgr;
			g = gm * (pixel & visual->green_mask) + bgg;
			b = bm * (pixel & visual->blue_mask) + bgb;

			XPutPixel (ximg, x, y,
							((r >> 8) & visual->red_mask) |
							((g >> 8) & visual->green_mask) |
							((b >> 8) & visual->blue_mask));
		}
	}
}

#endif /* USE_XLIB */

/* Fast shading routine. Based on code by Willem Monsuwe <willem@stack.nl> */

#define SHADE_IMAGE(bytes, type, rmask, gmask, bmask) \
	unsigned char *ptr; \
	int x, y; \
	int bgr = (256 - rm) * (bg & rmask); \
	int bgg = (256 - gm) * (bg & gmask); \
	int bgb = (256 - bm) * (bg & bmask); \
	ptr = (unsigned char *) data + (w * bytes); \
	for (y = h; --y >= 0;) \
	{ \
		for (x = -w; x < 0; x++) \
		{ \
			int r, g, b; \
			b = ((type *) ptr)[x]; \
			r = rm * (b & rmask) + bgr; \
			g = gm * (b & gmask) + bgg; \
			b = bm * (b & bmask) + bgb; \
			((type *) ptr)[x] = ((r >> 8) & rmask) \
										| ((g >> 8) & gmask) \
										| ((b >> 8) & bmask); \
		} \
		ptr += bpl; \
    }

/* RGB 15 */
static void
shade_ximage_15 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	SHADE_IMAGE (2, guint16, 0x7c00, 0x3e0, 0x1f);
}

/* RGB 16 */
static void
shade_ximage_16 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	SHADE_IMAGE (2, guint16, 0xf800, 0x7e0, 0x1f);
}

/* RGB 24 */
static void
shade_ximage_24 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	/* 24 has to be a special case, there's no guint24, or 24bit MOV :) */
	unsigned char *ptr;
	int x, y;
	int bgr = (256 - rm) * ((bg & 0xff0000) >> 16);
	int bgg = (256 - gm) * ((bg & 0xff00) >> 8);
	int bgb = (256 - bm) * (bg & 0xff);

	ptr = (unsigned char *) data + (w * 3);
	for (y = h; --y >= 0;) {
		for (x = -(w * 3); x < 0; x += 3) {
			int r, g, b;

#if (G_BYTE_ORDER == G_BIG_ENDIAN)
			r = (ptr[x + 0] * rm + bgr) >> 8;
			g = (ptr[x + 1] * gm + bgg) >> 8;
			b = (ptr[x + 2] * bm + bgb) >> 8;
			ptr[x + 0] = r;
			ptr[x + 1] = g;
			ptr[x + 2] = b;
#else /* (G_BYTE_ORDER == G_BIG_ENDIAN) */
			r = (ptr[x + 2] * rm + bgr) >> 8;
			g = (ptr[x + 1] * gm + bgg) >> 8;
			b = (ptr[x + 0] * bm + bgb) >> 8;
			ptr[x + 2] = r;
			ptr[x + 1] = g;
			ptr[x + 0] = b;
#endif /* (G_BYTE_ORDER == G_BIG_ENDIAN) */
		}
		ptr += bpl;
	}
}

/* RGB 32 */
static void
shade_ximage_32 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	SHADE_IMAGE (4, guint32, 0xff0000, 0xff00, 0xff);
}

static void
shade_image (GdkVisual *visual, void *data, int bpl, int bpp, int w, int h,
				 int rm, int gm, int bm, int bg, int depth)
{
	int bg_r, bg_g, bg_b;

	bg_r = bg & visual->red_mask;
	bg_g = bg & visual->green_mask;
	bg_b = bg & visual->blue_mask;

#ifdef USE_MMX
	/* the MMX routines are about 50% faster at 16-bit. */
	/* only use MMX routines with a pure black background */
	/* We do a runtime check for MMX too */
	if (bg_r == 0 && bg_g == 0 && bg_b == 0 && have_mmx ()) {
		switch (depth)
		{
		case 15:
			shade_ximage_15_mmx (data, bpl, w, h, rm, gm, bm);
			break;
		case 16:
			shade_ximage_16_mmx (data, bpl, w, h, rm, gm, bm);
			break;
		case 24:
			if (bpp != 32) {
				goto generic;
			}
		case 32:
			shade_ximage_32_mmx (data, bpl, w, h, rm, gm, bm);
			break;
		default:
			goto generic;
		}
	} else {
generic:
#endif /* USE_MMX */
		switch (depth)
		{
		case 15:
			shade_ximage_15 (data, bpl, w, h, rm, gm, bm, bg);
			break;
		case 16:
			shade_ximage_16 (data, bpl, w, h, rm, gm, bm, bg);
			break;
		case 24:
			if (bpp != 32) {
				shade_ximage_24 (data, bpl, w, h, rm, gm, bm, bg);
				break;
			}
		case 32:
			shade_ximage_32 (data, bpl, w, h, rm, gm, bm, bg);
		}
#ifdef USE_MMX
	}
#endif /* USE_MMX */
}

#ifdef USE_XLIB

#ifdef USE_SHM

static XImage *
get_shm_image (Display *xdisplay, XShmSegmentInfo *shminfo, int x, int y, int w, int h, int depth, Pixmap pix)
{
	XImage *ximg;

	shminfo->shmid = -1;
	shminfo->shmaddr = (char*) -1;
	ximg = XShmCreateImage (xdisplay, 0, depth, ZPixmap, 0, shminfo, w, h);
	if (!ximg) {
		return NULL;
	}

	shminfo->shmid = shmget (IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT|0600);
	if (shminfo->shmid == -1) {
		XDestroyImage (ximg);
		return NULL;
	}

	shminfo->readOnly = False;
	ximg->data = shminfo->shmaddr = (char *)shmat (shminfo->shmid, 0, 0);
	if (shminfo->shmaddr == ((char *)-1)) {
		shmctl (shminfo->shmid, IPC_RMID, 0);
		XDestroyImage (ximg);
		return NULL;
	}

	XShmAttach (xdisplay, shminfo);
	XSync (xdisplay, False);
	shmctl (shminfo->shmid, IPC_RMID, 0);
	XShmGetImage (xdisplay, pix, ximg, x, y, AllPlanes);

	return ximg;
}

static XImage *
get_image (XText *xtext, Display *xdisplay, XShmSegmentInfo *shminfo,
           int x, int y, int w, int h, int depth, Pixmap pix)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	XImage *ximg;

	priv->has_shm = TRUE;
	ximg = get_shm_image (xdisplay, shminfo, x, y, w, h, depth, pix);
	if (!ximg) {
		priv->has_shm = FALSE;
		ximg = XGetImage (xdisplay, pix, x, y, w, h, -1, ZPixmap);
	}

	return ximg;
}

#endif /* USE_SHM */

static GdkPixmap *
shade_pixmap (XText * xtext, Pixmap p, int x, int y, int w, int h, gboolean recycle)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	unsigned int dummy, width, height, depth;
	GdkPixmap *shaded_pix;
	Window root;
	Pixmap tmp;
	XImage *ximg;
	XGCValues gcv;
	GC tgc;
	Display *xdisplay = GDK_WINDOW_XDISPLAY (priv->draw_buffer);

	XGetGeometry (xdisplay, p, &root, &dummy, &dummy, &width, &height, &dummy, &depth);

	if (width < x + w || height < y + h || x < 0 || y < 0) {
		gcv.subwindow_mode = IncludeInferiors;
		gcv.graphics_exposures = False;
		tgc = XCreateGC (xdisplay, p, GCGraphicsExposures|GCSubwindowMode, &gcv);
		tmp = XCreatePixmap (xdisplay, p, w, h, depth);
		XSetTile (xdisplay, tgc, p);
		XSetFillStyle (xdisplay, tgc, FillTiled);
		XSetTSOrigin (xdisplay, tgc, -x, -y);
		XFillRectangle (xdisplay, tmp, tgc, 0, 0, w, h);
		XFreeGC (xdisplay, tgc);

#ifdef USE_SHM
		ximg = get_image (xtext, xdisplay, &priv->shminfo, 0, 0, w, h, depth, tmp);
#else /* USE_SHM */
		ximg = XGetImage (xdisplay, tmp, 0, 0, w, h, -1, ZPixmap);
#endif /* USE_SHM */
		XFreePixmap (xdisplay, tmp);
	} else {
#ifdef USE_SHM
		ximg = get_image (xtext, xdisplay, &priv->shminfo, x, y, w, h, depth, p);
#else /* USE_SHM */
		ximg = XGetImage (xdisplay, p, x, y, w, h, -1, ZPixmap);
#endif /* USE_SHM */
	}

	if (!ximg) {
		return NULL;
	}

	if (depth <= 14) {
		shade_ximage_generic (gdk_drawable_get_visual (GTK_WIDGET (xtext)->window),
		                      ximg, ximg->bytes_per_line, w, h, priv->tint,
		                      priv->tint, priv->tint, xtext->palette[XTEXT_BG]);
	} else {
		shade_image (gdk_drawable_get_visual (GTK_WIDGET (xtext)->window),
		             ximg->data, ximg->bytes_per_line, ximg->bits_per_pixel,
		             w, h, priv->tint, priv->tint, priv->tint,
		             xtext->palette[XTEXT_BG], depth);
	}

	if (recycle) {
		shaded_pix = priv->pixmap;
	} else {
#ifdef USE_SHM
		if (priv->has_shm) {
			shaded_pix = gdk_pixmap_foreign_new_for_display (
				gdk_drawable_get_display (priv->draw_buffer),
				XShmCreatePixmap (xdisplay, p, ximg->data, &priv->shminfo, w, h, depth));
		} else {
			shaded_pix = gdk_pixmap_new (GTK_WIDGET (xtext)->window, w, h, depth);
		}
#else /* USE_SHM */
		shaded_pix = gdk_pixmap_new (GTK_WIDGET (xtext)->window, w, h, depth);
#endif /* USE_SHM */
	}

#ifdef USE_SHM
	if (!priv->has_shm) {
		XPutImage (xdisplay, GDK_WINDOW_XWINDOW (shaded_pix),
		           GDK_GC_XGC (priv->fgc), ximg, 0, 0, 0, 0, w, h);
	}
#else /* USE_SHM */
	XPutImage (xdisplay, GDK_WINDOW_XWINDOW (shaded_pix),
	           GDK_GC_XGC (priv->fgc), ximg, 0, 0, 0, 0, w, h);
#endif /* USE_SHM */
	XDestroyImage (ximg);

	return shaded_pix;
}

#endif /* USE_XLIB */

/* free transparency priv->pixmap */
#if defined(USE_XLIB) || defined(WIN32)

static void
xtext_free_trans (XText * xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	if (priv->pixmap) {
#ifdef USE_SHM
		if (priv->has_shm) {
			XFreePixmap (GDK_WINDOW_XDISPLAY (priv->pixmap),
			             GDK_WINDOW_XWINDOW (priv->pixmap));
			XShmDetach (GDK_WINDOW_XDISPLAY (priv->draw_buffer), &priv->shminfo);
			shmdt (priv->shminfo.shmaddr);
		}
		priv->has_shm = FALSE;
#endif /* USE_SHM */
		g_object_unref (priv->pixmap);
		priv->pixmap = NULL;
	}
}

#endif /* defined(USE_XLIB) || defined(WIN32) */

#ifdef WIN32

static GdkPixmap *
win32_tint (XText *xtext, GdkImage *img, int width, int height)
{
	XTextPriv *priv;
	guchar *pixelp;
	int x, y;
	GdkPixmap *pix;
	GdkVisual *visual = gdk_drawable_get_visual (GTK_WIDGET (xtext)->window);
	guint32 pixel;
	int r, g, b;

	priv = XTEXT_GET_PRIVATE (xtext);

	if (img->depth <= 14) {
		/* slow generic routine */
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (img->depth == 1) {
					pixel = (((guchar *) img->mem)[y * img->bpl + (x >> 3)] & (1 << (7 - (x & 0x7)))) != 0;
					goto here;
				}

				if (img->depth == 4) {
					pixelp = (guchar *) img->mem + y * img->bpl + (x >> 1);
					if (x&1) {
						pixel = (*pixelp) & 0x0F;
						goto here;
					}

					pixel = (*pixelp) >> 4;
					goto here;
				}

				pixelp = (guchar *) img->mem + y * img->bpl + x * img->bpp;

				switch (img->bpp) {
				case 1:
					pixel = *pixelp; break;

				/* Windows is always LSB, no need to check img->byte_order. */
				case 2:
					pixel = pixelp[0] | (pixelp[1] << 8); break;

				case 3:
					pixel = pixelp[0] | (pixelp[1] << 8) | (pixelp[2] << 16); break;

				case 4:
					pixel = pixelp[0] | (pixelp[1] << 8) | (pixelp[2] << 16); break;
				}

here:
				r = (pixel & visual->red_mask) >> visual->red_shift;
				g = (pixel & visual->green_mask) >> visual->green_shift;
				b = (pixel & visual->blue_mask) >> visual->blue_shift;

				/* actual tinting is only these 3 lines */
				pixel = ((r * priv->tint) >> 8) << visual->red_shift |
				        ((g * priv->tint) >> 8) << visual->green_shift |
				        ((b * priv->tint) >> 8) << visual->blue_shift;

				if (img->depth == 1) {
					if (pixel & 1) {
						((guchar *) img->mem)[y * img->bpl + (x >> 3)] |= (1 << (7 - (x & 0x7)));
					} else {
						((guchar *) img->mem)[y * img->bpl + (x >> 3)] &= ~(1 << (7 - (x & 0x7)));
					}
				} else if (img->depth == 4) {
					pixelp = (guchar *) img->mem + y * img->bpl + (x >> 1);

					if (x&1) {
						*pixelp &= 0xF0;
						*pixelp |= (pixel & 0x0F);
					} else {
						*pixelp &= 0x0F;
						*pixelp |= (pixel << 4);
					}
				} else {
					pixelp = (guchar *) img->mem + y * img->bpl + x * img->bpp;

					/* Windows is always LSB, no need to check img->byte_order. */
					switch (img->bpp) {
					case 4:
						pixelp[3] = 0;
					case 3:
						pixelp[2] = ((pixel >> 16) & 0xFF);
					case 2:
						pixelp[1] = ((pixel >> 8) & 0xFF);
					case 1:
						pixelp[0] = (pixel & 0xFF);
					}
				}
			}
		}
	} else {
		shade_image (visual, img->mem, img->bpl, img->bpp, width, height,
		             priv->tint, priv->tint, priv->tint,
		             xtext->palette[XTEXT_BG], visual->depth);
	}

	/* no need to dump it to a Pixmap, it's one and the same on win32 */
	pix = (GdkPixmap *)img;

	return pix;
}

#endif /* WIN32 */

/* grab pixmap from root window and set xtext->pixmap */
#if defined(USE_XLIB) || defined(WIN32)

static void
xtext_load_trans (XText * xtext, gboolean recycle)
{
#ifdef WIN32
	XTextPriv *priv;
	GdkImage *img;
	int width, height;
	HDC hdc;
	HWND hwnd;

	priv = XTEXT_GET_PRIVATE (xtext);

	/* if not shaded, we paint directly with PaintDesktop() */
	if (!xtext->shaded) {
		return;
	}

	hwnd = GDK_WINDOW_HWND (GTK_WIDGET (xtext)->window);
	hdc = GetDC (hwnd);
	PaintDesktop (hdc);
	ReleaseDC (hwnd, hdc);

	gdk_window_get_size (GTK_WIDGET (xtext)->window, &width, &height);
	img = gdk_image_get (GTK_WIDGET (xtext)->window, 0, 0, width+128, height);
	priv->pixmap = win32_tint (xtext, img, img->width, img->height);

#else /* WIN32 */

	XTextPriv *priv;
	Pixmap rootpix;
	GtkWidget *widget = GTK_WIDGET (xtext);
	int x, y;

	priv = XTEXT_GET_PRIVATE (xtext);

	rootpix = get_pixmap_prop (GDK_WINDOW_XDISPLAY (widget->window), GDK_WINDOW_XWINDOW (widget->window));
	if (rootpix == None) {
		xtext->transparent = FALSE;
		return;
	}

	gdk_window_get_origin (widget->window, &x, &y);

	if (xtext->shaded) {
		int width, height;
		gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);
		priv->pixmap = shade_pixmap (xtext, rootpix, x, y, width+105, height, recycle);
		if (priv->pixmap == NULL) {
			xtext->shaded = 0;
			goto noshade;
		}
		gdk_gc_set_tile (priv->bgc, priv->pixmap);
		gdk_gc_set_ts_origin (priv->bgc, 0, 0);
		xtext->ts_x = xtext->ts_y = 0;
	} else {
noshade:
		priv->pixmap = gdk_pixmap_foreign_new_for_display (gdk_drawable_get_display (GTK_WIDGET (xtext)->window), rootpix);
		gdk_gc_set_tile (priv->bgc, priv->pixmap);
		gdk_gc_set_ts_origin (priv->bgc, -x, -y);
		xtext->ts_x = -x;
		xtext->ts_y = -y;
	}
	gdk_gc_set_fill (priv->bgc, GDK_TILED);
#endif /* WIN32 */
}

#endif /* defined(USE_XLIB) || defined(WIN32) */

/* walk through str until this line doesn't fit anymore */

static int
find_next_wrap (XText * xtext, textentry * ent, unsigned char *str, int win_width, int indent)
{
	unsigned char *last_space = str;
	unsigned char *orig_str = str;
	int str_width = indent;
	int col = FALSE;
	int nc = 0;
	int mbl;
	int ret;
	int limit_offset = 0;

	/* single liners */
	if (win_width >= ent->str_width + ent->indent) {
		return ent->str_len;
	}

	/* it does happen! */
	if (win_width < 1) {
		ret = ent->str_len - (str - ent->str);
		goto done;
	}

	while (1) {
		if ((col && isdigit (*str) && nc < 2) || (col && *str == ',' && isdigit (*(str+1)) && nc < 3)) {
			nc++;
			if (*str == ',') {
				nc = 0;
			}
			limit_offset++;
			str++;
		} else {
			col = FALSE;
			switch (*str) {
			case ATTR_COLOR:
				col = TRUE;
				nc = 0;
			case ATTR_BEEP:
			case ATTR_RESET:
			case ATTR_REVERSE:
			case ATTR_BOLD:
			case ATTR_UNDERLINE:
			case ATTR_ITALICS:
				limit_offset++;
				str++;
				break;
			default:
				str_width += backend_get_char_width (xtext, str, &mbl);
				if (str_width > win_width) {
					if (str - last_space > WORDWRAP_LIMIT + limit_offset) {
						ret = str - orig_str; /* fall back to character wrap */
					} else {
						if (*last_space == ' ') {
							last_space++;
						}
						ret = last_space - orig_str;
						if (ret == 0) {
							/* fall back to character wrap */
							ret = str - orig_str;
						}
					}
					goto done;
				}

				/* keep a record of the last space, for wordwrapping */
				if (is_del (*str)) {
					last_space = str;
					limit_offset = 0;
				}

				/* progress to the next char */
				str += mbl;

			}
		}

		if (str >= ent->str + ent->str_len) {
			ret = str - orig_str;
			goto done;
		}
	}

done:

	/* must make progress */
	if (ret < 1) {
		ret = 1;
	}

	return ret;
}

/* find the offset, in bytes, that wrap number 'line' starts at */

static int
xtext_find_subline (XText *xtext, textentry *ent, int line)
{
	int win_width;
	unsigned char *str;
	int indent, str_pos, line_pos, len;

	if (ent->lines_taken < 2 || line < 1) {
		return 0;
	}

	/* we record the first 4 lines' wraps, so take a shortcut */
	if (line <= RECORD_WRAPS) {
		return ent->wrap_offset[line - 1];
	}

	gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &win_width, 0);
	win_width -= MARGIN;

	/* start from the last recorded wrap, and move forward */
	indent = xtext->buffer->indent;
	str_pos = ent->wrap_offset[RECORD_WRAPS-1];
	str = str_pos + ent->str;
	line_pos = RECORD_WRAPS;

	do {
		len = find_next_wrap (xtext, ent, str, win_width, indent);
		indent = xtext->buffer->indent;
		str += len;
		str_pos += len;
		line_pos++;
		if (line_pos >= line) {
			return str_pos;
		}
	} while (str < ent->str + ent->str_len);

	return 0;
}

/* render a single line, which may wrap to more lines */

static int
xtext_render_line (XText * xtext, textentry * ent, int line, int lines_max, int subline, int win_width)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	unsigned char *str;
	int indent, taken, entline, len, y, start_subline;

	entline = taken = 0;
	str = ent->str;
	indent = ent->indent;
	start_subline = subline;

	/* draw the timestamp */
	if (xtext->auto_indent && xtext->buffer->time_stamp && !xtext->skip_stamp) {
		char *time_str;
		int stamp_size = xtext_get_stamp_str (ent->stamp, &time_str);
		int tmp = ent->mb;
		y = (xtext->fontsize * line) + priv->font->ascent - priv->pixel_offset;
		ent->mb = TRUE;
		xtext_render_str (xtext, y, ent, time_str, stamp_size, win_width, 2, line, TRUE);
		ent->mb = tmp;
		g_free (time_str);
	}

	/* draw each line one by one */
	do {
		/* if it's one of the first 4 wraps, we don't need to calculate it, it's
		 * recorded in ->wrap_offset. This saves us a loop. */
		if (entline < RECORD_WRAPS) {
			if (ent->lines_taken < 2) {
				len = ent->str_len;
			} else {
				if (entline > 0) {
					len = ent->wrap_offset[entline] - ent->wrap_offset[entline-1];
				} else {
					len = ent->wrap_offset[0];
				}
			}
		} else {
			len = find_next_wrap (xtext, ent, str, win_width, indent);
		}

		entline++;

		y = (xtext->fontsize * line) + priv->font->ascent - priv->pixel_offset;
		if (!subline) {
			if (!xtext_render_str (xtext, y, ent, str, len, win_width, indent, line, FALSE)) {
				/* small optimization */
				xtext_draw_marker (xtext, ent, y - xtext->fontsize * (taken + start_subline + 1));
				return ent->lines_taken - subline;
			}
		} else {
			xtext->dont_render = TRUE;
			xtext_render_str (xtext, y, ent, str, len, win_width, indent, line, FALSE);
			xtext->dont_render = FALSE;
			subline--;
			line--;
			taken--;
		}

		indent = xtext->buffer->indent;
		line++;
		taken++;
		str += len;

		if (line >= lines_max) {
			break;
		}

	} while (str < ent->str + ent->str_len);

	xtext_draw_marker (xtext, ent, y - xtext->fontsize * (taken + start_subline));

	return taken;
}

static void
xtext_fix_indent (xtext_buffer *buf)
{
	int j;

	/* make indent a multiple of the space width */
	if (buf->indent && buf->xtext->space_width) {
		j = 0;
		while (j < buf->indent) {
			j += buf->xtext->space_width;
		}
		buf->indent = j;
	}

	dontscroll (buf);	/* force scrolling off */
}

static void
xtext_recalc_widths (xtext_buffer *buf, int do_str_width)
{
	textentry *ent;

	/* since we have a new font, we have to recalc the text widths */
	ent = buf->text_first;
	while (ent) {
		if (do_str_width) {
			ent->str_width = xtext_text_width (buf->xtext, ent->str, ent->str_len, NULL);
		}
		if (ent->left_len != -1) {
			ent->indent = (buf->indent - xtext_text_width (buf->xtext, ent->str, ent->left_len, NULL)) - buf->xtext->space_width;
			if (ent->indent < MARGIN) {
				ent->indent = MARGIN;
			}
		}
		ent = ent->next;
	}

	xtext_calc_lines (buf, FALSE);
}

int
xtext_set_font (XText *xtext, char *name)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	char *time_str;
	int stamp_size;

	if (priv->font) {
		backend_font_close (xtext);
	}

	/* realize now, so that font_open has a XDisplay */
	gtk_widget_realize (GTK_WIDGET (xtext));

	backend_font_open (xtext, name);
	if (priv->font == NULL) {
		return FALSE;
	}

	xtext->space_width = xtext_text_width (xtext, " ", 1, NULL);
	xtext->fontsize = priv->font->ascent + priv->font->descent;

	stamp_size = xtext_get_stamp_str (time(0), &time_str);
	xtext->stamp_width = xtext_text_width (xtext, time_str, stamp_size, NULL) + MARGIN;
	g_free (time_str);

	xtext_fix_indent (xtext->buffer);

	if (GTK_WIDGET_REALIZED (xtext)) {
		xtext_recalc_widths (xtext->buffer, TRUE);
	}

	return TRUE;
}

void
xtext_set_background (XText * xtext, GdkPixmap * pixmap, gboolean trans)
{
	XTextPriv *priv;
	GdkGCValues val;
	gboolean shaded = FALSE;

	priv = XTEXT_GET_PRIVATE (xtext);

	if (trans && priv->tint != 255) {
		shaded = TRUE;
	}

#if !defined(USE_XLIB) && !defined(WIN32)
	shaded = FALSE;
	trans = FALSE;
#endif /* !defined(USE_XLIB) && !defined(WIN32) */

	if (priv->pixmap) {
#if defined(USE_XLIB) || defined(WIN32)
		if (xtext->transparent) {
			xtext_free_trans (xtext);
		} else {
			g_object_unref (priv->pixmap);
		}
#else
		g_object_unref (priv->pixmap);
#endif /* !defined(USE_XLIB) && !defined(WIN32) */
		priv->pixmap = NULL;
	}

	xtext->transparent = trans;

#if defined(USE_XLIB) || defined(WIN32)
	if (trans) {
		xtext->shaded = shaded;
		if (GTK_WIDGET_REALIZED (xtext))
			xtext_load_trans (xtext, FALSE);
		return;
	}
#endif /* !defined(USE_XLIB) && !defined(WIN32) */

	dontscroll (xtext->buffer);
	priv->pixmap = pixmap;

	if (pixmap != 0) {
		g_object_ref (pixmap);
		if (GTK_WIDGET_REALIZED (xtext)) {
			gdk_gc_set_tile (priv->bgc, pixmap);
			gdk_gc_set_ts_origin (priv->bgc, 0, 0);
			xtext->ts_x = xtext->ts_y = 0;
			gdk_gc_set_fill (priv->bgc, GDK_TILED);
		}
	} else if (GTK_WIDGET_REALIZED (xtext)) {
		g_object_unref (priv->bgc);
		val.subwindow_mode = GDK_INCLUDE_INFERIORS;
		val.graphics_exposures = 0;
		priv->bgc = gdk_gc_new_with_values (GTK_WIDGET (xtext)->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
		xtext_set_fg (xtext, priv->bgc, XTEXT_BG);
	}
}

void
xtext_save (XText * xtext, int fh)
{
	textentry *ent;
	int newlen;
	char *buf;

	ent = xtext->buffer->text_first;
	while (ent) {
		buf = xtext_strip_color (ent->str, ent->str_len, NULL, &newlen, NULL);
		write (fh, buf, newlen);
		write (fh, "\n", 1);
		g_free (buf);
		ent = ent->next;
	}
}

/* count how many lines 'ent' will take (with wraps) */

static int
xtext_lines_taken (xtext_buffer *buf, textentry * ent)
{
	unsigned char *str;
	int indent, taken, len;
	int win_width;

	win_width = buf->window_width - MARGIN;

	if (ent->str_width + ent->indent < win_width) {
		return 1;
	}

	indent = ent->indent;
	str = ent->str;
	taken = 0;

	do {
		len = find_next_wrap (buf->xtext, ent, str, win_width, indent);
		if (taken < RECORD_WRAPS) {
			ent->wrap_offset[taken] = (str + len) - ent->str;
		}
		indent = buf->indent;
		taken++;
		str += len;
	} while (str < ent->str + ent->str_len);

	return taken;
}

/* Calculate number of actual lines (with wraps), to set adj->lower. *
 * This should only be called when the window resizes.               */

static void
xtext_calc_lines (xtext_buffer *buf, int fire_signal)
{
	textentry *ent;
	int width;
	int height;
	int lines;

	gdk_drawable_get_size (GTK_WIDGET (buf->xtext)->window, &width, &height);
	width -= MARGIN;

	if (width < 30 || height < buf->xtext->fontsize || width < buf->indent + 30) {
		return;
	}

	lines = 0;
	ent = buf->text_first;
	while (ent) {
		ent->lines_taken = xtext_lines_taken (buf, ent);
		lines += ent->lines_taken;
		ent = ent->next;
	}

	buf->pagetop_ent = NULL;
	buf->num_lines = lines;
	xtext_adjustment_set (buf, fire_signal);
}

/* find the n-th line in the linked list, this includes wrap calculations */

static textentry *
xtext_nth (XText *xtext, int line, int *subline)
{
	int lines = 0;
	textentry *ent;

	ent = xtext->buffer->text_first;

	/* -- optimization -- try to make a short-cut using the pagetop ent */
	if (xtext->buffer->pagetop_ent) {
		if (line == xtext->buffer->pagetop_line) {
			*subline = xtext->buffer->pagetop_subline;
			return xtext->buffer->pagetop_ent;
		}

		if (line > xtext->buffer->pagetop_line) {
			/* lets start from the pagetop instead of the absolute beginning */
			ent = xtext->buffer->pagetop_ent;
			lines = xtext->buffer->pagetop_line - xtext->buffer->pagetop_subline;
		} else if (line > xtext->buffer->pagetop_line - line) {
			/* move backwards from pagetop */
			ent = xtext->buffer->pagetop_ent;
			lines = xtext->buffer->pagetop_line - xtext->buffer->pagetop_subline;
			while (1) {
				if (lines <= line) {
					*subline = line - lines;
					return ent;
				}
				ent = ent->prev;
				if (!ent) {
					break;
				}
				lines -= ent->lines_taken;
			}
			return 0;
		}
	}
	/* -- end of optimization -- */

	while (ent) {
		lines += ent->lines_taken;
		if (lines > line) {
			*subline = ent->lines_taken - (lines - line);
			return ent;
		}
		ent = ent->next;
	}
	return 0;
}

/* render enta (or an inclusive range enta->entb) */

static int
xtext_render_ents (XText * xtext, textentry * enta, textentry * entb)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent, *orig_ent, *tmp_ent;
	int line;
	int lines_max;
	int width;
	int height;
	int subline;
	int drawing = FALSE;

	if (xtext->buffer->indent < MARGIN) {
		xtext->buffer->indent = MARGIN;	  /* 2 pixels is our left margin */
	}

	gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);
	width -= MARGIN;

	if (width < 32 || height < xtext->fontsize || width < xtext->buffer->indent + 30) {
		return 0;
	}

	lines_max = ((height + priv->pixel_offset) / xtext->fontsize) + 1;
	line = 0;
	orig_ent = xtext->buffer->pagetop_ent;
	subline = xtext->buffer->pagetop_subline;

	/* used before a complete page is in buffer */
	if (orig_ent == NULL) {
		orig_ent = xtext->buffer->text_first;
	}

	/* check if enta is before the start of this page */
	if (entb) {
		tmp_ent = orig_ent;
		while (tmp_ent) {
			if (tmp_ent == enta) {
				break;
			}

			if (tmp_ent == entb) {
				drawing = TRUE;
				break;
			}
			tmp_ent = tmp_ent->next;
		}
	}

	ent = orig_ent;
	while (ent) {
		if (entb && ent == enta) {
			drawing = TRUE;
		}

		if (drawing || ent == entb || ent == enta) {
			xtext_reset (xtext, FALSE, TRUE);
			line += xtext_render_line (xtext, ent, line, lines_max, subline, width);
			subline = 0;
			xtext->jump_in_offset = 0;	/* jump_in_offset only for the 1st */
		} else {
			if (ent == orig_ent) {
				line -= subline;
				subline = 0;
			}
			line += ent->lines_taken;
		}

		if (ent == entb) {
			break;
		}

		if (line >= lines_max) {
			break;
		}

		ent = ent->next;
	}

	/* space below last line */
	return (xtext->fontsize * line) - priv->pixel_offset;
}

/* render a whole page/window, starting from 'startline' */

static void
xtext_render_page (XText * xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent;
	int line;
	int lines_max;
	int width;
	int height;
	int subline;
	int startline = priv->adj->value;

	if (!GTK_WIDGET_REALIZED(xtext)) {
		return;
	}

	if (xtext->buffer->indent < MARGIN) {
		xtext->buffer->indent = MARGIN;	  /* 2 pixels is our left margin */
	}

	gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);

	if (width < 34 || height < xtext->fontsize || width < xtext->buffer->indent + 32) {
		return;
	}

	priv->pixel_offset = (priv->adj->value - startline) * xtext->fontsize;

	subline = line = 0;
	ent = xtext->buffer->text_first;

	if (startline > 0) {
		ent = xtext_nth (xtext, startline, &subline);
	}

	xtext->buffer->pagetop_ent = ent;
	xtext->buffer->pagetop_subline = subline;
	xtext->buffer->pagetop_line = startline;

{
	int pos, overlap;
	GdkRectangle area;

	if (xtext->buffer->num_lines <= priv->adj->page_size) {
		dontscroll (xtext->buffer);
	}

	pos = priv->adj->value * xtext->fontsize;
	overlap = xtext->buffer->last_pixel_pos - pos;
	xtext->buffer->last_pixel_pos = pos;

#ifdef WIN32
	if (!xtext->transparent && !xtext->pixmap && abs (overlap) < height) {
#else /* WIN32 */
	if (!priv->pixmap && abs (overlap) < height) {
#endif /* WIN32 */
		/* so the obscured regions are exposed */
		gdk_gc_set_exposures (priv->fgc, TRUE);
		if (overlap < 1) {
			/* DOWN */
			int remainder;

			gdk_draw_drawable (priv->draw_buffer, priv->fgc, priv->draw_buffer, 0, -overlap, 0, 0, width, height + overlap);
			remainder = ((height - priv->font->descent) % xtext->fontsize) + priv->font->descent;
			area.y = (height + overlap) - remainder;
			area.height = remainder - overlap;
		} else {
			gdk_draw_drawable (priv->draw_buffer, priv->fgc, priv->draw_buffer, 0, 0, 0, overlap, width, height - overlap);
			area.y = 0;
			area.height = overlap;
		}
		gdk_gc_set_exposures (priv->fgc, FALSE);

		if (area.height > 0) {
			area.x = 0;
			area.width = width;
			xtext_paint (GTK_WIDGET (xtext), &area);
		}
		xtext->buffer->grid_dirty = TRUE;

		return;
	}
}

	xtext->buffer->grid_dirty = FALSE;
	width -= MARGIN;
	lines_max = ((height + priv->pixel_offset) / xtext->fontsize) + 1;

	while (ent) {
		xtext_reset (xtext, FALSE, TRUE);
		line += xtext_render_line (xtext, ent, line, lines_max, subline, width);
		subline = 0;

		if (line >= lines_max)
			break;

		ent = ent->next;
	}

	line = (xtext->fontsize * line) - priv->pixel_offset;
	/* fill any space below the last line with our background GC */
	xtext_draw_bg (xtext, 0, line, width + MARGIN, height - line);

	/* draw the separator line */
	xtext_draw_sep (xtext, -1);
}

void
xtext_refresh (XText * xtext, int do_trans)
{
	if (GTK_WIDGET_REALIZED (GTK_WIDGET (xtext))) {
#if defined(USE_XLIB) || defined(WIN32)
		if (xtext->transparent && do_trans) {
			xtext_free_trans (xtext);
			xtext_load_trans (xtext, FALSE);
		}
#endif /* defined(USE_XLIB) || defined(WIN32) */
		xtext_render_page (xtext);
	}
}

/* remove the topline from the list */

static void
xtext_remove_top (xtext_buffer *buffer)
{
	XText *xtext = buffer->xtext;
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent;

	ent = buffer->text_first;
	if (!ent) {
		return;
	}
	buffer->num_lines -= ent->lines_taken;
	buffer->pagetop_line -= ent->lines_taken;
	buffer->last_pixel_pos -= (ent->lines_taken * buffer->xtext->fontsize);
	buffer->text_first = ent->next;
	buffer->text_first->prev = NULL;

	buffer->old_value -= ent->lines_taken;
	if (buffer->xtext->buffer == buffer) {
		/* is it the current buffer? */
		priv->adj->value -= ent->lines_taken;
		xtext->select_start_adj -= ent->lines_taken;
	}

	if (ent == buffer->pagetop_ent) {
		buffer->pagetop_ent = NULL;
	}

	if (ent == buffer->last_ent_start) {
		buffer->last_ent_start = ent->next;
	}

	if (ent == buffer->last_ent_end) {
		buffer->last_ent_start = NULL;
		buffer->last_ent_end = NULL;
	}

	if (buffer->marker_pos == ent) {
		buffer->marker_pos = NULL;
	}

	g_free (ent);
}

void
xtext_clear (xtext_buffer *buf)
{
	textentry *next;

	buf->scrollbar_down = TRUE;
	buf->last_ent_start = NULL;
	buf->last_ent_end = NULL;
	buf->marker_pos = NULL;
	dontscroll (buf);

	while (buf->text_first) {
		next = buf->text_first->next;
		g_free (buf->text_first);
		buf->text_first = next;
	}
	buf->text_last = NULL;

	if (buf->xtext->buffer == buf) {
		xtext_calc_lines (buf, TRUE);
		xtext_refresh (buf->xtext, 0);
	} else {
		xtext_calc_lines (buf, FALSE);
	}

	if (buf->xtext->auto_indent) {
		buf->xtext->buffer->indent = 1;
	}
}

static gboolean
xtext_check_ent_visibility (XText * xtext, textentry *find_ent, int add)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent;
	int lines_max;
	int line = 0;
	int width;
	int height;

	gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);

	lines_max = ((height + priv->pixel_offset) / xtext->fontsize) + add;
	ent = xtext->buffer->pagetop_ent;

	while (ent && line < lines_max) {
		if (find_ent == ent) {
			return TRUE;
		}
		line += ent->lines_taken;
		ent = ent->next;
	}

	return FALSE;
}

void
xtext_check_marker_visibility (XText * xtext)
{
	if (xtext_check_ent_visibility (xtext, xtext->buffer->marker_pos, 1)) {
		xtext->buffer->marker_seen = TRUE;
	}
}

textentry *
xtext_search (XText * xtext, const gchar *text, textentry *start, gboolean case_match, gboolean backward)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	textentry *ent, *fent;
	int line;
	gchar *str = NULL, *nee, *hay = NULL;	/* needle in haystack */

	xtext_selection_clear_full (xtext->buffer);
	xtext->buffer->last_ent_start = NULL;
	xtext->buffer->last_ent_end = NULL;

	/* set up text comparand for Case Match or Ignore */
	if (case_match) {
		nee = g_strdup (text);
	} else {
		nee = g_utf8_casefold (text, strlen (text));
	}

	/* Validate that start gives a currently valid ent pointer */
	ent = xtext->buffer->text_first;
	while (ent) {
		if (ent == start) {
			break;
		}
		ent = ent->next;
	}
	if (!ent) {
		start = NULL;
	}

	/* Choose first ent to look at */
	if (start) {
		ent = backward? start->prev: start->next;
	} else {
		ent = backward? xtext->buffer->text_last: xtext->buffer->text_first;
	}

	/* Search from there to one end or the other until found */
	while (ent) {
		/* If Case Ignore, fold before & free after calling strstr */
		if (case_match) {
			hay = g_strdup (ent->str);
		} else {
			hay = g_utf8_casefold (ent->str, strlen (ent->str));
		}
		/* Try to find the needle in this haystack */
		str = g_strstr_len (hay, strlen (hay), nee);
		g_free (hay);
		if (str) {
			break;
		}
		ent = backward? ent->prev: ent->next;
	}
	fent = ent;

	/* Save distance to start, end of found string */
	if (ent) {
		ent->mark_start = str - hay;
		ent->mark_end = ent->mark_start + strlen (nee);

		/* is the match visible? Might need to scroll */
		if (!xtext_check_ent_visibility (xtext, ent, 0)) {
			ent = xtext->buffer->text_first;
			line = 0;
			while (ent) {
				line += ent->lines_taken;
				ent = ent->next;
				if (ent == fent) {
					break;
				}
			}
			while (line > priv->adj->upper - priv->adj->page_size) {
				line--;
			}

			priv->adj->value = line;
			xtext->buffer->scrollbar_down = FALSE;
			gtk_adjustment_changed (priv->adj);
		}
	}

	g_free (nee);
	gtk_widget_queue_draw (GTK_WIDGET (xtext));

	return fent;
}

static int
xtext_render_page_timeout (XText * xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	GtkAdjustment *adj = priv->adj;

	priv->add_source = 0;

	/* less than a complete page? */
	if (xtext->buffer->num_lines <= adj->page_size) {
		xtext->buffer->old_value = 0;
		adj->value = 0;
		xtext_render_page (xtext);
	} else if (xtext->buffer->scrollbar_down) {
		g_signal_handler_block (priv->adj, xtext->vc_signal_tag);
		xtext_adjustment_set (xtext->buffer, FALSE);
		gtk_adjustment_set_value (adj, adj->upper - adj->page_size);
		g_signal_handler_unblock (priv->adj, xtext->vc_signal_tag);
		xtext->buffer->old_value = adj->value;
		xtext_render_page (xtext);
	} else {
		xtext_adjustment_set (xtext->buffer, TRUE);
		if (xtext->indent_changed) {
			xtext->indent_changed = FALSE;
			xtext_render_page (xtext);
		}
	}

	return 0;
}

/* append a textentry to our linked list */

static void
xtext_append_entry (xtext_buffer *buf, textentry * ent)
{
	XText *xtext = buf->xtext;
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	unsigned int mb;
	int i;

	/* we don't like tabs */
	i = 0;
	while (i < ent->str_len) {
		if (ent->str[i] == '\t') {
			ent->str[i] = ' ';
		}
		i++;
	}

	ent->stamp = time (0);
	ent->str_width = xtext_text_width (buf->xtext, ent->str, ent->str_len, &mb);
	ent->mb = FALSE;
	if (mb) {
		ent->mb = TRUE;
	}
	ent->mark_start = -1;
	ent->mark_end = -1;
	ent->next = NULL;

	if (ent->indent < MARGIN) {
		ent->indent = MARGIN;	  /* 2 pixels is the left margin */
	}

	/* append to our linked list */
	if (buf->text_last) {
		buf->text_last->next = ent;
	} else {
		buf->text_first = ent;
	}
	ent->prev = buf->text_last;
	buf->text_last = ent;

	ent->lines_taken = xtext_lines_taken (buf, ent);
	buf->num_lines += ent->lines_taken;

	if (buf->reset_marker_pos || ((buf->marker_pos == NULL || buf->marker_seen) && (xtext->buffer != buf ||
		!gtk_window_has_toplevel_focus (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (xtext))))))) {
		buf->marker_pos = ent;
		dontscroll (buf); /* force scrolling off */
		buf->marker_seen = FALSE;
		buf->reset_marker_pos = FALSE;
	}

	if (priv->max_lines > 2 && priv->max_lines < buf->num_lines) {
		xtext_remove_top (buf);
	}

	if (xtext->buffer == buf) {
		/* this could be improved */
		if ((buf->num_lines - 1) <= priv->adj->page_size) {
			dontscroll (buf);
		}

		if (!priv->add_source) {
			/* remove scrolling events */
			if (priv->refresh_source) {
				g_source_remove (priv->refresh_source);
				priv->refresh_source = 0;
			}
			priv->add_source = g_timeout_add (REFRESH_TIMEOUT * 2, (GSourceFunc) xtext_render_page_timeout, xtext);
		}
	} else if (buf->scrollbar_down) {
		buf->old_value = buf->num_lines - priv->adj->page_size;
		if (buf->old_value < 0) {
			buf->old_value = 0;
		}
	}
}

/* the main two public functions */

void
xtext_append_indent (xtext_buffer *buf,
                         unsigned char *left_text, int left_len,
                         unsigned char *right_text, int right_len)
{
	textentry *ent;
	unsigned char *str;
	int space;
	int tempindent;
	int left_width;

	if (left_len == -1) {
		left_len = strlen (left_text);
	}

	if (right_len == -1) {
		right_len = strlen (right_text);
	}

	if (right_len >= sizeof (buf->xtext->scratch_buffer)) {
		right_len = sizeof (buf->xtext->scratch_buffer) - 1;
	}

	if (right_text[right_len-1] == '\n') {
		right_len--;
	}

	ent = g_malloc (left_len + right_len + 2 + sizeof (textentry));
	str = (unsigned char *) ent + sizeof (textentry);

	memcpy (str, left_text, left_len);
	str[left_len] = ' ';
	memcpy (str + left_len + 1, right_text, right_len);
	str[left_len + 1 + right_len] = 0;

	left_width = xtext_text_width (buf->xtext, left_text, left_len, NULL);

	ent->left_len = left_len;
	ent->str = str;
	ent->str_len = left_len + 1 + right_len;
	ent->indent = (buf->indent - left_width) - buf->xtext->space_width;

	if (buf->time_stamp) {
		space = buf->xtext->stamp_width;
	} else {
		space = 0;
	}

	/* do we need to auto adjust the separator position? */
	if (buf->xtext->auto_indent && ent->indent < MARGIN + space) {
		tempindent = MARGIN + space + buf->xtext->space_width + left_width;

		if (tempindent > buf->indent) {
			buf->indent = tempindent;
		}

		if (buf->indent > buf->xtext->max_auto_indent) {
			buf->indent = buf->xtext->max_auto_indent;
		}

		xtext_fix_indent (buf);
		xtext_recalc_widths (buf, FALSE);

		ent->indent = (buf->indent - left_width) - buf->xtext->space_width;
		buf->xtext->indent_changed = TRUE;
	}

	xtext_append_entry (buf, ent);
}

void
xtext_append (xtext_buffer *buf, unsigned char *text, int len)
{
	textentry *ent;

	if (len == -1) {
		len = strlen (text);
	}

	if (text[len-1] == '\n') {
		len--;
	}

	if (len >= sizeof (buf->xtext->scratch_buffer)) {
		len = sizeof (buf->xtext->scratch_buffer) - 1;
	}

	ent = g_malloc (len + 1 + sizeof (textentry));
	ent->str = (unsigned char *) ent + sizeof (textentry);
	ent->str_len = len;
	if (len) {
		memcpy (ent->str, text, len);
	}
	ent->str[len] = 0;
	ent->indent = 0;
	ent->left_len = -1;

	xtext_append_entry (buf, ent);
}

gboolean
xtext_is_empty (xtext_buffer *buf)
{
	return buf->text_first == NULL;
}

void
xtext_foreach (xtext_buffer *buf, XTextForeach func, void *data)
{
	textentry *ent = buf->text_first;

	while (ent) {
		(*func) (buf->xtext, ent->str, data);
		ent = ent->next;
	}
}

void
xtext_set_indent (XText *xtext, gboolean indent)
{
	xtext->auto_indent = indent;
}

void
xtext_set_max_indent (XText *xtext, int max_auto_indent)
{
	xtext->max_auto_indent = max_auto_indent;
}

void
xtext_set_max_lines (XText *xtext, int max_lines)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	priv->max_lines = max_lines;
}

void
xtext_set_show_marker (XText *xtext, gboolean show_marker)
{
	xtext->marker = show_marker;
}

void
xtext_set_time_stamp (xtext_buffer *buf, gboolean time_stamp)
{
	buf->time_stamp = time_stamp;
}

void
xtext_set_urlcheck_function (XText *xtext, int (*urlcheck_function) (GtkWidget *, char *, int))
{
	xtext->urlcheck_function = urlcheck_function;
}

void
xtext_reset_marker_pos (XText *xtext)
{
	xtext->buffer->marker_pos = NULL;
	dontscroll (xtext->buffer); /* force scrolling off */
	xtext_render_page (xtext);
	xtext->buffer->reset_marker_pos = TRUE;
}

void
xtext_buffer_show (XText *xtext, xtext_buffer *buf, int render)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	int w, h;

	if (xtext->buffer == buf) {
		return;
	}

	buf->xtext = xtext;

	if (priv->add_source) {
		g_source_remove (priv->add_source);
		priv->add_source = 0;
	}

	if (priv->refresh_source) {
		g_source_remove (priv->refresh_source);
		priv->refresh_source = 0;
	}

	if (!GTK_WIDGET_REALIZED (GTK_WIDGET (xtext))) {
		gtk_widget_realize (GTK_WIDGET (xtext));
	}

	gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &w, &h);

	/* after a font change */
	if (buf->needs_recalc) {
		buf->needs_recalc = FALSE;
		xtext_recalc_widths (buf, TRUE);
	}

	/* now change to the new buffer */
	xtext->buffer = buf;
	dontscroll (buf);	/* force scrolling off */
	priv->adj->value = buf->old_value;
	priv->adj->upper = buf->num_lines;
	if (priv->adj->upper == 0) {
		priv->adj->upper = 1;
	} else if (priv->adj->value > priv->adj->upper - priv->adj->page_size) {
		/* sanity check */
		/*buf->pagetop_ent = NULL;*/
		priv->adj->value = priv->adj->upper - priv->adj->page_size;
		if (priv->adj->value < 0) {
			priv->adj->value = 0;
		}
	}

	if (render) {
		/* did the window change size since this buffer was last shown? */
		if (buf->window_width != w) {
			buf->window_width = w;
			xtext_calc_lines (buf, FALSE);
			if (buf->scrollbar_down) {
				gtk_adjustment_set_value (priv->adj, priv->adj->upper - priv->adj->page_size);
			}
		} else if (buf->window_height != h) {
			buf->window_height = h;
			buf->pagetop_ent = NULL;
			xtext_adjustment_set (buf, FALSE);
		}

		xtext_render_page (xtext);
		gtk_adjustment_changed (priv->adj);
	} else {
		/* avoid redoing the transparency */
		xtext->avoid_trans = TRUE;
	}
}

xtext_buffer *
xtext_buffer_new (XText *xtext)
{
	xtext_buffer *buf;

	buf = g_malloc0 (sizeof (xtext_buffer));
	buf->old_value = -1;
	buf->xtext = xtext;
	buf->scrollbar_down = TRUE;
	buf->indent = xtext->space_width * 2;
	dontscroll (buf);
	buf->laststamp[0] = 0;

	return buf;
}

void
xtext_buffer_free (xtext_buffer *buf)
{
	textentry *ent, *next;

	if (buf->xtext->buffer == buf) {
		buf->xtext->buffer = buf->xtext->orig_buffer;
	}

	if (buf->xtext->selection_buffer == buf) {
		buf->xtext->selection_buffer = NULL;
	}

	ent = buf->text_first;
	while (ent) {
		next = ent->next;
		g_free (ent);
		ent = next;
	}

	g_free (buf);
}


/***********************************************************************************
 * !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!! *
 ***********************************************************************************/

G_DEFINE_TYPE (XText, xtext, GTK_TYPE_WIDGET);

GtkWidget *
xtext_new (void)
{
	return GTK_WIDGET (g_object_new (XTEXT_TYPE, NULL));
}

static void
unrealize (GtkWidget *widget)
{
	XText *xtext = XTEXT (widget);
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	GtkClipboard *clipboard;

	backend_deinit (xtext);

	/* Withdraw PRIMARY selection */
	clipboard = gtk_widget_get_clipboard (widget, GDK_SELECTION_PRIMARY);
	if (gtk_clipboard_get_owner (clipboard) == G_OBJECT (widget)) {
		gtk_clipboard_clear (clipboard);
	}

	gdk_cursor_unref (priv->ibeam_cursor);
	priv->ibeam_cursor = NULL;

	gdk_cursor_unref (priv->hand_cursor);
	priv->hand_cursor = NULL;

	gdk_cursor_unref (priv->resize_cursor);
	priv->resize_cursor = NULL;

	GTK_WIDGET_CLASS (parent_class)->unrealize (widget);
}

/*
 * This signal handler handles setting the colormap for the xtext widget.
 * If we can get an ARGB colormap, then we have half of what is necessary for
 * real transparency to work.
 */
static void
screen_changed (GtkWidget *widget,
                GdkScreen *old_screen,
		gpointer   data)
{
	XText       *xtext;
	XTextPriv   *priv;
	GdkScreen   *screen;
	GdkColormap *cmap;

	xtext = XTEXT (widget);
	priv = XTEXT_GET_PRIVATE (xtext);

	screen = gtk_widget_get_screen (widget);
	cmap = gdk_screen_get_rgba_colormap (screen);
	if (cmap) {
		priv->has_argb_visual = TRUE;
	} else {
		cmap = gdk_screen_get_rgb_colormap (screen);
		priv->has_argb_visual = FALSE;
	}

	gtk_widget_set_colormap (widget, cmap);
	if (xtext->transparent) {
		gtk_widget_queue_draw (widget);
	}
}

static void
composited_changed (GtkWidget *widget,
                    gpointer   data)
{
	XText     *xtext;
	XTextPriv *priv;

	xtext = XTEXT (widget);
	priv = XTEXT_GET_PRIVATE (widget);

	priv->has_composite = gtk_widget_is_composited (widget);
	if (xtext->transparent) {
		gtk_widget_queue_draw (widget);
	}
}

GtkAdjustment *
xtext_get_adjustment (XText *xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	return priv->adj;
}

void
xtext_set_palette (XText    *xtext,
                   GdkColor  palette[])
{
	XTextPriv *priv;
	int i;
	GdkColor col;

	priv = XTEXT_GET_PRIVATE (xtext);

	for (i = 0; i < XTEXT_N_COLORS; i++) {
		xtext->palette[i] = palette[i].pixel;
	}

	if (GTK_WIDGET_REALIZED (xtext)) {
		xtext_set_fg (xtext, priv->fgc, XTEXT_FG);
		xtext_set_bg (xtext, priv->fgc, XTEXT_BG);
		xtext_set_fg (xtext, priv->bgc, XTEXT_BG);

		col.pixel = xtext->palette[XTEXT_MARKER];
		gdk_gc_set_foreground (priv->marker_gc, &col);
	}
	priv->foreground_color = XTEXT_FG;
	priv->background_color = XTEXT_BG;
}

void
xtext_set_tint (XText *xtext,
                int    tint)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	priv->tint = tint;
}

static void
backend_font_close (XText *xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	pango_font_description_free (priv->font->font_desc);
}

static void
backend_init (XText *xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	if (priv->layout == NULL) {
		priv->layout = gtk_widget_create_pango_layout (GTK_WIDGET (xtext), 0);
		if (priv->font) {
			pango_layout_set_font_description (priv->layout, priv->font->font_desc);
		}
	}
}

static void
backend_deinit (XText *xtext)
{
	XTextPriv *priv = XTEXT_GET_PRIVATE (xtext);
	if (priv->layout) {
		g_object_unref (priv->layout);
		priv->layout = NULL;
	}
}
