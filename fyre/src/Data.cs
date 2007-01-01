/*
 * Data.cs - basic types in the pipeline
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
using System.Xml;

namespace Fyre
{

	public class Type
	{
		// Type-checker helper functions. These make Element.Check() much cleaner
		public static bool
		IsInt (Type t)
		{
			return (t is Fyre.Int);
		}

		public static bool
		IsFloat (Type t)
		{
			return (t is Fyre.Float);
		}

		public static bool
		IsBool (Type t)
		{
			return (t is Fyre.Bool);
		}

		public static bool
		IsMatrix (Type t)
		{
			return (t is Fyre.Matrix);
		}

		public static int
		GetMatrixRank (Type t)
		{
			Fyre.Matrix m = (Fyre.Matrix) t;
			return m.Rank;
		}

		public static int[]
		GetMatrixSize (Type t)
		{
			Fyre.Matrix m = (Fyre.Matrix) t;
			return m.Size;
		}

		public static Fyre.Type
		GetMatrixType (Type t)
		{
			Fyre.Matrix m = (Fyre.Matrix) t;
			return m.ChildType;
		}
	}

	public class Int : Type
	{
		public int	Value;

		public override string
		ToString ()
		{
			return "Int";
		}
	}

	public class Float : Type
	{
		public double	Value;

		public override string
		ToString ()
		{
			return "Float";
		}
	}

	public class Bool : Type
	{
		public bool	Value;

		public override string
		ToString ()
		{
			return "Bool";
		}
	}

	public class Matrix : Type
	{
		public Type	ChildType;
		public int	Rank;
		public int[]	Size;

		public
		Matrix (Type t, int rank, int[] size)
		{
			ChildType = t;
			Rank = rank;
			Size = size;
		}

		public override string
		ToString ()
		{
			return System.String.Format ("Matrix({0}, {1}, [{2}])", ChildType.ToString (), Rank, Size);
		}
	}

	public class PadError : System.Exception
	{
		public int pad;

		public
		PadError (int pad, string text) : base (text)
		{
			this.pad = pad;
		}
	}
}
