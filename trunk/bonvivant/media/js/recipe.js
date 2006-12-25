/*
 * recipe.js --
 *    dHTML (yeah, baby) for viewing and editing recipes.
 */

function addIngredient()
{
	var container = document.getElementById("ingredients");

	var n = container.getElementsByTagName("input").length

	var el = document.createElement("input");
	el.setAttribute("type",  "text");
	el.setAttribute("name",  "ingredient" + (n + 1));
	el.setAttribute("class", "ingredient");

	container.appendChild(el)
}

function getElementsByClass(value, scope, tag_name)
{
	if (!scope)
		scope = document;
	if (!tag_name)
		tag_name='*';

	values = value.split(' ');
	class_res = [];
	for (var i = 0; i < values.length; i++) {
		class_res[class_res.length] = new RegExp('(^|\\s)'+values[i]+'($|\\s)');
	}

	var els = scope.getElementsByTagName(tag_name);
	result = [];
	for (var i = 0; i < els.length; i++) {
		var found = true;
		for (var j = 0; j < class_res.length; j++) {
			if (!els[i].className.match(class_res[j])) {
				found = false;
				break;
			}
		}
		if (found)
			result[result.length] = els[i];
	}

	return result
}

function absLeft(el)
{
	if (el) {
		return el.offsetLeft + absLeft(el.offsetParent);
	} else {
		return 0;
	}
}

function absTop(el)
{
	if (el) {
		return el.offsetTop + absTop(el.offsetParent);
	} else {
		return 0;
	}
}

function loadComments()
{
	var comments = document.getElementById("comments");


	// Create the hover indicators for all commentable blocks
	var blocks = getElementsByClass("cb");
	var x = absLeft(document.getElementById("container"));
	for (var i = 0; i < blocks.length; i++) {
		var y = absTop(blocks[i]);

		el = document.createElement("div");
		el.className = "ch";

		el.style.left = (x + 1) + "px";
		el.style.top = y + "px";
		el.style.width = "10px";
		el.style.height = blocks[i].offsetHeight + "px";

		comments.appendChild(el);
	}

	// Position comment indicators
	blocks = getElementsByClass("cg");
	for (var i = 0; i < blocks.length; i++) {
		var indicator = getElementsByClass("ci", blocks[i])[0];

		var id = "cb" + blocks[i].id.slice(2);
		var cb = document.getElementById(id);
		var y = absTop(cb);

		indicator.style.left = (x - 13) + "px";
		indicator.style.top = (y - 10) + "px";
	}
}
