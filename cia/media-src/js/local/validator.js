/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 *
 * Javascript interface for server-side data validators.
 */

var Validator = {};

/* 
 * When the text area is modified, start a timeout. If the user
 * stops editing it for a little while, we'll automatically
 * validate it.
 */
Validator.editTimeout = 2000.0;

Validator.init = function(url, validMessage, fieldId, statusId, loadingId)
{
    this.url = url;
    this.validMessage = validMessage;
    this.field = document.getElementById(fieldId);
    this.status = document.getElementById(statusId);
    this.loading = document.getElementById(loadingId);

    this._oldValue = this.field.value;

    var self = this;
    attachOnTextareaChanged(this.field, function() {
	/*
	 * Delay testForChanges until the end of the Javascript
	 * event queue, in order to pick up the key that was just pressed.
	 */
	setTimeout(function() {
	    self.testForChanges();
	}, 0);
    });
}

Validator.testForChanges = function()
{
    /*
     * Has the text editor actually changed?
     * It's potentially expensive to compare the
     * whole field contents, but we really don't
     * want false positives here.
     */
    if (this._oldValue == this.field.value) {
	return;
    }
    this._oldValue = this.field.value;

    if (this.timer) {
	/*
	 * We're already waiting, bump the timer forward.
	 */
	clearTimeout(this.timer);
	this.timer = null;
    } else {
	/*
	 * We're starting a new edit. Clear any validation status,
	 * which is now out of date, and abort any outstanding
	 * connections.
	 */
	if (this.request) {
	    YAHOO.util.Connect.abort(this.request, null, false);
	    this.loading.style.display = "none";	    
	    this.request = null;
	}
	this.show("&nbsp;");
    }

    var self = this;
    this.timer = setTimeout(function() {
	/*
	 * The user stopped editing. Open a remote validation
	 * connection.
	 */
	self.timer = null;
	self.updateData();
    }, this.editTimeout);
};

Validator.show = function(message, is_error)
{
    if (is_error) {
	AddClass(this.status, "error");
    } else {
	KillClass(this.status, "error");
    }
    this.status.innerHTML = message;
};

Validator.updateData = function()
{
    var self = this;

    self.loading.style.display = "block";

    var responseSuccess = function(req) {
	self.loading.style.display = "none";
	self.request = null;

	var obj = req.responseText.parseJSON();

	if (obj.is_valid) {
	    self.show(self.validMessage);
	} else {
	    self.show(obj.messages[0].replace(/&/g, '&amp;').replace(/</g, '&lt;'), true);
	}
    }

    var responseFailure = function(req) {
	self.loading.style.display = "none";
	self.request = null;

	self.show("Connection error during validation (" + req.status + ")");
    }

    var callback = {
	success: responseSuccess,
	failure: responseFailure,
	timeout: 5000
    };

    self.request = YAHOO.util.Connect.asyncRequest('POST', this.url, callback,
						   'content=' + escape(this.field.value));
};
