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
     * copy its value to the proper hidden input element in our parent
     * document. The result element will have an ID equal to our iframe's
     * name plus '_result'.
     */
    var childResult = childDocument.getElementById("form-result");
    var parentResult = document.getElementById(childWindow.name + "_result");
    if (childResult && parentResult) {
	parentResult.value = childResult.value;
    }
}
