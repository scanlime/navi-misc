from django.conf.urls.defaults import *
from django.conf import settings
import os

urlpatterns = patterns('',
    (r'^account/', include('cia.apps.accounts.urls')),
    (r'^api/', include('cia.apps.api.urls')),
#    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^blog/', include('cia.apps.blog.urls')),
    (r'^doc/', include('cia.apps.doc.urls')),
    (r'^images/', include('cia.apps.images.urls')),
    (r'^stats-experimental/', include('cia.apps.stats.urls')),
    (r'^feedback/', 'cia.apps.feedback.feedback'),
)

# XXX: Experimental
if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^testing/fidtool/graph/', 'cia.apps.stats.views.fidtool_graph'),
        (r'^testing/fidtool/counts/', 'cia.apps.stats.views.fidtool_counts'),
        (r'^testing/fidtool/page/', 'cia.apps.stats.views.fidtool_page'),
    )

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^media/(?P<path>.*)$', 'django.views.static.serve', {
            'document_root': settings.MEDIA_ROOT,
            'show_indexes': True,
        })
    )
    
