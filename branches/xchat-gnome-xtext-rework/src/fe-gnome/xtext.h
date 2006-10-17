#ifndef __XTEXT_H__
#define __XTEXT_H__

#include <gtk/gtkadjustment.h>

#ifdef USE_SHM
#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

#define XTEXT_TYPE              (xtext_get_type ())
#define XTEXT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), XTEXT_TYPE, XText))
#define XTEXT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), XTEXT_TYPE, XTextClass))
#define IS_XTEXT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), XTEXT_TYPE))
#define IS_XTEXT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), XTEXT_TYPE))
#define XTEXT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), XTEXT_TYPE, XTextClass))

#define ATTR_BOLD      '\002'
#define ATTR_COLOR     '\003'
#define ATTR_BEEP      '\007'
#define ATTR_RESET     '\017'
#define ATTR_REVERSE   '\026'
#define ATTR_ITALICS   '\035'
#define ATTR_UNDERLINE '\037'

/* these match palette.h */
#define XTEXT_MIRC_COLORS 32
#define XTEXT_MARK_FG     32
#define XTEXT_MARK_BG     33
#define XTEXT_FG          34
#define XTEXT_BG          35
#define XTEXT_MARKER      36
#define XTEXT_N_COLORS    37

#include "../common/xchat.h"

typedef struct _XText      XText;
typedef struct _XTextClass XTextClass;
typedef struct _XTextPriv  XTextPriv;
typedef struct textentry textentry;

typedef struct {
	XText *xtext;

	gfloat old_value; /* last known adj->value */
	textentry *text_first;
	textentry *text_last;
	guint16 grid_offset[256];

	/* this basically describes the last rendered selection */
	textentry *last_ent_start;
	textentry *last_ent_end;
	int last_offset_start;
	int last_offset_end;

	int last_pixel_pos;

	int pagetop_line;
	int pagetop_subline;
	textentry *pagetop_ent; /* what's at xtext->adj->value */
	char laststamp[NICKLEN];

	int num_lines;
	int indent; /* position of separator (pixels) from left */

	textentry *marker_pos;

	/* window size when last rendered */
	int window_width;
	int window_height;

	unsigned int time_stamp:1;
	unsigned int scrollbar_down:1;
	unsigned int needs_recalc:1;
	unsigned int grid_dirty:1;
	unsigned int marker_seen:1;
	unsigned int reset_marker_pos:1;
} xtext_buffer;

struct _XText
{
	GtkWidget widget;

	xtext_buffer *buffer;
	xtext_buffer *orig_buffer;
	xtext_buffer *selection_buffer;

	gulong palette[XTEXT_N_COLORS];

	guint scroll_tag; /* marking-scroll timeout */
	gulong vc_signal_tag; /* signal handler for "value_changed" adj */

	int select_start_adj; /* the adj->value when the selection started */
	int select_start_x;
	int select_start_y;
	int select_end_x;
	int select_end_y;

	char num[8]; /* for parsing mirc color */
	int nc; /* offset into xtext->num */

	textentry *hilight_ent;
	int hilight_start;
	int hilight_end;

	int fontsize;
	int space_width; /* width (pixels) of the space " " character */
	int stamp_width; /* width of timestamp string */
	int max_auto_indent;

	unsigned char scratch_buffer[4096];

	int (*urlcheck_function) (GtkWidget * xtext, char *word, int len);

	int jump_out_offset; /* point at which to stop rendering */
	int jump_in_offset; /* "" start rendering */

	int ts_x; /* ts origin for ->bgc GC */
	int ts_y;

	int clip_x; /* clipping (x directions) */
	int clip_x2; /* from x to x2 */

	int clip_y; /* clipping (y directions) */
	int clip_y2; /* from y to y2 */

	unsigned int auto_indent:1;
	unsigned int moving_separator:1;
	unsigned int word_or_line_select:1;
	unsigned int color_paste:1;
	unsigned int parsing_backcolor:1;
	unsigned int parsing_color:1;
	unsigned int backcolor:1;
	unsigned int button_down:1;
	unsigned int hilighting:1;
	unsigned int bold:1;
	unsigned int underline:1;
	unsigned int italics:1;
	unsigned int transparent:1;
	unsigned int marker:1;
	unsigned int shaded:1;
	unsigned int dont_render:1;
	unsigned int dont_render2:1;
	unsigned int cursor_hand:1;
	unsigned int cursor_resize:1;
	unsigned int skip_border_fills:1;
	unsigned int skip_stamp:1;
	unsigned int render_hilights_only:1;
	unsigned int in_hilight:1;
	unsigned int un_hilight:1;
	unsigned int avoid_trans:1;
	unsigned int indent_changed:1;

	gchar *current_word;
};

struct _XTextClass
{
	GtkWidgetClass parent_class;
	void (*word_click) (XText * xtext, char *word, GdkEventButton * event);
	void (*word_enter) (XText * xtext, char *word);
	void (*word_leave) (XText * xtext, char *word);
};

void xtext_append (xtext_buffer *buf, unsigned char *text, int len);
void xtext_append_indent (xtext_buffer *buf, unsigned char *left_text, int left_len, unsigned char *right_text, int right_len);
void xtext_selection_clear_full (xtext_buffer *buf);
int xtext_set_font (XText *xtext, char *name);
void xtext_set_background (XText * xtext, GdkPixmap * pixmap, gboolean trans);
void xtext_clear (xtext_buffer *buf);
void xtext_save (XText * xtext, int fh);
void xtext_refresh (XText * xtext, int do_trans);
textentry *xtext_search (XText * xtext, const gchar *text, textentry *start, gboolean case_match, gboolean backward);
void xtext_reset_marker_pos (XText *xtext);
void xtext_check_marker_visibility(XText *xtext);

gboolean xtext_is_empty (xtext_buffer *buf);
typedef void (*XTextForeach) (XText *xtext, unsigned char *text, void *data);
void xtext_foreach (xtext_buffer *buf, XTextForeach func, void *data);

void xtext_set_indent (XText *xtext, gboolean indent);
void xtext_set_max_indent (XText *xtext, int max_auto_indent);
void xtext_set_max_lines (XText *xtext, int max_lines);
void xtext_set_show_marker (XText *xtext, gboolean show_marker);
void xtext_set_time_stamp (xtext_buffer *buf, gboolean timestamp);
void xtext_set_urlcheck_function (XText *xtext, int (*urlcheck_function) (GtkWidget *, char *, int));

xtext_buffer *xtext_buffer_new (XText *xtext);
void xtext_buffer_free (xtext_buffer *buf);
void xtext_buffer_show (XText *xtext, xtext_buffer *buf, int render);
void xtext_copy_selection (XText *xtext);

/***********************************************************************************
 * !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!!    CRUFT BARRIER    !!!! *
 ***********************************************************************************/

GType          xtext_get_type            (void) G_GNUC_CONST;
GtkWidget     *xtext_new                 (void);

GtkAdjustment *xtext_get_adjustment      (XText    *xtext);
void           xtext_set_palette         (XText    *xtext,
                                          GdkColor  palette[]);
void           xtext_set_tint            (XText    *xtext,
                                          int       tint);

#endif
