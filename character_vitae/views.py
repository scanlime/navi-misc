# Copyright (C) 2006 W. Evan Sheehan
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

from character_vitae.Character.models import Character
from django.contrib.auth.models import User
from django.http import HttpResponse
from django.shortcuts import render_to_response

def profile(request):
    user = User.objects.get(username=request.META["USERNAME"])
    context = {
            "name": "%s %s" % (user.first_name, user.last_name),
            "character_list": Character.objects.filter(player=user).order_by("name")
            }
    response = render_to_response("profile.html", context)
    return response

# vim: ts=4:sw=4:et
