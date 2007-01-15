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
	    form.action = tabs.activeTab;
	}
    });
};

/*
 * Attach event handlers which fire when a textarea is modified.
 * This handler will be called on every keypress, not just when
 * the textarea loses focus.
 */
var attachOnTextareaChanged = function(obj, handler)
{
    AttachEvent(obj, "change", handler);
    AttachEvent(obj, "keypress", function(e) {
	/* Ignore tabs */
	if (e.keyCode != 9) {
	    handler(e);
	}
    });
};

/*
 * Shortcut to automatically check a checkbox when the indicated
 * textarea is changed.
 */
var setCheckboxOnTextChanged = function(checkboxId, textareaId) {
    var checkbox = document.getElementById(checkboxId);
    var textarea = document.getElementById(textareaId);

    attachOnTextareaChanged(textarea, function() {
	checkbox.checked = true;
    });
};
