/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

var htmlEscape = function(str)
{
    return str.replace(/&/g, '&amp;').replace(/</g, '&lt;');
}
