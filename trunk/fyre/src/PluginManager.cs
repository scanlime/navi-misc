/*
 * PluginManager.cs - abstract data type defining an Element
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
using System.IO;
using System.Reflection;

namespace Fyre {

	class PluginManager
	{
		string directory;
		public ArrayList plugin_types;

		public
		PluginManager (string directory)
		{
			this.directory = directory;
		}

		public void
		Load ()
		{
			plugin_types = FindPluginTypes ();

			ElementFactory factory = ElementFactory.Instance;
			foreach (System.Type t in plugin_types)
				factory.AddType (t);
		}

		ArrayList
		FindPluginTypes ()
		{
			ArrayList all_plugin_types = new ArrayList ();

			ArrayList files = new ArrayList();
			string current_dir = Directory.GetCurrentDirectory();

			if (current_dir.IndexOf (Defines.DATADIR) == -1) {
				// Before make install is run, the plugins are in the various subdirectories
				// in src/Plugins. Go through each directory and look for dlls. This allows
				// Fyre to be run straight from the toplevel directory or the src directory,
				// and it will still find the plugins

				string plugins     = System.String.Concat (current_dir, "/Plugins");
				string src_plugins = System.String.Concat (current_dir, "/src/Plugins");

				if (Directory.Exists (plugins)) {
					foreach (string dir in Directory.GetDirectories (plugins)) {
						foreach (string file in Directory.GetFiles (dir, "*.dll"))
							files.Add (file);
					}
				} else if (Directory.Exists (src_plugins)) {
					foreach (string dir in Directory.GetDirectories (src_plugins)) {
						foreach (string file in Directory.GetFiles (dir, "*.dll"))
							files.Add (file);
					}
				}
			}

			// Add all the files in the PLUGINSDIR to the list of plugins.
			if (Directory.Exists (directory)) {
				foreach (string file in Directory.GetFiles (directory, "*.dll")) {
					files.Add (file);
				}
			}

			// Pull in types from assemblies
			foreach (string file in files) {
				try {
					ArrayList asm_types = FindPluginTypesInFile (file);
					// Since we try multiple plugin directories, it's concievable we'll get
					// duplicates. Don't add a plugin type unless we can't already find it
					// in our store.
					foreach (System.Type type in asm_types)
						if (!all_plugin_types.Contains (type))
							all_plugin_types.Add (type);
				} catch (System.Exception e) {
					// FIXME - aggregate all exceptions that get caught here into a single
					// message, rather than using separate dialogs for each warning. We
					// should probably also try to humanize this a little, rather than just
					// using the exception text.
					WarningDialog err = new WarningDialog (null, "Load Error", System.String.Format ("Error loading plugin: {0}", e.ToString()));
					err.Run();
					err.Destroy();
				}
			}

			return all_plugin_types;
		}

		static ArrayList
		FindPluginTypesInFile (string filepath)
		{
			Assembly asm = Assembly.LoadFrom (filepath);
			return FindPluginTypesInAssembly (asm);
		}

		static ArrayList
		FindPluginTypesInAssembly (Assembly asm)
		{
			System.Type [] types = asm.GetTypes ();
			ArrayList plugin_types = new ArrayList ();

			// Grab Element types. Eventually, we might want to convert this
			// to just load everything and keep a hash of base class->type for
			// different plugin hooks.
			foreach (System.Type type in types)
				if (type.BaseType == typeof (Element))
					plugin_types.Add (type);

			return plugin_types;
		}
	}

}
