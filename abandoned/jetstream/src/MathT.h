/*
 * MathT.h - Math tables
 *
 * Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
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

#ifndef _H_MATHT
#define _H_MATHT

class Math {
  public:
    Math();
    Math(int resolution);

    static float	cos(float n);
    static float	sin(float n);

    static Math		*getInstance();

  private:
    void		recompute();

    static Math		*singleton;
    float		*cost;
    float		*sint;
    int			resolution;
    float		scale;
};

#endif
