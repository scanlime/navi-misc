from django.conf.urls.defaults import *
from cia.apps.stats import browser

urlpatterns = patterns('',
    # Valid stats paths consist of at least one path segment, each of
    # which is not allowed to begin with a dot.

    (r'^(?P<path>[^./][^/]*(/[^./][^/]*)*)/$', browser.stats_page),
)
