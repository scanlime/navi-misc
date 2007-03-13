/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2003-2007 Micah Dowty <micah@navi.cx>
 */

/*
 * Set a link's href according to the current state of a
 * form. This can be used to preserve the state of a form
 * via GET parameters. We use it in the security_widget
 * when switching between HTTP and HTTPS, for example.
 *
 * This is safe to call multiple times on the same link.
 * The original URL, including all original query arguments,
 * is preserved.
 *
 * Note that this will replace any existing query args
 * with the same names, rather than appending additional
 * values.
 */
var formStateToLink = function(link, form)
{
    /*
     * Retrieve the original URL, saving it if this is our first call.
     */
    if (!link.originalHref) {
	link.originalHref = link.href;
    }
    var href = link.originalHref;

    /*
     * Split the URL into path, query args, and fragment.
     */
    var url_parts = href.match(/([^?]*)(\?([^#]*))?(#.*)?/);
    var path = url_parts[1];
    var query_array = (url_parts[3] || "").split("&");
    var fragment = url_parts[4] || "";

    /*
     * Hash away the original query args into an object which
     * maps key names to URL-encoded values. We prefix each
     * key with '~' in order to keep us in a separate namespace
     * from built-in Object members and from Object members
     * supplied by other Javascript packages.
     */
    var query_dict = {};
    for (var i in query_array) {
	var key_value = query_array[i].split("=");
	if (key_value[0]) {
	    query_dict["~" + key_value[0]] = key_value[1] || "";
	}
    }

    /*
     * Add the form's values to our query argument dict,
     * overriding the ones put there by the original URL.
     *
     * This must explicitly skip 'password' fields.
     * XXX: Right now we only bother with 'text' fields.
     */
    for (var name in form) {
	var obj = form[name];
	if (name && obj && obj.type == 'text' && obj.value) {
	    /*
	     * We could use escape(obj.value) here, but that currently
	     * introduces Unicode badness since the browser knows how
	     * to escape Unicode characters as sequences like %u0000,
	     * and the server doesn't know how to decode those.
	     * 
	     * We'll cheat a little, and only manually replace a few
	     * characters.
	     */
	    query_dict["~" + name] = ('' + obj.value).replace(/%/g, "%25").replace(
							      /&/g, "%26").replace(
							      /#/g, "%23").replace(
							      /;/g, "%3B").replace(
							      /\+/g, "%2B").replace(
							      /\?/g, "%3F").replace(
							      /\//g, "%2F");
	}
    }

    /*
     * Re-package the query arguments
     */
    var args = [];
    for (var name in query_dict) {
	if (name && name[0] == "~") {
	    args.push(name.substr(1) + "=" + query_dict[name]);
	}
    }

    /*
     * Assemble the final string
     */
    link.href = path + "?" + args.join("&") + fragment;
};
