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

class PluginManager
{
	string directory;
	FileSystemWatcher dir_watcher;
	ArrayList plugin_types;

	public PluginManager (string directory)
	{
		this.directory = directory;

		dir_watcher = new FileSystemWatcher (directory, "*.dll");
		dir_watcher.Created += OnPluginCreated;
		dir_watcher.Deleted += OnPluginDeleted;
		dir_watcher.EnableRaisingEvents = true;
	}

	void OnPluginCreated (object sender, FileSystemEventArgs args)
	{
		ArrayList asm_plugins = FindPluginTypesInFile (args.FullPath);

		foreach (Type type in asm_plugins) {
			plugin_types.Add (type);
		}
	}

	void OnPluginDeleted (object sender, FileSystemEventArgs args)
	{
		/* Not sure we really want to deal with this */
	}

	static ArrayList FindPluginTypesInFile (string filepath)
	{
		Assembly asm = Assembly.LoadFrom (filepath);
		return FindPluginTypesInAssembly (asm);
	}

	static ArrayList FindPluginTypesInAssembly (Assembly asm)
	{
		Type [] types = asm.GetTypes ();
		ArrayList asm_plugins = new ArrayList ();
		bool found_one = false;

		foreach (Type type in types)
			if (type.BaseType == typeof (Element))
				asm_plugins.Add (type);

		return asm_plugins;
	}
}
