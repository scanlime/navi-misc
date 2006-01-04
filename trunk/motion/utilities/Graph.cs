/*
 * Graph.cs - graph data structures for MC interpolation
 *
 * Copyright (C) 2005-2006 David Trowbridge
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

namespace AMC.Graph
{
	class Edge
	{
		int		weight;
		public int	Weight {
			get { return weight; }
			set { weight = value; }
		}
	}

	class Node
	{
		int		visited;
		public int	Visited {
			get { return visited; }
			set { visited = value; }
		}
	}

	class Graph
	{
		ArrayList		edges;
		public ArrayList	Edges {
			get { return edges; }
		}

		ArrayList		nodes;
		public ArrayList	Nodes {
			get { return nodes; }
		}

		public Graph ()
		{
			edges = new ArrayList ();
			nodes = new ArrayList ();
		}
	}
}
