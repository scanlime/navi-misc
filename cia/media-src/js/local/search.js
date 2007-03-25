/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var CIASearch = {}

CIASearch.editTimeout = 100.0;

CIASearch.iconSize = 16;

CIASearch.sectionInfo = {
    'projects': { heading: 'Projects:',     priority: 3 },
    'authors':  { heading: 'Authors:',      priority: 2 },
    'stats':    { heading: 'Other stats:',  priority: 1 }
};


CIASearch.toggleDefaultText = function(enable)
{
    this.field.style.display = "inline";

    (enable ? YAHOO.util.Dom.addClass : YAHOO.util.Dom.removeClass)
        (this.field, "default-text");

    this.hasDefaultText = enable;
    this.field.value = enable ? this.defaultText : "";
}

CIASearch.toggleResults = function(enable)
{
    var res = this.results;
    if (!res) {
	if (!enable) {
	    return;
	}

	res = new YAHOO.widget.Overlay(this.fieldId + "-results", {
	    context: [this.fieldId, 'tl', 'bl'],
	    width: "25em",
	    visible: true,
	    zIndex: 10,

	    effect: { effect: YAHOO.widget.ContainerEffect.FADE,
		      duration:0.25 },

	    /*
	     * Currently disabled: this can come in handy if the search
	     * box is flowing off the right edge of the page, but it
	     * can cause problems if the result set is long and it ends
	     * up flowing off the *bottom* of the page.
	     */
	    // constraintoviewport: true,

	    /*
	     * The resize monitor doesn't seem to work right, and
	     * it sometimes causes the page to momentarily grow a scroll bar.
	     */
	    monitorresize: false
	});
	this.results = res;
	
	/*
	 * YUI seems to use relative positioning by default. Override this,
	 * since the relatively positioned results box will still cause blank
	 * space to appear at the bottom of the document.
	 */
	YAHOO.util.Dom.setStyle(res.element, "position", "absolute");

	res.setBody("");
	res.render(document.body);
    }

    if (enable) {
	/* Recalculate the alignment, in case our context element moved */
	res.align('tl', 'bl');

	res.show();
    } else {
	res.hide();
    }
}

CIASearch.cancel = function()
{
    /*
     * Cancel an existing update timer or connection
     */
    if (this.timer) {
	clearTimeout(this.timer);
	this.timer = null;
    }
    if (this.request) {
	YAHOO.util.Connect.abort(this.request, null, false);
	this.request = null;
    }
}

CIASearch.updateQuery = function()
{
    var query = this.field.value;

    if (this.hasFocus && query) {
	this.toggleResults(true);

	if (query != this.resultsQuery) {
	    this.cancel();

	    this.resultsQuery = null;
	    this.results.setBody("Searching for <strong>" +
				 htmlEscape(query) +
				 "</strong>...");

	    var self = this;
	    this.timer = setTimeout(function() {
		/*
		 * The user stopped editing for a while. Send a search query.
		 */
		self.timer = null;
		self.sendQuery();
	    }, this.editTimeout);
	}

    } else {
	this.cancel();
	this.toggleResults(false);
    }
}

CIASearch.delayedUpdateQuery = function()
{
    /* The new value won't be valid until the event finishes propagating */
    var self = this;
    setTimeout(function() { self.updateQuery() }, 0);
}

CIASearch.sendQuery = function()
{
    var self = this;
    var query = this.field.value;

    var responseSuccess = function(req) {
	self.request = null;
	self.resultsQuery = query;
	try {
	    var obj = parseJSON(req.responseText);
	    self.displayResults(obj.results, query);
	}
	catch (e) {
	    self.results.setBody("Internal error (" + e + ")");
	}
    }

    var responseFailure = function(req) {
	self.request = null;
	self.resultsQuery = null;
	self.results.setBody("Connection error during search (" + req.status + ")");
    }

    var callback = {
	success: responseSuccess,
	failure: responseFailure,
	timeout: 5000
    };

    /*
     * Must use encodeURIComponent() instead of escape().  escape()
     * will try to encode text in latin-1 (or some other unspecified
     * character set?) and it will emit %u1234-style escapes for other
     * Unicode characters.  encodeURIComponent() generates UTF-8
     * url-encoded text, which is exactly what we want.
     */
    var q = encodeURIComponent(query);

    self.request = YAHOO.util.Connect.asyncRequest('GET', this.url + q, callback)
}

