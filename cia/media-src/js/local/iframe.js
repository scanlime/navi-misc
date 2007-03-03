/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var iFrameResultCallbacks = {}

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

    /*
     * XXX: If we couldn't guess the height correctly, assign a reasonable
     *      default. This may happen if the iframe is within a hidden
     *      block when it loads, and it always happens on some browsers
     *      (like Konqueror).
     */
    if (h < 10) {
	h = 150;
    }

    childWindow.frameElement.style.height = h + "px";

    /*
     * If the child iframe has a "form-result" hidden input element,
     * propagate that value to the parent...
     */
    var childResult = childDocument.getElementById("form-result");
    if (childResult) {

	/* If the parent has a corresponding form field, copy the value there */
	var parentResult = document.getElementById(childWindow.name + "_result");
	if (parentResult) {
	    parentResult.value = childResult.value;
	}

	/* If we have a special handler function in the parent, call it */
	var handler = iFrameResultCallbacks[childWindow.name];
	if (handler) {
	    handler(childResult.value);
	}
    }
}
