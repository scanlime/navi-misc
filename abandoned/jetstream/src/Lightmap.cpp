/*
 * Lightmap.cpp - Precomputed lightmaps using Radiosity
 *
 *     References:
 *        http://freespace.virgin.net/hugo.elias/radiosity/radiosity.htm
 *        http://www.cs.bris.ac.uk/Tools/Reports/Ps/1994-reinhard.pdf
 *        http://www.cs.utah.edu/~schmelze/radiosity/proj/proj.html
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

#include "Lightmap.h"


void Lightmap::revert(void) {
}

void Lightmap::commit(void) {
}

void Lightmap::bind(void) {
}

void Lightmap::render(void) {
}

Lightmap::Lightmap(JetCOW *cow, Sint32 id, const char *type) : JetCOWObject(cow,id,type) {
}

Lightmap::~Lightmap(void) {
}


/* The End */
