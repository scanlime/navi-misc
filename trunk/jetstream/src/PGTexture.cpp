/*
 * PGTexture.cpp - Implementation for loading textures from picogui themes
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

#include "PGTexture.h"
#include "EmbeddedPGserver.h"
#include <stdio.h>
extern "C" {
#include <pgserver/common.h>
#include <pgserver/handle.h>
#include <pgserver/svrtheme.h>
#include <pgserver/video.h>
#include <pgserver/pgstring.h>
#include <pgserver/gl.h>
}

class TextureException : public std::exception {
public:
  TextureException(const char *object) throw() {
    s = "Error loading texture from theme object: ";
    s += object;
  }
  ~TextureException() throw() {}
  virtual const char *what(void) const throw() {
    return s.c_str();
  }
private:
  std::string s;
};

PGTexture::PGTexture(const char *theme_object, bool clamped) {
  s16 id;
  handle hbitmap;
  g_error e;

  EmbeddedPGserver::getInstance()->lock();

  /* Find the numeric ID associated with this theme object.
   * pgstring_tmpwrap is a wrapper that temporarily converts a given
   * C string into an ASCII pgstring object.
   */
  find_named_thobj(pgstring_tmpwrap(theme_object), &id);

  /* Find the handle of the loaded bitmap */
  hbitmap = theme_lookup(id, PGTH_P_BITMAP1);

  /* Dereference the handle. Note that if the theme is unloaded while
   * this class instance still exists, calling bind() will crash!
   * This could be fixed by moving this dereference step into bind(),
   * but at a small speed penalty.
   */
  e = rdhandle((void**) &bitmap, PG_TYPE_BITMAP, -1, hbitmap);
  if (iserror(e)) {
    EmbeddedPGserver::getInstance()->unlock();
    throw PicoGUIException(e);
  }
  if (!bitmap) {
    EmbeddedPGserver::getInstance()->unlock();
    throw TextureException(theme_object);
  }
  EmbeddedPGserver::getInstance()->unlock();

  if (!clamped) {
    /* PicoGUI clamps the texture by default, we'd rather wrap it */
    bind();
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  }
}

void PGTexture::bind(void) {
  EmbeddedPGserver::getInstance()->lock();
  gl_bind_texture(bitmap);
  EmbeddedPGserver::getInstance()->unlock();
}


/* The End */
