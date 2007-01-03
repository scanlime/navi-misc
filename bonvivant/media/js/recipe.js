/*
 * recipe.js --
 *    dHTML (yeah, baby) for viewing recipes.
 */

dojo.require("dojo.dnd.HtmlDragMove");

var comments = function()
{
	var commentsWindow;

	var _createWindow = function() {
		commentsWindow = document.createElement('div');
		commentsWindow.id = 'comments';
		document.body.appendChild(commentsWindow);

		var dragHandle = document.createElement('div');
		dragHandle.id = 'commentDrag';
		commentsWindow.appendChild(dragHandle);

		var drag = new dojo.dnd.HtmlDragMoveSource(commentsWindow);
		drag.setDragHandle(dragHandle);
	}

	var _showWindow = function(ev) {
		commentsWindow.style.display = 'block';
		commentsWindow.style.left = ev.pageX + 'px';
		commentsWindow.style.top  = ev.pageY + 'px';
	}

	return {
		load : function() {
			var cis = document.getElementById('cis');
			var x = absLeft(document.getElementById('container'));

			// Position comment indicators
			for (var i = 0; i < cis.childNodes.length; i++) {
				var ci = cis.childNodes[i];

				var id = "cb" + ci.id.slice(2);
				var cb = document.getElementById(id);
				var y = absTop(cb);

				ci.style.left = (x - 13) + "px";
				ci.style.top = (y - 10) + "px";

				dojo.event.connect(ci, "onclick", _showWindow);
			}

			// Create the hover indicators for all commentable blocks
			var blocks = getElementsByClass('cb');
			for (var i = 0; i < blocks.length; i++) {
				var y = absTop(blocks[i]);

				var el = document.createElement('div');
				el.className = 'ch';

				el.style.left = (x + 1) + 'px';
				el.style.top  = y + 'px';
				el.style.width = '10px';
				el.style.height = blocks[i].offsetHeight + 'px';

				cis.appendChild(el);
			}

			_createWindow();
		},
	};
}();
