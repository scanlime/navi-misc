#!/usr/bin/env python
# coding=utf-8

from django import template

register = template.Library()

@register.filter
def prettynumber(value):
    """
    Return a pretty-printed, human number.  This is partially based on the AP
    style for numbers.

    >>> prettynumber(1)
    'one'

    >>> prettynumber(5)
    'five'

    >>> prettynumber(1.5)
    u'1\\xbd'

    >>> prettynumber(10)
    '10'

    >>> prettynumber(2.4)
    '2.4'
    """

    try:
        intv = int(value)
    except ValueError:
        return value

    fractionmapping = {
        1.0 / 8 : u'⅛',
        1.0 / 6 : u'⅙',
        1.0 / 4 : u'¼',
        1.0 / 3 : u'⅓',
        1.0 / 2 : u'½',
        2.0 / 3 : u'⅔',
        3.0 / 4 : u'¾',
    }

    if value - intv == 0:
        # Integer
        if not 0 < value < 10:
            return '%d' % value
        return ('one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine')[value-1]
    else:
        try:
            return u'%d%s' % (intv, fractionmapping[value - intv])
        except KeyError:
            return '%.1f' % value
