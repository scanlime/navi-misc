/*
 * remote.c - Remote control mode, an interface for automating
 *            Fyre rendering. Among other things, this is used
 *            to implement slave nodes in a cluster.
 *
 *            This communicates using a line oriented protocol
 *            with SMTP-like numeric response codes.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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

#include <stdio.h>
#include "animation.h"
#include "iterative-map.h"
#include "remote.h"

void    remote_main (IterativeMap*  map,
		     Animation*     animation,
		     gboolean       have_gtk)
{
  char line[1024];
  printf("220 Fyre rendering server ready\n");

  while (fgets(line, sizeof(line)-1, stdin)) {
    line[sizeof(line)-1] = '\0';

  }
}

/* The End */
