#!/usr/bin/env python
# coding=utf-8

from django import template
from bonvivant.lib import human

register = template.Library()

@register.filter
def prettynumber(value):
    """
    Thin wrapper around the lib version, why not
    """
    return human.prettynumber(value)
