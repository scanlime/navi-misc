/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var ChangeHistory = {};

ChangeHistory.init = function(url, changesetListId, loadingId)
{
    this.url = url;
    this.changesetList = document.getElementById(changesetListId);
    this.loading = document.getElementById(loadingId);
    this.nextPage = 0;
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
	self.changesetList.innerHTML += req.responseText;
    }

    var responseFailure = function(req) {
	self.loading.style.display = "none";
	self.request = null;
	self.changesetList.innerHTML += "<li>Connection error during validation (" + req.status + ")</li>";
    }

    var callback = {
	success: responseSuccess,
	failure: responseFailure,
	timeout: 15000
    };

    self.request = YAHOO.util.Connect.asyncRequest(
       'GET', this.url + 'page' + self.nextPage + '/', callback)
};
