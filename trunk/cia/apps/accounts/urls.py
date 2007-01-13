from django.conf.urls.defaults import *
from django.conf import settings
import django.contrib.auth.views
from django.contrib import auth
from cia.apps.accounts import authplus, bots, assets

account_page = {
    'next_page': '/account/',
}

login_page = {
    'next_page': settings.LOGIN_URL,
}

lost_pages = {   
    'next_page': settings.LOGIN_URL,
    'recovery_page': '/account/reset/%s/',
}

urlpatterns = patterns('',
    (r'^login/$', authplus.login, account_page),
    (r'^logout/$', auth.views.logout, login_page),
    (r'^register/$', authplus.register, account_page),
    (r'^lost/$', authplus.lost, lost_pages),
    (r'^reset/(?P<key>\w+)/$', authplus.reset, account_page),

    (r'^$', assets.profile),

    (r'^(?P<asset_type>bots)/add/$', bots.add_bot),
    (r'^(?P<asset_type>bots)/(?P<asset_id>\d+)/$', bots.bot),

    (r'^(?P<asset_type>(projects|authors))/add/$', assets.add_stats_asset),
    (r'^(?P<asset_type>(projects|authors))/(?P<asset_id>\d+)/$', assets.stats_asset),
)
