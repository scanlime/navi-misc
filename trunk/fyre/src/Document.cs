/*
 * EditorDocument.cs - represents a document within the pipeline editor.
 *
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

namespace Fyre.Editor
{

	class Document
	{
		Fyre.Pipeline			pipeline;
		Layout				layout;
		string				filename;
		bool				saved;

		// Count to know how many documents we've created. We use this for
		// the default filename (Untitled%d), and DnD data descriptors (so
		// we can't drag from one element list to another window).
		static int			count;
		public int			Number;

		public Fyre.Pipeline
		Pipeline
		{
			get {
				return pipeline;
			}
			set {
				pipeline = value;
			}
		}

		public Layout
		Layout
		{
			get {
				return layout;
			}
			set {
				layout = value;
			}
		}

		public string
		Filename
		{
			get {
				if (filename == null)
					return System.String.Format ("Untitiled{0}", Number);
				return filename;
			}
			set {
				filename = value;
				// FIXME - trigger event
			}
		}

		public bool
		Saved
		{
			get {
				return saved;
			}
			set {
				saved = value;
				// FIXME - trigger event
			}
		}

		public bool
		HasRealFilename
		{
			get {
				return (filename != null);
			}
		}

		public
		Document ()
		{
			Pipeline = new Fyre.Pipeline ();
			Layout = new Layout ();

			// We start out with saved = true, since it doesn't make sense to force (or
			// even allow) our user to save an empty document.
			saved = true;
			filename = null;

			count++;
			Number = count;
		}
	}
}
