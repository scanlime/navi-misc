/* autoaway.h
 *
 * An auto away plugin for xchat-gnome.
 *
 * Copyright (C) 2005 Isak Savo <isak.savo@gmail.com>
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
#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <glib.h>
#define AUTOAWAY_VERSION "0.3"

//#define DEBUG

#ifdef DEBUG
//# define trace(...) do{ if (xchat_get_context(ph)) xchat_printf(ph, __VA_ARGS__); }while(0);
# define trace(...) do { g_print (__VA_ARGS__); g_print ("\n"); }while (0);
#else
# define trace(...)
#endif /* DEBUG */


#endif /* AUTOAWAY */
