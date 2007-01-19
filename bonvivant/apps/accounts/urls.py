#
# urls.py
#
# Copyright (C) 2007 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

from django.conf import settings
from django.conf.urls.defaults import *
from django.contrib.auth import views as authcore

from djblets.auth import views as auth

from bonvivant.apps.accounts import views as accounts

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
    (r'^$',          accounts.profile),

    (r'^login/$',    auth.login,      account_page),
    (r'^logout/$',   authcore.logout, login_page),
    (r'^register/$', auth.register),
    (r'^lost/$',     auth.lost,       lost_pages),
)
