from django.conf.urls.defaults import *
from django.conf import settings

urlpatterns = patterns('',
    (r'^account/login/$', 'cia.accounts.views.login_form', dict(next_page='/account/')),
    (r'^account/logout/$', 'django.contrib.auth.views.logout', dict(next_page=settings.LOGIN_URL)),
    (r'^account/register/$', 'cia.accounts.views.index'),
    (r'^account/forgot-password/$', 'cia.accounts.views.index'),
                       
    (r'^account/$', 'cia.accounts.views.profile'),
    (r'^account/projects/$', 'cia.accounts.views.default_project'),
    (r'^account/projects/add/$', 'cia.accounts.views.add_project'),
    (r'^account/projects/(?P<project_id>\d+)/$', 'cia.accounts.views.project'),
    (r'^account/authors/$', 'cia.accounts.views.default_author'),
    (r'^account/authors/add/$', 'cia.accounts.views.add_author'),
    (r'^account/authors/(?P<author_id>\d+)/$', 'cia.accounts.views.author'),
    (r'^account/bots/$', 'cia.accounts.views.default_bot'),
    (r'^account/bots/add/$', 'cia.accounts.views.add_bot'),
    (r'^account/bots/(?P<bot_id>\d+)/$', 'cia.accounts.views.bot'),

    (r'^admin/', include('django.contrib.admin.urls')),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^media/(?P<path>.*)$', 'django.views.static.serve', dict(
            document_root = settings.MEDIA_ROOT,
            show_indexes = True,
        ))
    )
    
