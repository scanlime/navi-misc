/*
 * PluginManager.cs - abstract data type defining an Element
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
 *
 * Some code borrowed from Tomboy, Copyright Alex Gravely
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

using System;
using System.Collections;
using System.IO;
using System.Reflection;

namespace Fyre {

	class PluginManager
	{
		string directory;
		public ArrayList plugin_types;

		public PluginManager (string directory)
		{
			this.directory = directory;

			plugin_types = FindPluginTypes ();
		}

		ArrayList FindPluginTypes ()
		{
			ArrayList all_plugin_types = new ArrayList ();

			string [] files = Directory.GetFiles (directory, "*.dll");

			// Pull in types from assemblies
			foreach (string file in files) {
				try {
					ArrayList asm_types = FindPluginTypesInFile (file);
					foreach (Type type in asm_types)
						all_plugin_types.Add (type);
				} catch (Exception e) {
					Console.WriteLine ("Error loading plugin: {0}", e);
				}
			}

			return all_plugin_types;
		}

		static ArrayList FindPluginTypesInFile (string filepath)
		{
			Assembly asm = Assembly.LoadFrom (filepath);
			return FindPluginTypesInAssembly (asm);
		}

		static ArrayList FindPluginTypesInAssembly (Assembly asm)
		{
			Type [] types = asm.GetTypes ();
			ArrayList plugin_types = new ArrayList ();

			// Grab Element types. Eventually, we might want to convert this
			// to just load everything and keep a hash for different plugin hooks
			foreach (Type type in types)
				if (type.BaseType == typeof (Element))
					plugin_types.Add (type);

			return plugin_types;
		}
	}

}
