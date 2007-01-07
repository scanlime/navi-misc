from django import template
from django.conf import settings

register = template.Library()

@register.filter
def paren(value):
    """Surround a value with parens, hiding if it evaluates to False."""

    # Resolve callables, if necessary. See resolve_variable() in django.template.
    if callable(value):
        if getattr(value, 'alters_data', False):
            return settings.TEMPLATE_STRING_IF_INVALID
        else:
            try:
                value = value()
            except:
                return settings.TEMPLATE_STRING_IF_INVALID

    if value:
        return '(%s)' % value
    return ''
