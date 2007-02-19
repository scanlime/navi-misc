/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2003-2007 Micah Dowty <micah@navi.cx>
 */

/*
 * Control the visibility of an arbitrary element using a checkbox
 */
var autohideWithCheckbox = function(checkboxId, targetId)
{
    var checkbox = document.getElementById(checkboxId);
    var target = document.getElementById(targetId);

    checkbox.onchange = function () {
      target.style.display = checkbox.checked ? 'block' : 'none';
    };

    checkbox.onchange();
};
