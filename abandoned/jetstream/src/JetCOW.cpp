/*
 * JetCOW.h - Implementation for the Jetstream Copy-On-Write format.
 *            See JetCOW.h for a description of the file format.
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

#include "JetCOW.h"
#include "JetCOWRegistry.h"
#include <string.h>
#include <malloc.h>
#include <db3/db_cxx.h>


/* Exception utility classes */
class JetCOWDBException : public std::exception {
 public:
  JetCOWDBException(JetCOW *cow, DbException &e) throw() {
    s = "JetCOW database error on file " + cow->getFilename() + ": " + e.what();
  }
  ~JetCOWDBException() throw() {};
  virtual const char *what(void) const throw() {
    return s.c_str();
  }    
private:
  std::string s;
};

class JetCOWFormatException : public std::exception {
 public:
  JetCOWFormatException(JetCOW *cow, const char *str) throw() {
    s = "JetCOW format error on file " + cow->getFilename() + ": " + str;
  }
  ~JetCOWFormatException() throw() {};
  virtual const char *what(void) const throw() {
    return s.c_str();
  }
 private:
  std::string s;
};


JetCOW::JetCOW(const char *filename, bool readOnly, bool create, Uint32 cacheSize, Uint32 pageSize) {
  const char *magic = "JetCOW Mark I";
  parent = NULL;
  this->filename = filename;
  this->readOnly = readOnly;

  try {
    dbp = new Db(NULL,0);
    dbp->set_cachesize(0,cacheSize,0);
    dbp->set_pagesize(pageSize);
    dbp->open(filename, NULL, DB_BTREE, 
	      DB_THREAD | (create ? (DB_CREATE | DB_TRUNCATE) : 0) | (readOnly ? DB_RDONLY : 0), 0);
  }
  catch (DbException &e) {
    throw JetCOWDBException(this, e);
  }

  /* The header is a dictionary fixed at ID 0 */
  header = (JetCOWDictionary*) checkout("JetCOWDictionary",0);
  if (create) {
    /* Initialize a new header */
    header->setAttr("FormatMagic"   ,magic);
    header->setAttr("FormatVersion" ,1.0f);
    header->setAttr("ContentVersion",0);
    header->setAttr("NextID"        ,1);
    header->setAttr("Generation"    ,0);
    header->commit();
  }
  else {
    /* Load the header (not done automatically, since the id is zero) */
    header->revert();
    if (strcmp(header->getAttrStr("FormatMagic"),magic))
      throw JetCOWFormatException(this, "Header isn't magic");

    /* If this isn't a first generation JetCOW, load the parent */
    if (header->getAttrInt("Generation") > 0) {
      parent = new JetCOW(header->getAttrStr("ParentFilename"));
      if (header->getAttrInt("ParentVersion") != parent->header->getAttrInt("ContentVersion"))
	throw JetCOWFormatException(this,"The parent of this database has been modified, this database is no longer valid");
    }
  }
}

JetCOW::~JetCOW() {
  if (parent)
    delete parent;
  header->unref();
  dbp->close(0);
  if (objectInstances.size()) {
    std::map<Sint32,JetCOWObject*>::iterator i;
    printf("Warning - JetCOW %s closed with object references still active:\n", filename.c_str());
    for (i=objectInstances.begin();i!=objectInstances.end();i++)
      printf("  %s(%d)\n", i->second->type.c_str(), i->second->id);
  }
}

/* Commit all open object instances, and flush the database to disk */
void JetCOW::flush(void) {
  std::map<Sint32,JetCOWObject*>::iterator i;
  for (i=objectInstances.begin();i!=objectInstances.end();i++)
    i->second->commit();
  dbp->sync(0);
}


/* Make a copy-on-write fork of the JetCOW */
JetCOW *JetCOW::fork(const char *newFilename, Uint32 cacheSize, Uint32 pageSize) {
  /* We can't fork unless this database is read-only */
  if (!readOnly)
    throw JetCOWFormatException(this, "Forking on read-write JetCOWs is not allowed");

  JetCOW *forked = new JetCOW(newFilename, false, true, cacheSize, pageSize);

  /* Copy the header, and modify it for this forked copy */
  *forked->header = *header;
  forked->header->setAttr("Generation"      ,header->getAttrInt("Generation")+1);
  forked->header->setAttr("ParentVersion"   ,header->getAttrInt("ContentVersion"));
  forked->header->setAttr("ContentVersion"  ,0);
  forked->header->setAttr("ParentFilename"  ,filename);
  forked->header->commit();

  /* Give the forked copy a new read-only instance of ourselves */
  forked->parent = new JetCOW(filename.c_str());

  return forked;
}
  
