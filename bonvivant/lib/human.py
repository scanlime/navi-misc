from gourmet import convert

def prettynumber(value, fractions=convert.FRACTIONS_NORMAL):
    """
    Return a pretty-printed, human number.

    >>> prettynumber(1.5)
    u'1 \\xbd'

    >>> prettynumber(2)
    '2'

    >>> prettynumber(2999)
    '2999'

    >>> prettynumber(0.25)
    u'\\xbc'
    """

    try:
        intv = int(value)
        floatv = float(value)
    except ValueError:
        # Non-numeric
        return value

    remainder = floatv - intv
    if remainder == 0:
        return '%d' % intv
    else:
        return convert.float_to_frac(n=floatv, fractions=fractions)

def asciinumber(value):
    """
    Return a pretty printed number using only ascii characters

    >>> asciinumber(1.5)
    '1 1/2'

    >>> asciinumber(0.125)
    '1/8'
    """

    return prettynumber(value, convert.FRACTIONS_ASCII)
