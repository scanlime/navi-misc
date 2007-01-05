from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/login/$', 'django.contrib.auth.views.login'),
    (r'^account/logout/$', 'django.contrib.auth.views.logout'),

    (r'^account/$', 'cia.accounts.views.index'),
    (r'^account/projects/(?P<project_id>\d+)/$', 'cia.accounts.views.project'),
    (r'^account/authors/(?P<project_id>\d+)/$', 'cia.accounts.views.author'),
    (r'^account/bots/(?P<bot_id>\d+)/$', 'cia.accounts.views.bot'),

    (r'^admin/', include('django.contrib.admin.urls')),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^%s(?P<path>.*)$' % settings.MEDIA_URL[1:], 'django.views.static.serve', dict(
            document_root = settings.MEDIA_ROOT,
            show_indexes = True,
        ))
    )
    
