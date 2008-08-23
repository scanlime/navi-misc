#
# misc.py -- Miscellaneous utilities.
#
# Copyright (c) 2007  David Trowbridge
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#


import logging

from django.core.cache import cache
from django.conf import settings
from django.conf.urls.defaults import url, RegexURLPattern
from django.contrib.sites.models import Site
from django.db.models.manager import Manager
from django.views.decorators.cache import never_cache


DEFAULT_EXPIRATION_TIME = 60 * 60 * 24 * 30 # 1 month


def cache_memoize(key, lookup_callable,
                  expiration=getattr(settings, "CACHE_EXPIRATION_TIME",
                                     DEFAULT_EXPIRATION_TIME),
                  force_overwrite=False):
    try:
        site = Site.objects.get(pk=settings.SITE_ID)

        # The install has a Site app, so prefix the domain to the key.
        key = "%s:%s" % (site.domain, key)
    except:
        # The install doesn't have a Site app, so use the key as-is.
        pass

    if not force_overwrite and cache.has_key(key):
        return cache.get(key)
    data = lookup_callable()

    # Most people will be using memcached, and memcached has a limit of 1MB.
    # Data this big should be broken up somehow, so let's warn about this.
    if len(data) >= 1024 * 1024:
        logging.warning("Cache data for key %s (length %s) may be too big "
                        "for the cache." % (key, len(data)))

    try:
        cache.set(key, data, expiration)
    except:
        pass
    return data


def get_object_or_none(klass, *args, **kwargs):
    if isinstance(klass, Manager):
        manager = klass
        klass = manager.model
    else:
        manager = klass._default_manager

    try:
        return manager.get(*args, **kwargs)
    except klass.DoesNotExist:
        return None


def never_cache_patterns(prefix, *args):
    """
    Prevents any included URLs from being cached by the browser.

    It's sometimes desirable not to allow browser caching for a set of URLs.
    This can be used just like patterns().
    """
    pattern_list = []
    for t in args:
        if isinstance(t, (list, tuple)):
            t = url(prefix=prefix, *t)
        elif isinstance(t, RegexURLPattern):
            t.add_prefix(prefix)

        t._callback = never_cache(t.callback)
        pattern_list.append(t)

    return pattern_list
