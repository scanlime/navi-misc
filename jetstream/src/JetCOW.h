/*
 * JetCOW.h - Interfaces and file format for the Jetstream Copy-On-Write format.
 *            This format is implemented as a layer on top of libdb3
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

#ifndef _H_JETCOW
#define _H_JETCOW

#include <string>
#include <map>
#include <exception>
#include <vector>
#include "Util.h"
#include "Mutex.h"

class Db;
class Dbt;
class JetCOWObject;
class JetCOWDictionary;
class SceneNode;
class SplineSceneNode;
class Scene;

/*
 * The JetCOW format is a B-Tree libdb3 database with restrictions placed on the
 * format of the keys and values.
 *
 * Each database key is a 4-byte object ID number in network byte order.
 *
 * The database value always starts with a null-terminated object type string.
 * The rest of the value is handled depending on that object type string. All data stored in the
 * database should use network byte order where applicable.
 *
 * There is always a JetCOWDictionary with ID 0 containing at least the following keys:
 *
 *   "FormatMagic"       - Should always equal "JetCOW Mark I"
 *   "FormatVersion"     - Currently should equal 1.0
 *   "ContentVersion"    - Increments every time any page in the file is committed
 *   "NextID"            - The next node ID in the sequence of IDs to allocate
 *   "Generation"        - The number of times this JetCOW has been forked
 *   "ParentFilename"    - Path and filename of the parent JetCOW if Generation > 0
 *   "ParentVersion"     - The contentVersion of the parent JetCOW if Generation > 0
 */
class JetCOW {
  friend class JetCOWObject;

 public:
  JetCOW(const char *filename, bool readOnly=true, bool create=false, Uint32 cacheSize=4*1024*1024, Uint32 pageSize=4*1024);
  ~JetCOW();

  /* Make a copy-on-write fork of the JetCOW */
  JetCOW *fork(const char *newFilename, Uint32 cacheSize=4*1024*1024, Uint32 pageSize=8*1024);
  
  const std::string &getFilename(void) const;

  /* This is a wrapper function that constructs the JetCOWObject but insures that
   * each unique object only has one reference counted instance in memory.
   * 
   * Possible values for 'id':
   *     -1 : allocate a new ID, creates an object of the specified type.
   *     >0 : Use this id, load existing data- checks that the object is
   *          a subclass of the specified type.
   *  other : reserved or used internally
   *          (id==0 loads no existing data but keeps the zero id, for creating the header)
   *
   */
  JetCOWObject *checkout(const std::string type, Sint32 id=-1);

  /* Wrappers around checkout(), used from python */
  JetCOWDictionary *checkoutDict(Sint32 id);
  JetCOWDictionary *newDict(std::string type);
  SceneNode *checkoutSceneNode(Sint32 id);
  SplineSceneNode *newSpline();
  SceneNode *newSceneNode(std::string type);
  Scene *checkoutScene(Sint32 id);
  Scene *newScene(void);

  /* Commit all open object instances, and flush the database to disk */
  virtual void flush(void);

  JetCOWDictionary *getHeader(void);

 private:
  JetCOWDictionary *header;
  std::string filename;
  Db *dbp; 
  bool readOnly;
  JetCOW *parent;
  std::map<Sint32,JetCOWObject*> objectInstances;

  /* For the JetCOWObject's use: */
  void put(Sint32 id, Dbt &value);
  void get(Sint32 id, Dbt &value);
};


/* Abstract base class for an object that may be stored in the JetCOW. */
class JetCOWObject {
 public:
  /* NOTE: A JetCOWObject is *never* created directly, only via the JetCOW's
   *       checkout method which performs reference counting.
   */

  void ref(void);
  void unref(void);

  /* Never use these directly! */
  JetCOWObject(JetCOW *cow, Sint32 id, const char *type="JetCOWObject");
  virtual ~JetCOWObject();

  virtual void revert(void);
  virtual void commit(void);      /* Note: this commits to the libdb cache, not to disk */

  Sint32 id;
  JetCOW *cow;
  std::string type;
  bool dirty;

 protected:
  /* All access to the database value should be through these functions that
   * handle writing and reading the object type string.
   */
  void put(std::string &buffer);
  void get(std::string &buffer, int minSize=0);

 private:
  int refcount;
};


/*
 * This is a page that can hold simple key-value pairs. It's used for the
 * JetCOW header, and may be useful anywhere else a very simple yet extensible
 * format is needed. 
 *
 * The format is as follows, for each key/value pair:
 *   - The key value in ASCII or UTF-8 encoding
 *   - A zero character
 *   - A byte indicating the value type: currently 'i', 's', or 'f' for int (u32n), string, or float respectively.
 *   - Padding until the next 32-bit boundary
 *   - The value (representation depends on valueType)
 */
class JetCOWDictionary : public JetCOWObject, public Mutex {
 public:
  void setAttr(char *name, int value);
  void setAttr(char *name, Uint32 value);
  void setAttr(char *name, const char *value);
  void setAttr(char *name, char *value);
  void setAttr(char *name, float value);
  void setAttr(char *name, double value);
  void setAttr(char *name, std::string &value);
  void setAttrInt(char *name, int value);
  void setAttrStr(char *name, std::string value);
  void setAttrFloat(char *name, float value);

  int getAttrInt(char *name);
  char *getAttrStr(char *name);
  std::string getAttrSTLStr(char *name);
  float getAttrFloat(char *name);
  void delAttr(char *name);

  virtual void revert(void);
  virtual void commit(void);

  JetCOWDictionary &operator = (JetCOWDictionary &d);

  /* Never use these directly! */
  JetCOWDictionary(JetCOW *cow, Sint32 id, const char *type="JetCOWDictionary");
  virtual ~JetCOWDictionary();  

 protected:
  /* Subclasses can define these to maintain a cache of frequently used values in the dict.
   * Note that these must use the protected variants of setAttr/getAttr below to avoid infinite recursion.
   */
  virtual void loadCachedValues(void);
  virtual void saveCachedValues(void);

  void setAttrProtected(char *name, int value);
  void setAttrProtected(char *name, Uint32 value);
  void setAttrProtected(char *name, const char *value);
  void setAttrProtected(char *name, char *value);
  void setAttrProtected(char *name, float value);
  void setAttrProtected(char *name, double value);
  void setAttrProtected(char *name, std::string &value);

  int getAttrIntProtected(char *name);
  char *getAttrStrProtected(char *name);
  float getAttrFloatProtected(char *name);
  void delAttrProtected(char *name);
 
 private:
  void addKeyHeader(std::string &page, std::string key, char type);
  std::map<std::string,Sint32> intValues;            /* Stored here in host byte order */
  std::map<std::string,std::string> stringValues;
  std::map<std::string,float> floatValues;
};


class JetCOWVector : public JetCOWObject, private Mutex {
 public:
  /* JetCOW hooks - Never use these directly! */
  JetCOWVector(JetCOW *cow, Sint32 id, const char *type="JetCOWVector");
  virtual ~JetCOWVector();
  
  virtual void revert(void);
  virtual void commit(void);

  std::vector<JetCOWObject*> vec;
  std::string requiredType;

 private:
  void clearAndUnref(void);
};

#endif /* _H_JETCOW */

/* The End */
