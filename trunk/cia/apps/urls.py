from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/', include('cia.apps.accounts.urls')),
    (r'^api/', include('cia.apps.api.urls')),
    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^doc/', include('cia.apps.doc.urls')),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^media/(?P<path>.*)$', 'django.views.static.serve', {
            'document_root': settings.MEDIA_ROOT,
            'show_indexes': True,
        })
    )
    
