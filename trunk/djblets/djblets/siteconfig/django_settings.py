#
# djblets/siteconfig/django_settings.py
#
# Copyright (c) 2008  Christian Hammond
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

from django.conf import settings


locale_settings_map = {
    'locale_timezone':             'TIME_ZONE',
    'locale_language_code':        'LANGUAGE_CODE',
    'locale_date_format':          'DATE_FORMAT',
    'locale_datetime_format':      'DATETIME_FORMAT',
    'locale_default_charset':      'DEFAULT_CHARSET',
    'locale_language_code':        'LANGUAGE_CODE',
    'locale_month_day_format':     'MONTH_DAY_FORMAT',
    'locale_time_format':          'TIME_FORMAT',
    'locale_time_zone':            'TIME_ZONE',
}

mail_settings_map = {
    'locale_year_month_format':    'YEAR_MONTH_FORMAT',
    'mail_server_address':         'SERVER_EMAIL',
    'mail_default_from':           'DEFAULT_FROM_EMAIL',
    'mail_host':                   'EMAIL_HOST',
    'mail_port':                   'EMAIL_PORT',
    'mail_host_user':              'EMAIL_HOST_USER',
    'mail_host_password':          'EMAIL_HOST_PASSWORD',
    'mail_use_tls':                'EMAIL_USE_TLS',
}

site_settings_map = {
    'site_media_root':             'MEDIA_ROOT',
    'site_media_url':              'MEDIA_URL',
    'site_prepend_www':            'PREPEND_WWW',
    'site_upload_temp_dir':        'FILE_UPLOAD_TEMP_DIR',
    'site_upload_max_memory_size': 'FILE_UPLOAD_MAX_MEMORY_SIZE',
}

cache_settings_map = {
    'cache_backend':               'CACHE_BACKEND',
    'cache_expiration_time':       'CACHE_EXPIRATION_TIME',
}


# Don't build unless we need it.
_django_settings_map = {}


def get_django_settings_map():
    """
    Returns the settings map for all Django settings that users may need
    to customize.
    """
    if not _django_settings_map:
        _django_settings_map.update(locale_settings_map)
        _django_settings_map.update(mail_settings_map)
        _django_settings_map.update(site_settings_map)
        _django_settings_map.update(cache_settings_map)

    return _django_settings_map


def _generate_defaults(settings_map):
    """
    Utility function to generate a defaults mapping.
    """
    defaults = {}

    for siteconfig_key, settings_key in settings_map.iteritems():
        if hasattr(settings, settings_key):
            defaults[siteconfig_key] = getattr(settings, settings_key)

    return defaults


def get_locale_defaults():
    """
    Returns the locale-related Django defaults that projects may want to
    let users customize.
    """
    return _generate_defaults(locale_settings_map)


def get_mail_defaults():
    """
    Returns the mail-related Django defaults that projects may want to
    let users customize.
    """
    return _generate_defaults(mail_settings_map)


def get_site_defaults():
    """
    Returns the site-related Django defaults that projects may want to
    let users customize.
    """
    return _generate_defaults(site_settings_map)


def get_cache_defaults():
    """
    Returns the cache-related Django defaults that projects may want to
    let users customize.
    """
    return _generate_defaults(cache_settings_map)


def get_django_defaults():
    """
    Returns all Django defaults that projects may want to let users customize.
    """
    return _generate_defaults(get_django_settings_map())


def apply_django_settings(siteconfig, settings_map=None):
    """
    Applies all settings from the site configuration to the Django settings
    object.
    """
    if settings_map is None:
        settings_map = get_django_settings_map()

    for key, value in settings_map.iteritems():
        if key in siteconfig.settings:
            setattr(settings, key, siteconfig.get(key))
