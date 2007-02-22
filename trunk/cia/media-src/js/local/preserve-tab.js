/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2003-2007 Micah Dowty <micah@navi.cx>
 */

/*
 * Preserve the current Tabtastic tab when submitting a form
 */
var preserveCurrentTab = function(formId, tabsId)
{
    var form = document.getElementById(formId);
    var tabs = document.getElementById(tabsId);
    var origAction = form.action;

    AttachEvent(form, "submit", function() {
	if (tabs.activeTab) {
	    form.action = tabs.activeTab.tabHref;
	}
    });
};

