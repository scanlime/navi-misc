/*
 * Pipeline.cs - manages the pipeline
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

using System.Collections;

namespace Fyre
{

	class UidComparer : IComparer
	{
		public int Compare (object x, object y)
		{
			byte[] xb = (byte[]) x;
			byte[] yb = (byte[]) y;

			for (int i = 0; i < 16; i++) {
				if (xb[i] < yb[i]) return -1;
				if (xb[i] > yb[i]) return 1;
			}
			return 0;
		}
	}

	class Pipeline
	{
		private Hashtable	element_store;
		private UidComparer	comparer;

		public Pipeline ()
		{
			comparer = new UidComparer ();
			element_store = new Hashtable (null, comparer);
		}
	}

}
