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

#include <pthread.h>
#include <list>

#include <FieldNetwork.h>

/* Linear algebra classes borrowed from Jetstream */
#include <Vector.h>
#include <Matrix.h>


class FieldSensor {
 public:
  FieldSensor(const char *serialPort="/dev/ttyS0", const char *netFile="data/current.net");
  ~FieldSensor(void);
  
  VECTOR getRawData(void);
  Vector3 getPosition(void);

 private:
  /* Low-level communications */
  int fd;
  pthread_t thread_id;
  static void *readerThread(void *pthis);
  void sendSlowly(const char *str);
  void reset(void);
  void boot(void);
  void waitForData(void);
  VECTOR rawData;
  Vector3 unfilteredPosition;
  bool newData;

  /* Neural net */
  TwoLayerNetwork net;

  /* Kalman filter variables:
   *  x: State vector
   *  P: Estimated error covariance
   *  Q: Process noise covariance
   *  R: Measurement noise covariance
   *  K: filter gain
   *  z: New measurement
   */
  Vector3 x, z, P, Q, R, K, A;
};

#endif /* __H_FIELDSENSOR */

/* The End */

  
