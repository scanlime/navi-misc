/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

/* This is called by child iframes when they load. We'll update their size */
iFrameLoaded = function(childDocument, childWindow)
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

    childWindow.frameElement.style.height = h + "px";
}
