#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gdk-pixdata.h>

#include "../pixmaps/inline_pngs.h"

GdkPixbuf *pix_purple;
GdkPixbuf *pix_red;
GdkPixbuf *pix_op;
GdkPixbuf *pix_hop;
GdkPixbuf *pix_voice;

void
pixmaps_init (void)
{
	pix_purple = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/purple.png", 0);
	if (!pix_purple)
		pix_purple = gdk_pixbuf_new_from_inline (-1, purplepng, FALSE, 0);

	pix_red = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/red.png", 0);
	if (!pix_red)
		pix_red = gdk_pixbuf_new_from_inline (-1, redpng, FALSE, 0);

	pix_op = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/op.png", 0);
	if (!pix_op)
		pix_op = gdk_pixbuf_new_from_inline (-1, oppng, FALSE, 0);

	pix_hop = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/hop.png", 0);
	if (!pix_hop)
		pix_hop = gdk_pixbuf_new_from_inline (-1, hoppng, FALSE, 0);

	pix_voice = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/voice.png", 0);
	if (!pix_voice)
		pix_voice = gdk_pixbuf_new_from_inline (-1, voicepng, FALSE, 0);
}