CIASearch.displayResults = function(results, query)
{
    if (!results.length) {
	this.results.setBody("No results");
	return;
    }

    var lcQuery = query.toLowerCase();
    var list = document.createElement('div');
    var numExact = 0;

    /* Divide the results up by section, and look for exact matches */
    var sectionMap = {};
    var sectionList = []
    for (var i in results) {
	var result = results[i];

	var section = sectionMap[result.section];
	if (!section) {
	    section = sectionMap[result.section] = {
		results: [],
		name: result.section,
	        hasExactMatch: false,
		hasPrefixMatch: false,
		info: this.sectionInfo[result.section]
	    };
	    sectionList.push(section);
	}

	var lcTitle = result.title.toLowerCase();

	if (lcTitle == lcQuery) {
	    result.isExactMatch = true;
	    result.isPrefixMatch = true;
	    section.hasExactMatch = true;
	    numExact += 1;
	} else {
	    result.isExactMatch = false;
	    if (lcTitle.indexOf(lcQuery) == 0) {
		result.isPrefixMatch = true;
		section.hasPrefixMatch = true;
	    } else {
		result.isPrefixMatch = false;
	    }
	}

	section.results.push(result);
    }

    sectionList.sort(function(a, b) {
	if (a.hasExactMatch != b.hasExactMatch) {
	    return b.hasExactMatch - a.hasExactMatch;
	}
	if (a.hasPrefixMatch != b.hasPrefixMatch) {
	    return b.hasPrefixMatch - a.hasPrefixMatch;
	}
	return b.info.priority - a.info.priority;
    });

    /* For each section with results... */
    for (var i in sectionList) {
	var section = sectionList[i];

	/* Section heading */
	var heading = document.createElement('div');
	YAHOO.util.Dom.addClass(heading, 'search-heading');
	heading.innerHTML = section.info.heading;
	list.appendChild(heading);

	section.results.sort(function(a, b) {
	    if (a.isExactMatch != b.isExactMatch) {
		return b.isExactMatch - a.isExactMatch;
	    }
	    if (a.isPrefixMatch != b.isPrefixMatch) {
		return b.isPrefixMatch - a.isPrefixMatch;
	    }
	    if (a.title < b.title) {
		return -1;
	    }
	    if (a.title > b.title) {
		return 1;
	    }
	    return 0;
	});

	for (var j in section.results) {
	    var result = section.results[j];

	    /* Each result row is an <a> element */
	    var item = document.createElement('a');
	    item.href = result.url;
	    list.appendChild(item);

	    /* Reserve some space equal to our maximum icon size */
	    var iconBox = document.createElement('div');
	    YAHOO.util.Dom.addClass(iconBox, 'search-icon');
	    item.appendChild(iconBox);

	    if (result.icon) {
		/* Center the icon in iconBox */
		var icon = document.createElement('img');
		icon.src = result.icon.url;
		icon.style.width = result.icon.width + 'px';
		icon.style.height = result.icon.height + 'px';
		icon.style.left = (this.iconSize - result.icon.width) / 2 + 'px';
		icon.style.top = (this.iconSize - result.icon.height) / 2 + 'px';
		iconBox.appendChild(icon);
	    }

	    var title = document.createElement('span');
	    if (result.isExactMatch && numExact == 1) {
		title.innerHTML = "<strong>" + htmlEscape(result.title) + "</strong>";
	    } else {
		title.innerHTML = htmlEscape(result.title);
	    }
	    item.appendChild(title);
	}
    }

    this.results.setBody(list);
}

CIASearch.init = function(url, fieldId, defaultText)
{
    this.url = url + '?ico=' + this.iconSize + '&q=';

    this.fieldId = fieldId;
    this.field = document.getElementById(fieldId);
    this.field.setAttribute('autocomplete', 'off');

    this.defaultText = defaultText;
    this.toggleDefaultText(true);

    var Event = YAHOO.util.Event;
    Event.on(this.field, "focus", this.onFocus, this, true);
    Event.on(this.field, "blur", this.onBlur, this, true);
    Event.on(this.field, "change", this.onChange, this, true);
    Event.on(this.field, "keypress", this.onKeyPress, this, true);
}

CIASearch.onFocus = function(ev)
{
    if (this.hasDefaultText) {
	this.toggleDefaultText(false);
    }

    this.hasFocus = true;
    this.updateQuery();
}

CIASearch.onBlur = function(ev)
{
    this.hasFocus = false;
    this.updateQuery();

    if (this.field.value == "") {
	this.toggleDefaultText(true);
    }
}

CIASearch.onChange = function(ev)
{
    /*
     * This doesn't actually help us in the real world,
     * since browsers only send us onChange when the field
     * is about to blur...
     *
     * Our usual change notifications come via onKeyPress.
     */
    this.delayedUpdateQuery();
}

CIASearch.onKeyPress = function(ev)
{
    var Event = YAHOO.util.Event;
    var kc = Event.getCharCode(ev);
    switch (kc) {

    case 0x26: // up
    case 0x28: // down
	Event.preventDefault(ev);
	break;

    case 0x0d: // enter
	if (this.field.value != this.resultsQuery && !this.request) {
	    this.cancel();
	    this.sendQuery();
	}
	Event.preventDefault(ev);
	break;

    case 0x09: // tab
	break;

    default:
	this.delayedUpdateQuery();

    }
}
