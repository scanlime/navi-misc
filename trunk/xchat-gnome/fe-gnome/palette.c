/* X-Chat
 * Copyright (C) 1998 Peter Zelezny.
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gnome.h>
#include "../common/xchat.h"
#include "../common/util.h"
#include "../common/cfgfiles.h"


GdkColor colors[] = {
	{0, 0xcf3c, 0xcf3c, 0xcf3c}, /* 0  white */
	{0, 0x0000, 0x0000, 0x0000}, /* 1  black */
	{0, 0x0000, 0x0000, 0xcccc}, /* 2  blue */
	{0, 0x0000, 0xcccc, 0x0000}, /* 3  green */
	{0, 0xdddd, 0x0000, 0x0000}, /* 4  red */
	{0, 0xaaaa, 0x0000, 0x0000}, /* 5  light red */
	{0, 0xbbbb, 0x0000, 0xbbbb}, /* 6  purple */
	{0, 0xffff, 0xaaaa, 0x0000}, /* 7  orange */
	{0, 0xeeee, 0xdddd, 0x2222}, /* 8  yellow */
	{0, 0x3333, 0xdede, 0x5555}, /* 9  green */
	{0, 0x0000, 0xcccc, 0xcccc}, /* 10 aqua */
	{0, 0x3333, 0xeeee, 0xffff}, /* 11 light aqua */
	{0, 0x0000, 0x0000, 0xffff}, /* 12 blue */
	{0, 0xeeee, 0x2222, 0xeeee}, /* 13 light purple */
	{0, 0x7777, 0x7777, 0x7777}, /* 14 grey */
	{0, 0x9999, 0x9999, 0x9999}, /* 15 light grey */
	{0, 0xa4a4, 0xdfdf, 0xffff}, /* 16 marktext Back (blue) */
	{0, 0x0000, 0x0000, 0x0000}, /* 17 marktext Fore (black) */
	{0, 0xdf3c, 0xdf3c, 0xdf3c}, /* 18 foreground (white) */
	{0, 0x0000, 0x0000, 0x0000}, /* 19 background (black) */
	{0, 0x8c8c, 0x1010, 0x1010}, /* 20 tab New Data (dark red) */
	{0, 0x0000, 0x0000, 0xffff}, /* 21 tab Nick Mentioned (blue) */
	{0, 0xf5f5, 0x0000, 0x0000}, /* 22 tab New Message (red) */
	{0, 0x9999, 0x9999, 0x9999}, /* 23 away user (grey) */
};
#define MAX_COL 23
