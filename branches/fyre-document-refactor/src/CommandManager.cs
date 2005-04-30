/*
 * CommandManager.cs - Command handling, including undo/redo capabilities
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

namespace Fyre
{
	class Command
	{
		// We store this for use in the menu - we want to show "Undo <blah>", etc.
		public string 	Name;

		public virtual void
		Do (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
		{
		}

		public virtual void
		Undo (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
		{
		}
	};

	class CommandManager
	{
		// Undo and redo stacks
		public ArrayList	undo_stack;
		public ArrayList	redo_stack;

		// Handles to the pipeline editor that contains stuff commands might need
		public Layout		layout;
		public PipelineDrawing	drawing;
		public Pipeline		pipeline;

		public
		CommandManager (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
		{
			// Create the new undo and redo stack
			undo_stack = new ArrayList ();
			redo_stack = new ArrayList ();

			// Add in all the important stuff
			this.layout = layout;
			this.drawing = drawing;
			this.pipeline = pipeline;
		}

		public void
		Do (Command command)
		{
			command.Do (layout, drawing, pipeline);
			undo_stack.Add (command);
			redo_stack.Clear ();

			pipeline.saved = false;

			pipeline.OnChanged (new System.EventArgs());
		}

		public void
		Undo ()
		{
			Command command = (Command) undo_stack[undo_stack.Count - 1];
			undo_stack.RemoveAt (undo_stack.Count - 1);

			command.Undo (layout, drawing, pipeline);

			redo_stack.Add (command);

			if (undo_stack.Count == 0)
				pipeline.saved = true;

			pipeline.OnChanged (new System.EventArgs ());
		}

		public void
		Redo ()
		{
			Command command = (Command) redo_stack[redo_stack.Count - 1];
			redo_stack.RemoveAt (redo_stack.Count - 1);

			command.Do (layout, drawing, pipeline);

			undo_stack.Add (command);

			pipeline.saved = false;

			pipeline.OnChanged (new System.EventArgs ());
		}
	}

	namespace Commands
	{
		class Add : Command
		{
			System.Guid		id;
			Element			e;
			int			x;
			int			y;

			public
			Add (Element e, int x, int y)
			{
				Name = "Add " + e.Name ();
				id = e.id;
				this.e = e;
				this.x = x;
				this.y = y;
			}

			public override void
			Do (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				pipeline.element_store.Add (id.ToString ("d"), e);
				drawing.AddElement (e, x, y);
			}

			public override void
			Undo (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				drawing.RemoveElement (e);
				pipeline.element_store.Remove (id.ToString ("d"));
			}
		}

		class Cut : Command
		{
			public Cut (Element e)
			{
				Name = "Cut " + e.Name ();
			}

			public override void
			Do (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				// FIXME - implement
			}

			public override void
			Undo (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				// FIXME - implement
			}
		}

		class Paste : Command
		{
			public Paste (Element e)
			{
				Name = "Paste " + e.Name ();
			}

			public override void
			Do (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				// FIXME - implement
			}

			public override void
			Undo (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				// FIXME - implement
			}
		}

		class Delete : Command
		{
			System.Guid		id;
			Element			e;
			int			x;
			int			y;

			public Delete (Element e, int x, int y)
			{
				Name = "Delete " + e.Name ();
				this.e = e;
				this.x = x;
				this.y = y;
				id = e.id;
			}

			public override void
			Do (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				drawing.RemoveElement (e);
				pipeline.element_store.Remove (id.ToString ("d"));
			}

			public override void
			Undo (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				pipeline.element_store.Add (id.ToString ("d"), e);
				drawing.AddElement (e, x, y);
			}
		}

		class Move : Command
		{
			System.Guid		id;
			Element			e;
			int			old_x, old_y;
			int			new_x, new_y;

			public Move (Element e, int old_x, int old_y, int new_x, int new_y)
			{
				Name = "Move " + e.Name ();
				this.e = e;
				this.old_x = old_x;
				this.old_y = old_y;
				this.new_x = new_x;
				this.new_y = new_y;
				id = e.id;
			}

			public override void
			Do (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				layout.SetElementPosition (e, new_x, new_y);
				drawing.Redraw();
			}

			public override void
			Undo (Layout layout, PipelineDrawing drawing, Pipeline pipeline)
			{
				layout.SetElementPosition (e, old_x, old_y);
				drawing.Redraw();
			}
		}
	}
}

