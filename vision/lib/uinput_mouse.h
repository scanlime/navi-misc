/*
 * uinput_mouse.h - A simple interface for emulating a mouse under linux using /dev/uinput
 *
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __UINPUT_MOUSE__H_
#define __UINPUT_MOUSE__H_

void uinput_mouse_init(const char *dev_name);
void uinput_mouse_move(int rel_x, int rel_y);
void uinput_mouse_move_subpixel(float rel_x, float rel_y);

#endif /* __UINPUT_MOUSE__H_ */

/* The End */
