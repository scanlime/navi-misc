/*
 * EmbeddedPGserver.h - Interface for a simple wrapper around libpgserver
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

#ifndef _H_EMBEDDEDPGSERVER
#define _H_EMBEDDEDPGSERVER

#include <exception>
#include <string>
extern "C" {
#include <pgserver/common.h>
#include <Python.h>
}
#include "ConfigProvider.h"
#include "Engine.h"
#include "Mutex.h"

class EmbeddedPGserver : public ConfigProvider, public Mutex {
 public:
  EmbeddedPGserver();

  /* ConfigProvider interface */
  virtual const char *getParam(const char *section, const char *param, const char *def);
  virtual int getParam(const char *section, const char *param, int def);
  virtual void setParam(const char *section, const char *param, const char *value);

  /* Initialize pgserver using the currently set config options */
  void init(void);
  void shutdown(void);

  /* We use a PyObject here instead of std::string because Boost's 
   * converters don't work on strings with zero bytes.
   */
  void write(PyObject *req);
  PyObject *read(int bytes);

  /* There will only be one EmbeddedPGserver, this returns it */
  static EmbeddedPGserver *getInstance(void);

  bool getShowFPS(void);
  void setShowFPS(bool f);

 private:
  void mainloopIteration(void);
  MainLoopHandler<EmbeddedPGserver> *mainHandler;
  u32 pg_clock;
  static EmbeddedPGserver *instance;
  std::string responseBuffer;
};

class PicoGUIException : public std::exception {
 public:
  PicoGUIException(g_error e) throw();
  ~PicoGUIException() throw();
  virtual const char *what(void) const throw();
 private:
  std::string s;
};


#endif /* _H_EMBEDDEDPGSERVER */

/* The End */
