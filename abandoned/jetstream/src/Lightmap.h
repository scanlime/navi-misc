/*
 * Lightmap.h - Precomputed lightmaps using Radiosity
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

#ifndef _H_LIGHTMAP
#define _H_LIGHTMAP

#include "Lightmap.h"
#include "JetCOW.h"
#include "PGTexture.h"
#include <GL/gl.h>


class Lightmap : public Texture, public JetCOWObject {
 public:
  virtual void revert(void);
  virtual void commit(void);  

  virtual void bind(void);
  virtual void render(void);

  /* Never use these directly! */
  Lightmap(JetCOW *cow, Sint32 id=0, const char *type="Lightmap");
  virtual ~Lightmap(void);

 protected:
  GLuint texture;
};

#endif /* _H_LIGHTMAP */

/* The End */
