from django import template

register = template.Library()

@register.filter
def paren(value):
    """Surround a value with parens, but only if it evaluates to True."""
    if value:
        return '(%s)' % value
    return ''
