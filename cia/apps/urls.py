from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/login/$', 'cia.apps.accounts.authplus.login', {
        'next_page': '/account/',
    }),
    (r'^account/logout/$', 'django.contrib.auth.views.logout', {
        'next_page': settings.LOGIN_URL,
    }),
    (r'^account/register/$', 'cia.apps.accounts.authplus.register', {
        'next_page': '/account/',
    }),
    (r'^account/lost/$', 'cia.apps.accounts.authplus.lost', {
        'next_page': settings.LOGIN_URL,
        'recovery_page': '/account/reset/%s/',
    }),
    (r'^account/reset/(?P<key>\w+)/$', 'cia.apps.accounts.authplus.reset', {
        'next_page': '/account/'
    }),

    (r'^account/$', 'cia.apps.accounts.assets.profile'),

    (r'^account/(?P<asset_type>bots)/add/$', 'cia.apps.accounts.bots.add_bot'),
    (r'^account/(?P<asset_type>bots)/(?P<asset_id>\d+)/$', 'cia.apps.accounts.bots.bot'),

    (r'^account/(?P<asset_type>(projects|authors))/add/$', 'cia.apps.accounts.assets.add_stats_asset'),
    (r'^account/(?P<asset_type>(projects|authors))/(?P<asset_id>\d+)/$', 'cia.apps.accounts.assets.stats_asset'),

    (r'^admin/', include('django.contrib.admin.urls')),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^media/(?P<path>.*)$', 'django.views.static.serve', {
            'document_root': settings.MEDIA_ROOT,
            'show_indexes': True,
        })
    )
    
