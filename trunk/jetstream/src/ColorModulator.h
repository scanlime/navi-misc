/*
 * ColorModulator.h - Surface modifier for modulating the vertex coloring of a surface
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

#ifndef _H_COLORMODULATOR
#define _H_COLORMODULATOR

#include "Surface.h"
#include "SurfaceGenerator.h"


class ColorModulator : public SurfaceModifier {
 public:
  virtual void f(SurfacePoint &p, SurfaceQuadtreeNode *qn);

  /* Dictionary keys: 
   *    Red, Green, Blue - floating point color modulations for each channel
   */

  /* JetCOW hooks - never call directly */
  ColorModulator(JetCOW *cow, Sint32 id,  const char *type="ColorModulator");
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);  

 protected:
  float color[3];
};


#endif /* _H_COLORMODULATOR */

/* The End */
