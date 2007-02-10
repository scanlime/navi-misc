from django import template
from django.conf import settings

register = template.Library()

def coerce_callable(value):
    if callable(value):
        if getattr(value, 'alters_data', False):
            return settings.TEMPLATE_STRING_IF_INVALID
        else:
            try:
                value = value()
            except:
                return settings.TEMPLATE_STRING_IF_INVALID
    return value

@register.filter
def paren(value):
    """Surround a value with parens, hiding if it evaluates to False."""
    value = coerce_callable(value)
    if value:
        return '(%s)' % value
    return ''

@register.inclusion_tag('accounts/person.html')
def person(user):
    return {'user': user}
