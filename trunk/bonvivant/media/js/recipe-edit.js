/*
 * recipe-edit.js --
 *    dHTML (yeah, baby) for editing recipes.
 */

//dojo.require('dojo.lang');

var ingredients = function() {
	var container;
	var addBlock;

	var _add = function() {
		var entry = document.createElement('p');
		entry.className = 'ingredientLine';
		container.insertBefore(entry, addBlock);

		var el = document.createElement('img');
		el.width  = 16;
		el.height = 16;
		el.src = '/site_media/images/list-remove.png';
		el.style.cursor = 'pointer';

		if (el.addEventListener) {
			el.addEventListener('click', _remove, false);
		} else {
			el.attachEvent('onclick', _remove);
		}

		entry.appendChild(el);

		el = document.createElement('input');
		el.className = 'ingredientEntry';
		el.type = 'text';
		el.size = 40;

		entry.appendChild(el);
		_fixIDs();
	};

	var _remove = function() {
		item = this.parentNode;

		// FIXME: check to see if entry is empty
		container.removeChild(item);
		_fixIDs();
	}

	var _fixIDs = function() {
		var entries = getElementsByClass('ingredientEntry');
		for (var i = 0; i < entries.length; i++) {
			var id = 'ingredient_' + i;
			entries[i].name = id;
			entries[i].id = id;
		}
	}

	return {
		add : function() {
			_add();
		},

		remove : function(item) {
			_remove(item);
		},

		init : function() {
			container = document.getElementById('ingredients');

			var entries = getElementsByClass('ingredientLine');
			for (var i = 0; i < entries.length; i++) {
				var entry = entries[i];

				var el = document.createElement('img');
				el.width  = 16;
				el.height = 16;
				el.src = '/site_media/images/list-remove.png';
				el.style.cursor = 'pointer';

				if (el.addEventListener) {
					el.addEventListener('click', _remove, false);
				} else {
					el.attachEvent('onclick', _remove);
				}

				entry.insertBefore(el, entry.childNodes[0]);
			}

			addBlock = document.createElement('p');
			addBlock.className = 'addingredient';

			container.appendChild(addBlock);

			var el = document.createElement('img');
			el.width  = 16;
			el.height = 16;
			el.src = '/site_media/images/list-add.png';
			el.style.cursor = 'pointer';

			if (el.addEventListener) {
				el.addEventListener('click', _add, false);
			} else {
				el.attachEvent('onclick', _add);
			}

			addBlock.appendChild(el);
			_fixIDs();
		},
	}
}();
