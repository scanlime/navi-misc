/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var BotStatus = {};

BotStatus.init = function(url)
{
    this.div_loading = document.getElementById("bot-status-loading");
    this.div_message = document.getElementById("bot-status-message");
    this.url = url;
    this.updateData();
};

BotStatus.showBotStatus = function(obj)
{
    this.disableTimeUpdates();
    
    if (!obj.request || !obj.request.is_active)
    {
	/*
	 * There is no request, or the request is marked inactive.
	 * Don't bother updating until the user submits a bot request.
	 */
	this.div_message.innerHTML = "<p>Inactive.</p>";
	return;
    }

    if (!obj.request.is_fulfilled || !obj.request.bots)
    {
	/*
	 * There is a request, but it's currently unfulfilled. 
	 * Reload pretty frequently, since the bot should be
	 * connecting Real Soon Now.
	 */
	this.div_message.innerHTML = "<p>Waiting for bots to connect...</p>";
	this.scheduleDataUpdate(5);
	return;
    }

    var bot = obj.request.bots[0];
    var status = "<p>Ready.</p>"

    status += "<p>Your bot is <strong>" + bot.nickname +
    "</strong> on " + this.formatNetwork(bot.network) + "</p>";

    status += "<p>Lag: " + TimeUnitsAbbrev.format(bot.lag) + "</p>";

    /* Convert the connection timestamp from server time to local time */
    var now = (new Date()).getTime() / 1000.0;
    var local_time_offset = now - bot.current_time;
    this.localConnectTime = new Date(1000 * (bot.connect_time + local_time_offset));

    /* Begin an auto-updating uptime counter */
    status += "<p>Uptime: <span id=\"bot-upcounter\" />" + this.formatUptime() + "</p>";
    this.enableTimeUpdates();

    /*
     * Update relatively slowly, since the bot is functioning normally
     * and nothing aside from lag will change unless provoked to.
     */
    this.div_message.innerHTML = status;
    this.scheduleDataUpdate(60);
};

BotStatus.formatNetwork = function(network)
{
    /*
     * Network names are represented by the server as a
     * 3-tuple of (name, server, port). The "name" is the
     * hostname-part of an irc:// URI, which may be a real
     * server name or an abstract network name.
     *
     * The "server" part always identifies the actual server
     * hostname that was selected. We'll display that, and
     * the port if it's non-default.
     */
    if (network[2] != 6667) {
	return network[1] + ":" + network[2];
    }
    return network[1];
}

BotStatus.formatUptime = function()
{
    var now = new Date();
    var diff = now - this.localConnectTime;

    var sec = Math.floor(diff / 1000) % 60;
    var min = Math.floor(diff / (60 * 1000)) % 60;
    var hour = Math.floor(diff / (60 * 60 * 1000)) % 24;
    var day = Math.floor(diff / (24 * 60 * 60 * 1000));

    var pad = function(i) {
	return (i < 10) ? ("0" + i) : i;
    }

    var s = pad(min) + ":" + pad(sec);
    if (hour) {
	s = pad(hour) + ":" + s;
    }
    if (day) {
	s = day + 'd ' + s;
    }

    return s;
}

BotStatus.updateTimes = function()
{
    document.getElementById("bot-upcounter").innerHTML = this.formatUptime();
}

BotStatus.enableTimeUpdates = function()
{
    var self = this;
    self.timeUpdateInterval = setInterval(function() {
	self.updateTimes();
    }, 1000);
}

BotStatus.disableTimeUpdates = function()
{
    if (this.timeUpdateInterval) {
	clearInterval(self.timeUpdateInterval);
    }
}
    
BotStatus.showError = function(code)
{
    this.disableTimeUpdates();
    this.div_message.innerHTML = "<p>Error checking bot status! (" + code + ")</p>";

    /*
     * We'll automatically retry if this appears to be a server error,
     * or if it was a network error while contacting the server.  It
     * may be something transient, like a daemon restart or an
     * overloaded proxy.
     */
    if (code <= 0 || code >= 500) {
	this.scheduleDataUpdate(60);
    }
};

BotStatus.scheduleDataUpdate = function(seconds)
{
    var self = this;
    if (self.updateTimer) {
	clearTimeout(self.updateTimer);
    }
    self.timer = setTimeout(function() { 
	self.timer = null;
	self.updateData();
    }, seconds * 1000);
};
    
BotStatus.updateData = function()
{
    var self = this;

    self.div_loading.style.display = "block";
    
    var responseSuccess = function(req) {
	self.div_loading.style.display = "none";
	self.showBotStatus(JSON.parse(req.responseText))
    }
    
    var responseFailure = function(req) {
	self.div_loading.style.display = "none";
	self.showError(req.status);
    }
    
    var callback = {
	success: responseSuccess,
	failure: responseFailure,
	timeout: 15000,
    };
    
    self._request = YAHOO.util.Connect.asyncRequest('GET', this.url, callback);
};
