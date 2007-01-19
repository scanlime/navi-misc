#
# bonvivant.apps.accounts.views
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

from django.shortcuts import render_to_response
from django.template import RequestContext

from djblets.auth.util import login_required

from bonvivant.apps.recipes.models import Recipe

import datetime

###########################
#       User Profile      #
###########################

@login_required
def profile(request):
    box = request.user.recipe_box.all()
    authored = Recipe.objects.filter(author=request.user.id)
    return render_to_response('accounts/profile.html', RequestContext(request, {
        'box'      : box,
        'authored' : authored,
        }))