const std::string &JetCOW::getFilename(void) const {
  return filename;
}

void JetCOW::put(Sint32 id, Dbt &value) {
  try {
    Dbt key;
    id = jet_htonl(id);
    key = Dbt(&id, sizeof(id));
    dbp->put(NULL, &key, &value, 0);
  }
  catch (DbException &e) {
    throw JetCOWDBException(this, e);
  }
}

void JetCOW::get(Sint32 id, Dbt &value) {
  try {
    Dbt key;
    id = jet_htonl(id);
    key = Dbt(&id, sizeof(id));
    dbp->get(NULL, &key, &value, 0);

    /* If the returned value is NULL, the key didn't exist. If we're not the root JetCOW,
     * we recursively pass this request on to the parent. Otherwise, it's an error.
     */
    if (!value.get_data()) {
      if (parent)
	parent->get(id,value);
      else
	throw JetCOWFormatException(this, "Attempt to retrieve a nonexistant key");
    }
  }
  catch (DbException &e) {
    throw JetCOWDBException(this, e);
  }
}

void JetCOWObject::ref(void) {
  refcount++;
}

void JetCOWObject::unref(void) {
  refcount--;
  if (refcount <= 0) {
    cow->objectInstances.erase(id);
    delete this;
  }
}

JetCOWObject::JetCOWObject(JetCOW *cow, Sint32 id, const char *type) {
  this->id = id;
  this->cow = cow;
  this->type = type;
  refcount = 1;

  /* Objects need to start out dirty so newly created objects will save their defaults.
   * When an existing object is loaded, revert() will set this flag to false anyway.
   */
  dirty = true;

  if (id==-1) {
    /* Generate a new ID */
    this->id = cow->header->getAttrInt("NextID");
    cow->header->setAttr("NextID", this->id + 1);
    cow->header->commit();
  }
}

JetCOWObject::~JetCOWObject() {}

void JetCOWObject::put(std::string &buffer) {
  std::string s = type + '\0' + buffer;
  Dbt value((void*) s.c_str(),s.size());
  cow->put(id, value);
}

void JetCOWObject::get(std::string &buffer, int minSize) {
  Uint8 *valueData;
  Dbt value;
  value.set_flags(DB_DBT_MALLOC);
  cow->get(id, value);
  valueData = (Uint8*) value.get_data();

  /* Extract the type as a C string from the beginning of the buffer,
   * so it stops at the null.
   */
  type = std::string((char*) value.get_data());

  /* Extract the data right after the null and to the end of the buffer */
  buffer =  std::string(((char *) value.get_data()) + type.size() + 1,
			value.get_size() - type.size() - 1);

  free(value.get_data());
  if (buffer.size() < minSize)
    throw JetCOWFormatException(cow, "Buffer size in object is smaller than expected");
}

void JetCOWObject::revert(void) {
  /* Throw away the data, but this sets our object type correctly
   * which is necessary to identify the type of an arbitrary JetCOWObject on disk.
   */
  std::string buffer;
  get(buffer);
}

void JetCOWObject::commit(void) {
  /* Nothing to save */
}

JetCOWDictionary::JetCOWDictionary(JetCOW *cow, Sint32 id, const char *type) : JetCOWObject(cow,id,type) {
  if (id>0)
    revert();
}

JetCOWDictionary::~JetCOWDictionary() {
  commit();
}

