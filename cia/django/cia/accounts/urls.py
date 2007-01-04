from django.conf.urls.defaults import *

urlpatterns = patterns('',
    (r'^$', 'cia.accounts.views.index'),
    (r'^logout/$', 'cia.accounts.views.logout'),
    (r'^password_change/$', 'cia.accounts.views.password_change'),
    (r'^password_change/done/$', 'cia.accounts.views.password_change_done'),

    (r'^projects/(?P<project_id>\d+)/$', 'cia.accounts.views.project'),
    (r'^authors/(?P<project_id>\d+)/$', 'cia.accounts.views.author'),
    (r'^bots/(?P<bot_id>\d+)/$', 'cia.accounts.views.bot'),
)
