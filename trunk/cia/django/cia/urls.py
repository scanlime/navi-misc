from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/login/$', 'cia.accounts.views.login', dict(next_page='/account/')),
    (r'^account/logout/$', 'django.contrib.auth.views.logout', dict(next_page=settings.LOGIN_URL)),
    (r'^account/register/$', 'cia.accounts.views.register', dict(next_page='/account/')),
    (r'^account/forgot/$', 'cia.accounts.views.forgot', dict(next_page=settings.LOGIN_URL)),

    (r'^account/$', 'cia.accounts.views.profile'),
    (r'^account/(?P<asset_type>\w+)/add/$', 'cia.accounts.views.add_asset'),
    (r'^account/(?P<asset_type>\w+)/(?P<asset_id>\d+)/$', 'cia.accounts.views.asset'),

    (r'^admin/', include('django.contrib.admin.urls')),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^media/(?P<path>.*)$', 'django.views.static.serve', dict(
            document_root = settings.MEDIA_ROOT,
            show_indexes = True,
        ))
    )
    
