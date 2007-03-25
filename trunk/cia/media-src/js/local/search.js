/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var CIASearch = {}

CIASearch.editTimeout = 500.0;

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
    if (enable == this.resultsVisible) {
	return;
    }
    this.resultsVisible = enable;

    this.results 

    var res = this.results;
    if (!res) {
	res = new YAHOO.widget.Overlay(this.fieldId + "-results", {
	    context: [this.fieldId, 'tl', 'bl'],
	    width: "25em",
	});
	this.results = res;
	res.setBody("");
	res.render(document.body);
    }

    if (enable) {
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

	if (query != this.query) {
	    this.query = query;
	    this.cancel();

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

CIASearch.sendQuery = function()
{
    var self = this;

    var responseSuccess = function(req) {
	self.request = null;
	self.results.setBody(req.responseText);
    }

    var responseFailure = function(req) {
	self.request = null;
	self.results.setBody("Connection error during search (" + req.status + ")");
    }

    var callback = {
	success: responseSuccess,
	failure: responseFailure,
	timeout: 5000
    };

    self.request = YAHOO.util.Connect.asyncRequest('POST', this.url, callback,
						   'query=' + escape(this.field.value));
}

CIASearch.init = function(url, fieldId, defaultText)
{
    var self = this;
    self.url = url;
    this.fieldId = fieldId;
    self.field = document.getElementById(fieldId);

    self.resultsVisible = false;
    self.defaultText = defaultText;
    self.toggleDefaultText(true);

    self.field.onfocus = function()
    {
	if (self.hasDefaultText) {
	    self.toggleDefaultText(false);
	}

	self.hasFocus = true;
	self.updateQuery();
    };

    self.field.onblur = function()
    {
	if (self.field.value == "") {
	    self.toggleDefaultText(true);
	}

	self.hasFocus = false;
	self.updateQuery();
    };

    attachOnTextareaChanged(self.field, function() {
	/* The new value won't be valid until the event finishes propagating */
	setTimeout(function() { self.updateQuery() }, 0);
    });
}
