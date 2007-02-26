/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 *
 * Expander functions for the Stats UI.
 */

/*
 * Toggle an expander element, returning its new state.
 */
var toggleExpander = function(obj)
{
    var image = obj.getElementsByTagName("img")[0];
    if (HasClass(obj, "expander-closed")) {
	KillClass(obj, "expander-closed");
	AddClass(obj, "expander-open");
	image.src = "/media/img/tlexpand-open.png";
	return true;
    } else {
	AddClass(obj, "expander-closed");
	KillClass(obj, "expander-open");
	image.src = "/media/img/tlexpand-closed.png";
	return false;
    }
}

/*
 * A 'day' expander was clicked. Toggle it, and set the visibility of
 * its child messages.
 */
var expandDay = function(y, m, d)
{
    var visible = toggleExpander(document.getElementById('expander-' + y + '-' + m + '-' + d)); 
    document.getElementById('msg-' + y + '-' + m + '-' + d).style.display = visible ? "block" : "none";
}

/*
 * A 'month' expander was clicked. Toggle it, along with the
 * visibility of its child messages.  Also hide the 'day' expander
 * that appears on the same line, since it won't be automatically
 * hidden by the msg div.
 */
var expandMonth = function(y, m, d)
{
    var visible = toggleExpander(document.getElementById('expander-' + y + '-' + m)); 
    document.getElementById('msg-' + y + '-' + m).style.display = visible ? "block" : "none";
    document.getElementById('expander-' + y + '-' + m + '-' + d).style.display = visible ? "inline" : "none";
}

/*
 * A 'year' expander was clicked. Toggle it, along with the visibility
 * of its child messages.  We need to hide both the 'day' and 'month'
 * expanders on the same line, since they won't be automatically
 * hidden by the msg div.
 */
var expandYear = function(y, m, d)
{
    var visible = toggleExpander(document.getElementById('expander-' + y)); 
    document.getElementById('msg-' + y).style.display = visible ? "block" : "none";
    document.getElementById('expander-' + y + '-' + m + '-' + d).style.display = visible ? "inline" : "none";
    document.getElementById('expander-' + y + '-' + m).style.display = visible ? "inline" : "none";
}
