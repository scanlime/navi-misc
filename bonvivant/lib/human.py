from gourmet import convert

def prettynumber(value):
    """
    Return a pretty-printed, human number.  This is partially based on the AP
    style for numbers.

    >>> prettynumber(1.5)
    u'1 \xbd'

    >>> prettynumber(2)
    '2'

    >>> prettynumber(2999)
    '2999'

    >>> prettynumber(0.25)
    u'\xbc'
    """

    try:
        intv = int(value)
        floatv = float(value)
    except ValueError:
        # Non-numeric
        return value

    remainder = floatv - intv
    if remainder == 0:
        #if not 0 < value < 10:
        #    return '%d' % value
        #return ('one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine')[intv-1]
        return '%d' % intv
    else:
        return convert.float_to_frac(floatv)
