from django.shortcuts import render_to_response
from django.contrib.auth import login, authenticate
from django.http import HttpResponseRedirect
from django.conf import settings
from cia.accounts import models

def login_required(view_func):
    """Simplified version of auth.decorators.login_required,
       which works with our LOGIN_URL and removes the 'next'
       parameter which we don't need yet.
       """
    def _checklogin(request, *args, **kwargs):
        if request.user.is_authenticated():
            return view_func(request, *args, **kwargs)
        else:
            return HttpResponseRedirect(settings.LOGIN_URL)
    _checklogin.__doc__ = view_func.__doc__
    _checklogin.__dict__ = view_func.__dict__
    return _checklogin


def login_form(request, next_page, template_name="accounts/login.html"):
    """Simple login form view which doesn't rely on Django's current
       inflexible oldforms-based auth view.
       """
    if request.POST:
        user = authenticate(username = request.POST['username'],
                            password = request.POST['password'])

        if user is None:
            error = "Incorrect username or password."
        elif not user.is_active:
            error = "This account is inactive."
        elif not request.session.test_cookie_worked():
            error = "Cookies must be enabled."
        else:
            request.session.delete_test_cookie()
            login(request, user)
            return HttpResponseRedirect(next_page)
    else:
        error = None
    request.session.set_test_cookie()
    return render_to_response(template_name, dict(
        error = error,
        login_url = settings.LOGIN_URL,
        ))

ASSET_MODELS

#
# This is to support navigation among our various
# user assets. Most pages substantially treat all
# assets identically, and due to duck typing in our
# model we can get away with this.
#
asset_info = [
    dict(slug = 'projects',
         name = 'Projects',
         model = models.Project,
         ),
    dict(slug = 'authors',
         name = 'Authors',
         model = models.Author,
         ),
    
    



@login_required
def profile(request):
    return render_to_response('layout_account.html', dict(
        profile = True,
        projects = Project.objects.all(),
        ))


@login_required
def default_project(request):
    return HttpResponseRedirect
