/*
 * FieldSensor.h - Header for electric field sensor interface
 *
 * Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
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
 * Contributors:
 * 
 * 
 * 
 */

#ifndef __H_FIELDSENSOR
#define __H_FIELDSENSOR

#include <list>

/* annie is the neural net library we use */
#include <annie.h>
using namespace annie;

/* Vector classes borrowed from Jetstream */
#include <Vector.h>


class FieldSensor {
 public:
  FieldSensor(const char *serialPort="/dev/ttyS0", const char *netFile="data/current.net");
  ~FieldSensor(void);

  /* Read a packet- if 'blocking' is false and no new data is
   * available this returns the previous packet received.
   */
  VECTOR readPacket(bool blocking=true);

  /* Like readPacket, but also decodes the packet 
   * into a position using the supplied neural net file.
   */
  Vector3 rawPosition(bool blocking=true);

  /* Put the position through a filter to eliminate most of the noise. */
  Vector3 readPosition(bool blocking=false);

 private:
  /* Low-level communications */
  int fd;
  void sendSlowly(const char *str);
  void reset(void);
  void boot(void);
  void waitForData(void);
  VECTOR resultBuffer;

  /* Neural net */
  TwoLayerNetwork net;

  /* Filter */
  Vector3 filterPosition;
};

#endif /* __H_FIELDSENSOR */

/* The End */

  
