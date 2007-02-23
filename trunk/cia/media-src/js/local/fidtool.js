/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 *
 * FIDtool client-side grapher.
 */

/*
 * FIDGraph constructor. Creaes a new graphing widget
 * inside the specified container. This automatically
 * begins fetching counter information so that we can
 * scale the graph and begin downloading images.
 */
var FIDGraph = function(url, containerId) {
    this.url = url;
    this.container = document.getElementById(containerId);

    var self = this;
    var responseSuccess = function(req) {
	/*
	 * The server gives us a dictionary of time names
	 * like 'now', 'month_ago', 'hour_ago', etc. Each
	 * one is a tuple of (query_time, sample_time, sample_index).
	 *
	 * query_time is the server's corresponding time for that
	 * name. counts.now[0] would be the server's current time,
	 * for example. This is the time that is queried for in the
	 * Fast Interval Database.
	 *
	 * sample_time is the timestamp on the matching FID sample,
	 * and sample_index is the index of the matching sample.
	 */
        var counts = req.responseText.parseJSON();

	/*
	 * Select the interval over which we'll scale the graph
	 */
	var now = counts.now;
	var interval = counts.month_ago;

	/*
	 * From this, we can calculate the extent of the graph's
	 * X axis. These are the minimum and maximum timestamps.
	 */
	var x_min = interval[0];
	var x_max = now[0];

	/*
	 * Using the FID indices, calculate the average frequency
	 * (in events per second) over the range of this graph.
	 * We'll use this to pick a reasonable scale for the Y axis.
	 */
	var avg_freq = (now[2] - interval[2]) / (x_max - x_min);

	var width = 500;
	var height = 100;

	var x_scale = (x_max - x_min) / width;
	var y_scale = 10000;
	var y_gridsize = 1000;

       	self.container.innerHTML =
	    '<img src="' + self.url + 'graph/?w=' + width
                                    + '&h=' + height
	                            + '&xo=' + x_min
                                    + '&xs=' + x_scale
                                    + '&ys=' + y_scale
                                    + '&yg=' + y_gridsize
	        + '" width="' + width
	        + '" height="' + height
            + '" />';
    }

    var responseFailure = function(req) {
	// XXX
    }

    var callback = {
	success: responseSuccess,
	failure: responseFailure,
	timeout: 5000
    };

    YAHOO.util.Connect.asyncRequest('GET', this.url + 'counts/', callback);
};
