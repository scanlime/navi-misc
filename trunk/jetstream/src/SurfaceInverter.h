/*
 * SurfaceInverter.h - A surface modifier that flips a surface inside out
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

#ifndef _H_SURFACEINVERTER
#define _H_SURFACEINVERTER

#include "Surface.h"
#include "SurfaceGenerator.h"


class SurfaceInverter : public SurfaceModifier {
 public:
  virtual void push(void);
  virtual void pop(void);
  virtual void f(SurfacePoint &p);

  /* JetCOW hooks - never call directly */
  SurfaceInverter(JetCOW *cow, Sint32 id,  const char *type="SurfaceInverter");
};


#endif /* _H_SURFACEINVERTER */

/* The End */
