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

#include "VisualElementManager.h"
#include "TextUtils.h"
#include "ViewCull.h"

// ==== base visual element ===========
BaseVisualElement::BaseVisualElement()
{
	factory = NULL;
	parent = NULL;
}

BaseVisualElement::~BaseVisualElement()
{

}

bool BaseVisualElement::init ( void )
{
	return true;
}

bool BaseVisualElement::visible ( BaseFrustum *frustum )
{
	return false;
}

void BaseVisualElement::draw ( void )
{
	return;
}

void BaseVisualElement::kill ( void )
{
	return;
}

// ==== base visual element factory ===========
BaseVisualElementFactory::BaseVisualElementFactory()
{
}

BaseVisualElementFactory::~BaseVisualElementFactory()
{
}

BaseVisualElement* BaseVisualElementFactory::newElement ( void )
{
	return new (BaseVisualElement);
}

void BaseVisualElementFactory::deleteElement ( BaseVisualElement* element )
{
	delete(element);
}

// ==== visual element manager ===========

// initialize the singleton
template <>
VisualElementManager* Singleton<VisualElementManager>::_instance = (VisualElementManager*)0;

void VisualElementManager::init ( void )
{
	clearFactorys();
}

void VisualElementManager::registerElementFactory ( BaseVisualElementFactory *factory, std::string &name )
{
	// return if no name, we need a name
	// or there is no factory, we need one of those too
	if (name.size()<1 || !factory)
		return;
	factorys[string_util::toupper(name)] = factory;
}

BaseVisualElement* VisualElementManager::newObject ( std::string &className, BaseVisableObject *parent )
{
	std::map<std::string,BaseVisualElementFactory*>::iterator itr = factorys.find(string_util::toupper(className));
	if (itr == factorys.end())
		return NULL;

	BaseVisualElement*	newElement = itr->second->newElement();
	if (newElement)
	{
		newElement->factory = itr->second;
		newElement->parent = parent;
		newElement->init();
	}
	objects.push_back(newElement);
	return newElement;
}

void VisualElementManager::deleteObject ( BaseVisualElement* element )
{
	if (!element)
		return;

	// there should be a faster way to do this
	std::vector<BaseVisualElement*>::iterator itr = objects.begin();
	while (itr != objects.end())
	{
		if (*itr == element)
			itr = objects.erase(itr);
		else
			itr++;
	}
	element->factory->deleteElement(element);
}

void VisualElementManager::calcVisObjects ( void )
{
	ViewFrustumGL		frustum;

	frustum.Get();

	visibles.clear();
	std::vector<BaseVisualElement*>::iterator itr = objects.begin();
	while (itr != objects.end())
	{
			if ((*itr)->visible(&frustum))
				visibles.push_back(*itr);
			itr++;
	}
}

void VisualElementManager::drawVisObjects ( void )
{
	std::vector<BaseVisualElement*>::iterator itr = visibles.begin();
	while (itr != visibles.end())
	{
		(*itr)->draw();
		itr++;
	}
}

void VisualElementManager::clearObjects ( void )
{
	// be sure to delete every single object using it's factory
	std::vector<BaseVisualElement*>::iterator itr = objects.begin();
	while (itr != objects.end())
	{
		(*itr)->factory->deleteElement((*itr));
		itr++;
	}

	objects.clear();
}

void VisualElementManager::clearFactorys ( void )
{
	clearObjects();
	factorys.clear();
}

VisualElementManager::VisualElementManager()
{
}

VisualElementManager::VisualElementManager(const VisualElementManager &dm)
{	
		factorys = dm.factorys;
		objects = dm.objects;
}

VisualElementManager& VisualElementManager::operator=(const VisualElementManager &dm)
{	
	factorys = dm.factorys;
	objects = dm.objects;
	return *this;
}

VisualElementManager::~VisualElementManager()
{
	clearFactorys();
}

// Local Variables: ***
// mode: C++ ***
// tab-width: 8 ***
// c-basic-offset: 2 ***
// indent-tabs-mode: t ***
// End: ***
// ex: shiftwidth=2 tabstop=8

