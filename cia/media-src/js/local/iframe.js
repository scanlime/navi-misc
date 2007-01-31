/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

/* This is called by child iframes when they load. We'll update their size */
iFrameLoaded = function(childDocument)
{
    /*
     * Calculate the child document height. This varies between
     * browsers...  try to cover all bases.
     */
    var cd = childDocument;
    var h = 0;
    if (cd.height) h = Math.max(h, cd.height);
    if (cd.body) {
	if (cd.body.offsetHeight) h = Math.max(h, cd.body.offsetHeight);
	if (cd.body.scrollHeight) h = Math.max(h, cd.body.scrollHeight);
    }

    /*
     * Find the matching iframe
     */
    var iframes = document.getElementsByTagName('iframe');
    for (var i=0, len=iframes.length; i<len; i++) {
	var frame = iframes[i];
	if (frame.contentDocument == childDocument) {
	    frame.style.height = h + "px";
	}
    }
}
