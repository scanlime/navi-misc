/*
 * EmbeddedPGserver.c - Implementation of a simple wrapper around libpgserver
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

extern "C" {
#include <pgserver/common.h>
#include <pgserver/init.h>
#include <pgserver/os.h>
#include <pgserver/configfile.h>
#include <pgserver/svrtheme.h>
#include <pgserver/requests.h>
#include <pgserver/gl.h>
}

#include "Util.h"
#include "EmbeddedPGserver.h"

EmbeddedPGserver *EmbeddedPGserver::instance = NULL;

EmbeddedPGserver::EmbeddedPGserver() {
  g_error e;
  instance = this;

  /* Set defaults */
  setParam("pgserver", "video", "sdlgl");
  setParam("pgserver", "font_engine", "ftgl");
  setParam("font-ftgl", "path", "data/fonts");
  setParam("pgserver", "dragsolid", "1");
  setParam("video-sdlgl", "caption", "jetstream (%dx%d)");
  setParam("opengl", "continuous", "1");
  setParam("pgserver", "appmgr", "panel");
  setParam("pgserver", "themedir", "data");
  setParam("pgserver", "themes", "textures.th holographic.th");
}

void EmbeddedPGserver::init(void) {
  g_error e;

  /* Load without a config file or args */
  e = pgserver_init(PGINIT_NO_CONFIGFILE | PGINIT_NO_COMMANDLINE, 0, NULL);
  if (iserror(e))
    throw PicoGUIException(e);
  
  pgserver_mainloop_start();
  pg_clock = 0;

  /* Create a connection buffer for our local 'client' */
  e = request_client_create(1);
  if (iserror(e))
    throw PicoGUIException(e);

  /* Add our handler to the main loop, to be executed at the drawUI priority */
  mainHandler = new MainLoopHandler<EmbeddedPGserver>(this,
						      Engine::priority_drawUI,
						      &EmbeddedPGserver::mainloopIteration);
}  

void EmbeddedPGserver::shutdown() {
  g_error e;

  delete mainHandler;
  e = pgserver_shutdown();
  if (iserror(e))
    throw PicoGUIException(e);
}  

const char *EmbeddedPGserver::getParam(const char *section, const char *param, const char *def) {
  const char *s;
  lock();
  s = get_param_str(section,param,def);
  unlock();
  return s;
}

int EmbeddedPGserver::getParam(const char *section, const char *param, int def) {
  int i;
  lock();
  i = get_param_int(section,param,def);
  unlock();
  return i;
}

void EmbeddedPGserver::setParam(const char *section, const char *param, const char *value) {
  g_error e;

  lock();
  e = set_param_str(section,param,value);
  unlock();
  if (iserror(e))
    throw PicoGUIException(e);
}

void EmbeddedPGserver::mainloopIteration(void) {
  g_error e;

  lock();
  /* Override picogui's wallclock time with our own */
  pg_clock += (u32) (Engine::getInstance()->frameTime * 1000);
  pg_ticks_override(pg_clock);

  Engine::getInstance()->running = Engine::getInstance()->running && pgserver_mainloop_is_running();

  e = pgserver_mainloop_iteration();
  unlock();
  if (iserror(e))
    throw PicoGUIException(e);
}

PicoGUIException::PicoGUIException(g_error e) throw() {
  s = "PicoGUI server exception: ";
  s += errortext(e);
}

PicoGUIException::~PicoGUIException() throw() {}

const char *PicoGUIException::what(void) const throw() {
  return s.c_str();
}

EmbeddedPGserver *EmbeddedPGserver::getInstance(void) {
  return instance;
}

void EmbeddedPGserver::write(PyObject *req) {
  char *reqbuffer;
  int reqbuffer_len;
  struct request_data d;
  g_error e;
  memset(&d,0,sizeof(d));

  PyString_AsStringAndSize(req, &reqbuffer, &reqbuffer_len);
  d.in.req = (struct pgrequest*) reqbuffer;
  d.in.data = reqbuffer + sizeof(pgrequest);
  d.in.owner = 1;

  /* Errors here are encoded in the response rather
   * than being immediately converted to exceptions.
   */
  request_exec(&d);

  /* Stick the response in a buffer */
  responseBuffer.append((char*) &d.out.response, d.out.response_len);
  if (d.out.response_data)
    responseBuffer.append((char*) d.out.response_data, d.out.response_data_len);
  if (d.out.free_response_data)
    g_free(d.out.response_data);  
}

PyObject *EmbeddedPGserver::read(int bytes) {
  PyObject *str;

  if (bytes > responseBuffer.size())
    bytes = responseBuffer.size();
  str = PyString_FromStringAndSize(responseBuffer.c_str(), bytes);
  responseBuffer.erase(0,bytes);

  return str;
}

bool EmbeddedPGserver::getShowFPS(void) {
  return gl_global.showfps;
}

void EmbeddedPGserver::setShowFPS(bool f) {
  gl_global.showfps = f;
}

/* The End */
