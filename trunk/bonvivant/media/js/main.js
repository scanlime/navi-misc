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
