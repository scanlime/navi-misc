/*
 * ElementFactory.cs - factory method which creates Elements based on
 *	a string or Type representation.
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2007 Fyre Team (see AUTHORS)
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

using System.Collections;
using System.Reflection;
using System;

namespace Fyre
{

	class ElementFactory
	{
		// This maps name->Type, so we can create Elements when deserializing
		// XML or dragging from the ElementList
		Hashtable			elements;
		Hashtable			elements_xml;
		static ElementFactory		instance;
		public static ElementFactory	Instance
		{
			get {
				if (instance == null)
					instance = new ElementFactory ();
				return instance;
			}
		}

		private
		ElementFactory ()
		{
			elements = new Hashtable ();
			elements_xml = new Hashtable ();
		}

		public IDictionaryEnumerator
		GetEnumerator ()
		{
			return elements.GetEnumerator ();
		}

		public void
		AddType (System.Type t)
		{
			Element e = Create (t);
			string name = e.Name ();
			if (elements.Contains (name)) {
				WarningDialog err = new WarningDialog (null, "Load Error",
						String.Format ("Error loading plugin:\nA plugin named {0} already exists.", name));
				err.Run ();
				err.Destroy ();
			} else {
				elements.Add (name, t);
				string xml_name = e.XmlName ();
				elements_xml.Add (xml_name, t);
			}

		}

		public Element
		Create (string name)
		{
			System.Type t = (System.Type) elements[name];
			return Create (t);
		}

		public Element
		Create (System.Type t)
		{
			object[] i = {};
			Element e = (Element) t.GetConstructor (System.Type.EmptyTypes).Invoke (i);

			return e;
		}

		public Element
		CreateFromXml (string name)
		{
			System.Type t = (System.Type) elements_xml[name];
			return Create (t);
		}
	}
}