void JetCOWDictionary::setAttrInt(char *name, int value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttrStr(char *name, std::string value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttrFloat(char *name, float value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttr(char *name, int value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttr(char *name, Uint32 value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttr(char *name, char *value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttr(char *name, const char *value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttr(char *name, float value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttr(char *name, double value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

void JetCOWDictionary::setAttr(char *name, std::string &value) {
  setAttrProtected(name,value);
  loadCachedValues();
}

int JetCOWDictionary::getAttrInt(char *name) {
  saveCachedValues();
  return getAttrIntProtected(name);
}

char *JetCOWDictionary::getAttrStr(char *name) {
  saveCachedValues();
  return getAttrStrProtected(name);
}

std::string JetCOWDictionary::getAttrSTLStr(char *name) {
  saveCachedValues();
  return getAttrStrProtected(name);
}

float JetCOWDictionary::getAttrFloat(char *name) {
  saveCachedValues();
  return getAttrFloatProtected(name);
}

void JetCOWDictionary::delAttr(char *name) {
  delAttrProtected(name);
  loadCachedValues();  
}

void JetCOWDictionary::setAttrProtected(char *name, int value) {
  lock();
  intValues[name] = value;
  dirty = true;
  unlock();
}

void JetCOWDictionary::setAttrProtected(char *name, Uint32 value) {
  lock();
  intValues[name] = value;
  dirty = true;
  unlock();
}

void JetCOWDictionary::setAttrProtected(char *name, char *value) {
  lock();
  stringValues[name] = value;
  dirty = true;
  unlock();
}

void JetCOWDictionary::setAttrProtected(char *name, const char *value) {
  lock();
  stringValues[name] = value;
  dirty = true;
  unlock();
}

void JetCOWDictionary::setAttrProtected(char *name, float value) {
  lock();
  floatValues[name] = value;
  dirty = true;
  unlock();
}

void JetCOWDictionary::setAttrProtected(char *name, double value) {
  lock();
  floatValues[name] = value;
  dirty = true;
  unlock();
}

void JetCOWDictionary::setAttrProtected(char *name, std::string &value) {
  lock();
  stringValues[name] = value;
  dirty = true;
  unlock();
}

int JetCOWDictionary::getAttrIntProtected(char *name) {
  int i;
  lock();
  i = intValues[name];
  unlock();
  return i;
}

char *JetCOWDictionary::getAttrStrProtected(char *name) {
  char *s;
  lock();
  s = (char*) stringValues[name].c_str();
  unlock();
  return s;
}

float JetCOWDictionary::getAttrFloatProtected(char *name) {
  float f;
  lock();
  f = floatValues[name];
  unlock();
  return f;
}

void JetCOWDictionary::delAttrProtected(char *name) {
  lock();
  if (stringValues.count(name))
    stringValues.erase(name);
  if (intValues.count(name))
    intValues.erase(name);
  if (floatValues.count(name))
    floatValues.erase(name);
  unlock();
}

JetCOWDictionary &JetCOWDictionary::operator = (JetCOWDictionary &d) {
  lock();
  intValues    = d.intValues;
  stringValues = d.stringValues;
  floatValues  = d.floatValues;
  dirty = true;
  unlock();
  commit();
  return *this;
}

void JetCOWDictionary::revert(void) {
  lock();
  std::string buffer;
  Uint8 *data;
  Uint32 dataSize;
  get(buffer);
  data = (Uint8*) buffer.c_str();
  dataSize = buffer.size();

  intValues.clear();
  stringValues.clear();
  floatValues.clear();
  
  while (dataSize > 0) {
    /* Read the key */
    char *key = (char*) data;
    while (dataSize && *data) {
      data++;
      dataSize--;
    }
    data++;
    dataSize--;
    if (dataSize <= 0) {
      dirty = false;
      unlock();
      throw JetCOWFormatException(cow, "Incomplete key in dictionary");
    }

    /* Read the data type and padding */
    char type = *data;
    dataSize--;
    data++;
    int bytesRead = data - (Uint8*)buffer.c_str();
    if (bytesRead & 3) {
      dataSize -= 4 - (bytesRead & 3);
      data     += 4 - (bytesRead & 3);
    }
    
    switch (type) {

    case 'i': {
      if (dataSize < sizeof(s32n)) {
	dirty = false;
	unlock();
	throw JetCOWFormatException(cow, "Incomplete integer value in dictionary");
      }

      setAttr(key, jet_ntohl(*(s32n*)data));
      data += sizeof(s32n);
      dataSize -= sizeof(s32n);
    }
      break;

    case 's': {
      char *stringvalue = (char *) data;
      while (dataSize>0 && *data) {
	dataSize--;
	data++;
      }
      dataSize--;
      data++;
      if (dataSize<0) {
	dirty = false;
	unlock();
	throw JetCOWFormatException(cow, "Incomplete string value in dictionary");
      }
      setAttr(key, stringvalue);
    }
      break;

    case 'f': {
      if (dataSize < sizeof(float)) {
	dirty = false;
	unlock();
	throw JetCOWFormatException(cow, "Incomplete floating point value in dictionary");
      }
      setAttr(key, *(float*)data);
      data += sizeof(float);
      dataSize -= sizeof(float);
    }
      break;

    default:
      dirty = false;
      unlock();
      throw JetCOWFormatException(cow, "Unknown data type in dictionary");
    }    
  }

  dirty = false;
  unlock();

  loadCachedValues();
}

void JetCOWDictionary::addKeyHeader(std::string &page, std::string key, char type) {
  page += key;
  page += '\0';
  page += type;
  
  /* Pad to 32-bit boundary */
  while (page.length() & 3)
    page += '\0';
}

void JetCOWDictionary::commit(void) {
  saveCachedValues();

  if (!dirty) return;
  lock();
  std::string page;

  /* Int values */
  for (std::map<std::string,Sint32>::iterator i = intValues.begin(); i!=intValues.end(); i++) {
    u32n n = htonl(i->second);
    char *bytes = (char*) &n;
    addKeyHeader(page, i->first, 'i');
    for (int j=0;j<sizeof(u32n);j++)
      page += bytes[j];
  }

  /* Float values */
  for (std::map<std::string,float>::iterator i = floatValues.begin(); i!=floatValues.end(); i++) {
    char *bytes = (char*) &i->second;
    addKeyHeader(page, i->first, 'f');
    for (int j=0;j<sizeof(float);j++)
      page += bytes[j];
  }

  /* string values */
  for (std::map<std::string,std::string>::iterator i = stringValues.begin(); i!=stringValues.end(); i++) {
    addKeyHeader(page, i->first, 's');
    page += i->second;
    page += '\0';
  }

  put(page);
  dirty = false;
  unlock();
}

/* Subclasses can define these to maintain a cache of frequently used values in the dict */
void JetCOWDictionary::loadCachedValues(void) {}
void JetCOWDictionary::saveCachedValues(void) {}

JetCOWObject *JetCOW::checkout(const std::string type, Sint32 id) {
  JetCOWObject *obj;

  /* Object already in core? */
  if (id>=0 && objectInstances.count(id)) {
    obj = objectInstances[id];
    obj->ref();
  }
  
  /* Have to create a completely new object? 
   * (Note: This case also works for the header, even though we're not
   * necessarily creating a new object there.)
   */
  else if (id<=0) {
    obj = JetCOWRegistry::getInstance()->createObject(this, id, type);
    objectInstances[obj->id] = obj;
  }

  /* Load the object from the JetCOW - first we have
   * to find out its actual data type.
   */
  else {
    JetCOWObject *typeTest = new JetCOWObject(this, id);
    typeTest->revert();
    std::string realType = typeTest->type;
    delete typeTest;
    obj = JetCOWRegistry::getInstance()->createObject(this, id, realType);
    objectInstances[obj->id] = obj;
    obj->revert();
  }

  /* Now make sure the type we just got is a subclass of the one requested */
  if (obj->type != type && !JetCOWRegistry::getInstance()->isSubclass(obj->type, type))
    throw JetCOWFormatException(this, "Unexpected object type");

  return obj;
}

JetCOWDictionary *JetCOW::getHeader(void) {
  return header;
}

JetCOWDictionary *JetCOW::checkoutDict(Sint32 id) {
  return (JetCOWDictionary*) checkout("JetCOWDictionary", id);
}

JetCOWDictionary *JetCOW::newDict(std::string type) {
  return (JetCOWDictionary*) checkout(type);
}

SceneNode *JetCOW::checkoutSceneNode(Sint32 id) {
  return (SceneNode*) checkout("SceneNode", id);
}

SceneNode *JetCOW::newSceneNode(std::string type) {
  return (SceneNode*) checkout(type);
}

SplineSceneNode *JetCOW::newSpline() {
  return (SplineSceneNode*) checkout("Spline");
}

Scene *JetCOW::checkoutScene(Sint32 id) {
  return (Scene*) checkout("Scene", id);
}

Scene *JetCOW::newScene(void) {
  return (Scene*) checkout("Scene");
}

JetCOWVector::JetCOWVector(JetCOW *cow, Sint32 id, const char *type) : JetCOWObject(cow,id,type) {
  requiredType = "JetCOWObject";
  if (id>0)
    revert();
}

JetCOWVector::~JetCOWVector() {
  commit();
  clearAndUnref();
}
  
/* Store a flat array of 32-bit JetCOWObject IDs in network byte order */
void JetCOWVector::revert(void) {
  lock();
  std::string buffer;
  Uint8 *data;
  Uint32 dataSize;
  get(buffer);
  data = (Uint8*) buffer.c_str();
  dataSize = buffer.size();
  
  clearAndUnref();
  while (dataSize >= sizeof(u32n)) {
    vec.push_back(cow->checkout(requiredType, jet_ntohl(*(s32n*)data)));
    data += sizeof(u32n);
    dataSize -= sizeof(u32n);
  }
  
  dirty = false;
  unlock();
}

void JetCOWVector::commit(void) {
  if (!dirty) return;
  lock();
  std::string page;
  std::vector<JetCOWObject*>::iterator i;
  
  for (i=vec.begin(); i!=vec.end(); i++) {
    u32n n = jet_htonl((*i)->id);
    char *bytes = (char*) &n;
    for (int j=0;j<sizeof(u32n);j++)
      page += bytes[j];
  }
  
  put(page);
  dirty = false;
  unlock();
}

void JetCOWVector::clearAndUnref(void) {
  std::vector<JetCOWObject*>::iterator i;
  for (i=vec.begin(); i!=vec.end(); i++)
    (*i)->unref();
  vec.clear();
}

/* The End */
