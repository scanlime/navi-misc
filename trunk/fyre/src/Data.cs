/*
 * Data.cs - basic types in the pipeline
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2005 Fyre Team (see AUTHORS)
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
using System.Xml;

namespace Fyre
{

	public class Type
	{
	}

	public class Int : Type
	{
		public int	Value;
	}

	public class Float : Type
	{
		public double	Value;
	}

	public class Bool : Type
	{
		public bool	Value;
	}

	public class Matrix : Type
	{
		public Type[]	Value;
		public int	Rank;
		public int[]	Size;

		public
		Matrix (Type t, int rank, int[] size)
		{
			Rank = rank;
			Size = size;
		}
	}
}
