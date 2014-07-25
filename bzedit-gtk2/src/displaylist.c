/*
 * displaylist.h - A drawable that stores itself to a display list before rendering
 *
 * BZEdit
 * Copyright (C) 2004 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "displaylist.h"

static void display_list_class_init (DisplayListClass *klass);
static void display_list_init       (DisplayList *dl);

GType
display_list_get_type (void)
{
  static GType dl_type = 0;

  if (!dl_type)
    {
      static const GTypeInfo dl_info =
      {
	sizeof (DisplayListClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    display_list_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (DisplayList),
	0,                  /* n preallocs */
	(GInstanceInitFunc) display_list_init,
      };

      dl_type = g_type_register_static (DRAWABLE_TYPE, "DisplayList", &dl_info, 0);
    }

  return dl_type;
}

static void
display_list_class_init (DisplayListClass *klass)
{
  DrawableClass *d_class;

  d_class = (DrawableClass*) klass;

  d_class->draw = display_list_draw;
}

static void
display_list_init (DisplayList *dl)
{
  dl->list = glGenLists (1);
  dl->dirty = TRUE;
}

void
display_list_build_list (DisplayList *dl)
{
  DisplayListClass *dlc = DISPLAY_LIST_CLASS (G_OBJECT_GET_CLASS (dl));

  if (dl->list == 0)
    dl->list == glGenLists (1);

  glNewList (dl->list, GL_COMPILE);
  dlc->draw_to_list (dl);
  glEndList ();
  if (dl->list != 0)
    dl->dirty = FALSE;
}

void
display_list_draw (Drawable *drawable, RenderState *rstate)
{
  DisplayList *dl = DISPLAY_LIST (drawable);

  if (dl->dirty)
    display_list_build_list (dl);
  glCallList (dl->list);
}
