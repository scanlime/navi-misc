/* 
 * main.cpp - The C++ entry point, immediately transfers
 *            control to startup.py after setting up the modules path.
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

#include <Python.h>

extern "C" void initJetEngine(void);

int main(int argc, char **argv) {
  Py_Initialize();
  PySys_SetArgv(argc,argv);

  /* Insert our JetEngine module as a builtin */
  PyImport_AppendInittab("JetEngine", initJetEngine);

  /* Set modules path */
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append('modules')");

  /* The rest of the init is written in python */
  PyRun_SimpleString("import startup");
}

/* The End */
