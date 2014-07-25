/* bzflag
 * Copyright (c) 1993 - 2004 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef _VISUAL_ELEMENT_MANAGER_H
#define _VISUAL_ELEMENT_MANAGER_H

#ifdef _MSC_VER
#pragma warning( 4: 4786 )
#endif

#include "common.h"
#include "Singleton.h"
#include <map>
#include <string>
#include "GeoUtils.h"

class BaseVisualElementFactory;

class BaseVisableObject
{
public:
	virtual ~BaseVisableObject(){};
	virtual bool getPos ( C3DVertex & pos ){return false;}
	virtual bool getSize( C3DVertex & size ){return false;}
	virtual bool getRot ( C3DVertex & rot ){return false;}
	virtual bool getInfoI ( std::string tag, int &value){return false;}
	virtual bool getInfoF ( std::string tag, float &value){return false;}
	virtual bool getInfoS ( std::string tag, std::string &value){return false;}
};

class BaseVisualElement
{
public:
	BaseVisualElement();
	virtual ~BaseVisualElement();

	virtual bool init ( void );
	virtual bool visible ( BaseFrustum *frustum  );
	virtual void draw ( void );

	virtual void kill ( void );

protected:	// dont' mess with these
	friend class VisualElementManager;
	BaseVisualElementFactory	*factory;
	BaseVisableObject	*parent;
};

class BaseVisualElementFactory
{
public:
	BaseVisualElementFactory();
	virtual ~BaseVisualElementFactory();

	virtual BaseVisualElement* newElement ( void );
	virtual void deleteElement ( BaseVisualElement* element);
};

class VisualElementManager : public Singleton<VisualElementManager>
{
public:
	void init ( void );

	void registerElementFactory ( BaseVisualElementFactory *factory, std::string &name );

	BaseVisualElement* newObject ( std::string &className, BaseVisableObject *parent );
	void deleteObject ( BaseVisualElement* element );

	void calcVisObjects ( void );
	void drawVisObjects ( void );

	void clearObjects ( void );
	void clearFactorys ( void );
protected:
  friend class Singleton<VisualElementManager>;

private:
  VisualElementManager();
  VisualElementManager(const VisualElementManager &dm);
  VisualElementManager& operator=(const VisualElementManager &dm);
  ~VisualElementManager();

	std::map<std::string,BaseVisualElementFactory*> factorys;
	std::vector<BaseVisualElement*> objects;
	std::vector<BaseVisualElement*> visibles;
};

#endif //_VISUAL_ELEMENT_MANAGER_H

// Local Variables: ***
// mode: C++ ***
// tab-width: 8 ***
// c-basic-offset: 2 ***
// indent-tabs-mode: t ***
// End: ***
// ex: shiftwidth=2 tabstop=8
