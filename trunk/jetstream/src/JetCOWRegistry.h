/*
 * JetCOWRegistry.h - A registry of data types used by JetCOW
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

#ifndef _H_JETCOWREGISTRY
#define _H_JETCOWREGISTRY

#include <stdexcept>
#include <string>
#include <map>
#include <vector>
#include "JetCOW.h"


/******************************************************** Utility classes ***/

class JetCOWObjectTypeBase {
 public:
  virtual JetCOWObject *instantiate(JetCOW *cow, Sint32 id) = 0;
  std::vector<std::string> subclasses;
  const char *type;
  const char *base;
};

template <class T>
class JetCOWObjectType : public JetCOWObjectTypeBase {
 public:
  virtual JetCOWObject *instantiate(JetCOW *cow, Sint32 id) {
    return new T(cow,id);
  }
};


/******************************************************** JetCOWRegistry class ***/

class JetCOWRegistry {
 public:

  /* This class is a singleton */
  static JetCOWRegistry *getInstance(void);
  
  JetCOWObject *createObject(JetCOW *cow, Sint32 id, const std::string &type);

  /* Check if 'a' is a subclass of 'b' */
  bool isSubclass(const std::string &a, const std::string &b);

 private:
  JetCOWRegistry();

  /* Note: Multiple inheritance is not recommended for JetCOWObjects, and is not supported yet.
   */
  template<class T> void add(const char *type, const char *base=NULL) {
    /* Add the object */
    JetCOWObjectType<T> *o = new JetCOWObjectType<T>();
    typeMap[type] = o;
    o->type = type;
    o->base = base;

    /* propagate it across its parents' subclass lists */
    while (base) {
      JetCOWObjectTypeBase *tb = typeMap[base];
      tb->subclasses.push_back(type);
      base = tb->base;
    }      
  }

  static JetCOWRegistry *instance;
  std::map<std::string, JetCOWObjectTypeBase*> typeMap;
};


/******************************************************** Dynamic casting ***/

template <class T>
T *JetCast(std::string typeName, JetCOWObject *o) {
  if (!JetCOWRegistry::getInstance()->isSubclass(o->type, typeName))
    throw std::runtime_error(std::string("Unable to cast an object of type '") + o->type + 
			     "' to type '" + typeName + "'");
  return (T*) o;
}

#endif /* _H_JETCOWREGISTRY */

/* The End */
