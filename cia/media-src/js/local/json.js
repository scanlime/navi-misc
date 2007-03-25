/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2007 Micah Dowty <micah@navi.cx>
 */

/*
 * Non-validating JSON evaluator. Only for use on trusted data.
 * (At least, as trusted as the source of this script...)
 */
var parseJSON = function(str)
{
    return eval("(" + str + ")");
}
