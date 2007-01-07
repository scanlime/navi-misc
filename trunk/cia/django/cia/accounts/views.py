from django.shortcuts import render_to_response, get_object_or_404
from django.contrib.auth import login, authenticate
from django.template.context import RequestContext
from django.http import HttpResponseRedirect, Http404
from django.conf import settings
from cia.accounts import models
import datetime


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
            login(request, user)
            request.session.delete_test_cookie()
            user.last_login = datetime.datetime.now()
            user.save()
            return HttpResponseRedirect(next_page)
    else:
        error = None
    request.session.set_test_cookie()
    return render_to_response(template_name, RequestContext(request, dict(
        error = error,
        login_url = settings.LOGIN_URL,
        )))

def get_default_asset_id(request, asset_type):
    """Return the default asset ID for a particular type. This will
       try, in order:

          1. The default recorded in the current session
          2. The first object returned by all_for_user
          3. 'add', to direct users to an asset creation page.
       """
    model = get_asset_by_type(asset_type)
    all = model.objects.all_for_user(request.user)

    default_id = request.session.get('default_' + asset_type)
    if default_id:
        try:
            all.get(pk=default_id)
            return default_id
        except models.UserAsset.DoesNotExist:
            pass

    try:
        return all[0].id
    except IndexError:
        pass

    return 'add'

def get_user_asset_types(request, current=None):
    """Return a list which summarizes a user's assets, for navigation."""
    return [{
        'asset_type': model._meta.asset_type,
        'is_current': model._meta.asset_type == current,
        'verbose_name_plural': model._meta.verbose_name_plural,
        'count': model.objects.all_for_user(request.user).count,
        'default_id': get_default_asset_id(request, model._meta.asset_type),
        }
        for model in models.AssetManager.models]

def get_asset_by_type(asset_type):
    for model in models.AssetManager.models:
        if model._meta.asset_type == asset_type:
            return model
    raise Http404

@login_required
def profile(request):
    return render_to_response('accounts/profile.html', RequestContext(request, dict(
        profile = True,
        asset_types = get_user_asset_types(request),
        )))

@login_required
def asset(request, asset_type, asset_id):
    model = get_asset_by_type(asset_type)
    asset_id = int(asset_id)
    user_asset = model.objects.all_for_user(request.user).get(pk=asset_id)

    # Set as the default asset for this type
    request.session['default_' + asset_type] = asset_id

    return render_to_response('accounts/asset.html', RequestContext(request, dict(
        asset_types = get_user_asset_types(request, asset_type),
        asset_type = asset_type,
        asset_type_name = model._meta.verbose_name,
        asset_id = asset_id,
        user_assets = model.objects.all_for_user(request.user),
        )))

@login_required
def add_asset(request, asset_type):
    model = get_asset_by_type(asset_type)

    return render_to_response('accounts/asset.html', RequestContext(request, dict(
        asset_types = get_user_asset_types(request, asset_type),
        asset_type = asset_type,
        asset_type_name = model._meta.verbose_name,
        user_assets = model.objects.all_for_user(request.user),
        add = True,
        )))
