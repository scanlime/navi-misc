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
 * Begin animating a group of messages gaining or losing visibility,
 * with a vertical blinds effect. Note that every messages div has
 * exactly one child div. We hide that child div during animation, to
 * prevent the animation from running unacceptably slow.
 */
var animateMessages = function(id, visible)
{
    var parent_div = document.getElementById(id);
    var content_div = parent_div.getElementsByTagName("div")[0]

    if (parent_div.currentAnim) {
	parent_div.currentAnim.stop();
    }

    /* Save the original height */
    var height = parent_div.offsetHeight;
    if (parent_div.originalHeight) {
	height = parent_div.originalHeight;
    } else {
	parent_div.originalHeight = height;
    }

    var resize = new YAHOO.util.Anim(parent_div);

    if (visible) {
	resize.attributes.height = { from: 0, to: height };
    } else {
	resize.attributes.height = { from: height, to: 0 };
	resize.setAttribute('height', height, 'px')
	content_div.style.display = "none";
    }

    resize.onComplete.subscribe(function() {
	if (visible) {
	    content_div.style.display = "block";
	}
	parent_div.currentAnim = null;
    });
				    
    resize.duration = 0.5;
    resize.method = YAHOO.util.Easing.easeOut;

    parent_div.currentAnim = resize;
    resize.animate();
}

/*
 * Begin animating a single expander gaining or losing visibility,
 * with a fade effect on browsers that support opacity.
 */
var animateExpander = function(id, visible)
{
    var div = document.getElementById(id);
    var fade = new YAHOO.util.Anim(div);

    if (div.currentAnim) {
	div.currentAnim.stop();
    }

    if (visible) {
	div.style.display = "inline";
	fade.attributes.opacity = { from: 0, to: 1 };
    } else {
	fade.attributes.opacity = { from: 1, to: 0 };
    }

    fade.onComplete.subscribe(function() {
	if (!visible) {
	    div.style.display = "none";
	}
	div.currentAnim = null;
    });

    fade.duration = 0.25;
    fade.method = YAHOO.util.Easing.easeOut; 

    div.currentAnim = fade;
    fade.animate();
}

/*
 * A 'day' expander was clicked. Toggle it, and set the visibility of
 * its child messages.
 */
var expandDay = function(y, m, d)
{
    var visible = toggleExpander(document.getElementById('expander-' + y + '-' + m + '-' + d)); 
    animateMessages('msg-' + y + '-' + m + '-' + d, visible);
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
    animateMessages('msg-' + y + '-' + m, visible);
    animateExpander('expander-' + y + '-' + m + '-' + d, visible);
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
    animateMessages('msg-' + y, visible);
    animateExpander('expander-' + y + '-' + m + '-' + d, visible);
    animateExpander('expander-' + y + '-' + m, visible);
}
