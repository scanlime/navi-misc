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

class BaseVisualElementFactory;
class ViewFrustum;

class BaseVisualElement
{
public:
	BaseVisualElement();
	virtual ~BaseVisualElement();

	virtual bool init ( void );
	virtual bool visible ( ViewFrustum *frustum  );
	virtual void draw ( void );

	virtual void kill ( void );

protected:	// dont' mess with these
	friend class VisualElementManager;
	BaseVisualElementFactory	*factory;
	void	*parent;
};

class BaseVisualElementFactory
{
public:
	BaseVisualElementFactory();
	virtual ~BaseVisualElementFactory();

	virtual BaseVisualElement* newElement ( void );
	virtual void deleteElement ( BaseVisualElement* );
};

class VisualElementManager : public Singleton<VisualElementManager>
{
public:
	void init ( void );

	void registerElementFactory ( BaseVisualElementFactory *factory, std::string &name );

	BaseVisualElement* newObject ( std::string &className, void *parent );
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
