/*
 * FieldNetwork.cpp - annie::MultiLayerNetwork, customized for use with the field sensors.
 *                    Currently this means we're using 3 layers instead of two, and a linear
 *                    activation rather than sigmoid.
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

#include <FieldNetwork.h>
#include <math.h>

FieldNetwork::FieldNetwork(void) : MultiLayerNetwork(8) {
  initLayers();
  initTransferFunctions();
}

FieldNetwork::FieldNetwork(const char *filename) : MultiLayerNetwork(filename) {
  initTransferFunctions();
}

void FieldNetwork::initLayers(void) {
  addLayer(8);
  addLayer(8);
  addLayer(3);
  connectLayer(0);
  connectLayer(1);
  connectLayer(2);
}

real FieldNetwork::transfer(real x) {
  return x + 0.5;
}

real FieldNetwork::dtransfer(real x) {
  return 1.0;
}

void FieldNetwork::initTransferFunctions(void) {
  /*
  setActivationFunction(1, &transfer, &dtransfer);
  setActivationFunction(2, &transfer, &dtransfer);
  setActivationFunction(3, &transfer, &dtransfer);
  */
}

/* The End */

  
