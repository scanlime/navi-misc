from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/login/$', 'cia.apps.accounts.views.login', dict(next_page='/account/')),
    (r'^account/logout/$', 'django.contrib.auth.views.logout', dict(next_page=settings.LOGIN_URL)),
    (r'^account/register/$', 'cia.apps.accounts.views.register', dict(next_page='/account/')),
    (r'^account/lost/$', 'cia.apps.accounts.views.lost', dict(next_page=settings.LOGIN_URL,
                                                              recovery_page='/account/reset/%s/')),
    (r'^account/reset/(?P<key>\w+)/$', 'cia.apps.accounts.views.reset', dict(next_page='/account/')),

    (r'^account/$', 'cia.apps.accounts.views.profile'),

    (r'^account/(?P<asset_type>bots)/add/$', 'cia.apps.accounts.views.add_bot'),
    (r'^account/(?P<asset_type>bots)/(?P<asset_id>\d+)/$', 'cia.apps.accounts.views.bot'),

    (r'^account/(?P<asset_type>(projects|authors))/add/$', 'cia.apps.accounts.views.add_stats_asset'),
    (r'^account/(?P<asset_type>(projects|authors))/(?P<asset_id>\d+)/$', 'cia.apps.accounts.views.stats_asset'),

    (r'^admin/', include('django.contrib.admin.urls')),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^media/(?P<path>.*)$', 'django.views.static.serve', dict(
            document_root = settings.MEDIA_ROOT,
            show_indexes = True,
        ))
    )
    
