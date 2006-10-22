/*
 * palette.h - palette setting/saving functions
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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

#include <gtk/gtkwidget.h>

extern GdkColor colors[];
extern const GdkColor *color_schemes[];
extern const GdkColor *palette_schemes[];
extern GdkColor custom_colors[9];
extern GdkColor custom_palette[32];

void load_colors (int selection);
void load_palette (int selection);
void palette_init (void);
void palette_save (void);
