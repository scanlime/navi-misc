/*
 * PGTexture.h - Interface for loading textures from picogui themes
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

#ifndef _H_PGTEXTURE
#define _H_PGTEXTURE

#include "EmbeddedPGserver.h"
struct glbitmap;

class Texture {
 public:
  virtual void bind(void) = 0;
};

class PGTexture : public Texture {
 public:
  PGTexture(const char *theme_object, bool clamped=false);
  virtual void bind(void);

 private:
  struct glbitmap *bitmap;
};

#endif /* _H_TEXTURE */

/* The End */
