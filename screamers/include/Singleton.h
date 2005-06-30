/*
 * Singleton.h - singleton class, taken mostly from BZFlag.  We use this one
 * 	instead of Ogre::Singleton if the class will is used in any non-client code.
 *
 * Copyright (C) 2005 David Trowbridge
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

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

/* Singleton template class
 *
 * This template class pattern provides a traditional Singleton pattern.
 * Allows you to designate a single-instance global class by inheriting
 * off of the template class.
 *
 * Example:
 *
 *   class Whatever : public Singleton<Whatever> ...
 *
 * The class will need to provide either a public or a protected friend
 * constructor:
 *
 *   friend class Singleton<Whatever>;
 *
 * The class will also need to initialize it's own instance in a single
 * compilation unit (a .cxx file):
 *
 *   // statically initialize the instance to nothing
 *   template <>
 *   Whatever* Singleton<Whatever>::_instance = 0;
 */

template<typename T>
class Singleton
{
private:
	static T		*_instance;

protected:

				 Singleton () {}
				 Singleton (T *instance) { _instance = instance; }
				 Singleton (const Singleton &) {}
	Singleton		&operator = (const Singleton &) { return *this; }
				~Singleton () {}

public:
	inline static T		&instance ()
	{
		if (_instance == NULL)
			_instance = new T;
		return *Singleton::_instance;
	}

	inline static T		*instancePtr ()
	{
		if (_instance == NULL)
			_instance = new T;
		return Singleton::_instance;
	}
};

#endif
