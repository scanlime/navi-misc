/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var CIASearch = {}

CIASearch.editTimeout = 1000.0;

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
	    constraintoviewport: true,
	    effect: { effect: YAHOO.widget.ContainerEffect.FADE,
		      duration:0.25 },

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
	    self.displayResults(obj.results);
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

    self.request = YAHOO.util.Connect.asyncRequest('GET', this.url + escape(query), callback)
}

CIASearch.displayResults = function(results)
{
    if (!results.length) {
	this.results.setBody("No results");
	return;
    }

    /*
     * First, check whether there is a single exact match
     */

    var list = document.createElement('div');
    for (var i in results) {
	var result = results[i];
	var item = document.createElement('a');
	item.href = result.url;
	item.innerHTML = htmlEscape(result.title);
	list.appendChild(item);
    }

    this.results.setBody(list);
}

CIASearch.init = function(url, fieldId, defaultText)
{
    this.url = url;

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
