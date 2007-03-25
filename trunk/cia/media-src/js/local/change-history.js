/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var ChangeHistory = {};

ChangeHistory.init = function(url, changesetListId, loadingId)
{
    this.url = url;
    this.changesetList = document.getElementById("changesets"); 
    this.loading = document.getElementById("changes-loading");
    this.moreChanges = document.getElementById("more-changes");
    this.moreChangesCount = document.getElementById("more-changes-count");

    this.nextPage = 0;
    this.changesetList.innerHTML = '';
    this.moreChanges.style.display = "none";

    this.loadNextPage();
}

ChangeHistory.loadNextPage = function()
{
    var self = this;

    if (self.request) {
	/* Already loading */
	return;
    }

    self.loading.style.display = "block";

    var responseSuccess = function(req) {
	self.loading.style.display = "none";
	self.request = null;
	self.nextPage += 1;

	var obj = parseJSON(req.responseText);
	if (obj.remaining) {
	    self.moreChangesCount.innerHTML = obj.remaining;
	    self.moreChanges.style.display = "block";
	} else {
	    self.moreChanges.style.display = "none";
	}
	self.changesetList.innerHTML += obj.html;
    }

    var responseFailure = function(req) {
	self.loading.style.display = "none";
	self.request = null;
	self.changesetList.innerHTML += "<li>Connection error while retrieving changes (" + req.status + ")</li>";
    }

    var callback = {
	success: responseSuccess,
	failure: responseFailure,
	timeout: 15000
    };

    self.request = YAHOO.util.Connect.asyncRequest(
       'GET', this.url + 'page' + self.nextPage + '/', callback)
};
