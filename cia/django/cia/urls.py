from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/', include('cia.accounts.urls')),
    (r'^admin/', include('django.contrib.admin.urls')),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^%s(?P<path>.*)$' % settings.MEDIA_URL[1:], 'django.views.static.serve', dict(
            document_root = settings.MEDIA_ROOT,
            show_indexes = True,
        ))
    )
    
