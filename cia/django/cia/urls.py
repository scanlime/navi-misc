from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/login/$', 'cia.accounts.views.login_form', dict(next_page='/account/')),
    (r'^account/logout/$', 'django.contrib.auth.views.logout', dict(next_page=settings.LOGIN_URL)),
    (r'^account/register/$', 'cia.accounts.views.index'),
    (r'^account/forgot-password/$', 'cia.accounts.views.index'),
                       
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
    
