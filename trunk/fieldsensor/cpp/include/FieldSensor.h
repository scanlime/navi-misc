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

class FieldSensor {
 public:
  FieldSensor(const char *serialPort="/dev/ttyS0");
  ~FieldSensor(void);

  void readPacket(double *packet);

 private:
  int fd;
  void sendSlowly(const char *str);
  void reset(void);
  void boot(void);
  void waitForData(void);
};

#endif /* __H_FIELDSENSOR */

/* The End */

  
