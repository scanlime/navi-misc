/* -*- Mode: C; c-basic-offset: 4 -*- */

var BotStatus = {};

BotStatus.init = function(url, containerId)
{
    this.container = document.getElementById(containerId);
    this.url = url;
};

BotStatus.showBotStatus = function(obj)
{
    var status;
    
    if (!obj.request || !obj.request.is_active)
    {
	/* There is no request, or the request is marked inactive. */
	status = "Request is inactive";
    }
    else if (!obj.request.is_fulfilled || !obj.request.bots)
    {
	/* There is a request, but it's currently unfulfilled. */
	status = "Waiting for bots to connect...";
    }
    else {
	status = "Bots are ready!";
	
	var bot = obj.request.bots[0];
	var network = bot.network[1] + ":" + bot.network[2];
	    
	var now = (new Date()).getTime() / 1000.0;
	var local_time_offset = now - bot.current_time;
	var local_connect_time = bot.connect_time + local_time_offset;

	status += "<br />Bot: ";

	status += network;
	status += "<br />";
	status += "Uptime: " + (now - local_connect_time);
    }
    
    this.container.innerHTML = status;
    this.scheduleUpdate(1.0);
};
    
BotStatus.showError = function(obj)
{
    var div = this.container;
    div.innerHTML = "<b>Error " + obj + "</b>";
};

BotStatus.scheduleUpdate = function(seconds)
{
    var self = this;
    setTimeout(function() { self.update() }, seconds * 1000);
};
    
BotStatus.update = function()
{
    var self = this;
    
    var responseSuccess = function(req) {
	self.showBotStatus(JSON.parse(req.responseText))
    }
    
    var responseFailure = function(req) {
	self.showError(req.status);
    }
    
    var callback = {
	success: responseSuccess,
	failure: responseFailure
    };
    
    self._request = YAHOO.util.Connect.asyncRequest('GET', this.url, callback);
};
