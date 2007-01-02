/*
 * recipe.js --
 *    dHTML (yeah, baby) for viewing recipes.
 */

var comments = function()
{
	var comments;

	return {
		load : function() {
			comments = document.getElementById('comments');
			var x = absLeft(document.getElementById('container'));

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

				comments.appendChild(el);
			}

			// Position comment indicators
			blocks = getElementsByClass('cg');
			for (var i = 0; i < blocks.length; i++) {
				var indicator = getElementsByClass("ci", blocks[i])[0];

				var id = "cb" + blocks[i].id.slice(2);
				var cb = document.getElementById(id);
				var y = absTop(cb);

				indicator.style.left = (x - 13) + "px";
				indicator.style.top = (y - 10) + "px";
			}
		},
	};
}();
