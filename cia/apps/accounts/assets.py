from cia.apps.accounts import models, authplus
from django.http import Http404, HttpResponseRedirect
from django.shortcuts import render_to_response
from django.template.context import RequestContext
import django.newforms as forms


###########################
#    Asset Navigation     #
###########################

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

def get_asset_add_context(request, asset_type):
    """Returns extra context items used when adding any asset. This
       raises a 404 error if the asset_type is not valid. This
       includes all context variables required for navigation.

       Note that this doesn't return a RequestContext directly, and
       for a good reason. Context processors invoked by RequestContext
       may have side-effects, like deleting a user's pending
       messages. If these side-effects are wanted at all, they should
       occur only when the view is ready for them.
       """
    model = get_asset_by_type(asset_type)
    return {
        'asset_types': get_user_asset_types(request, asset_type),
        'asset_type': asset_type,
        'asset_type_name': model._meta.verbose_name,
        'user_assets': model.objects.all_for_user(request.user),
        'add': True,
        'form_path': request.path,
        }

def get_asset_edit_context(request, asset_type, asset_id):
    """Returns a context for editing any asset. This raises a 404
       error if the asset_type is not valid or if the asset_id does
       not refer to a UserAsset for the current user. The result
       includes all context variables required for navigation.
       """
    model = get_asset_by_type(asset_type)
    asset_id = int(asset_id)
    try:
        user_asset = model.objects.all_for_user(request.user).get(pk=asset_id)
    except models.UserAsset.DoesNotExist:
        raise Http404

    # Set as the default asset for this type
    request.session['default_' + asset_type] = asset_id

    return {
        'asset_types': get_user_asset_types(request, asset_type),
        'asset_type': asset_type,
        'asset_type_name': model._meta.verbose_name,
        'user_assets': model.objects.all_for_user(request.user),
        'asset_id': asset_id,
        'user_asset': user_asset,
        'form_path': request.path,
        }


###########################
#     Profile Editing     #
###########################

@authplus.login_required
def profile(request):
    if 'change-password' in request.POST:
        password_form = authplus.do_change_password(request)
    else:
        password_form = None

    if 'change-profile' in request.POST:
        profile_form = authplus.do_change_profile(request)
    else:
        profile_form = authplus.ChangeProfileForm(request.user)

    return render_to_response('accounts/profile.html', RequestContext(request, {
        'profile': True,
        'asset_types': get_user_asset_types(request),
        'password_form': password_form,
        'profile_form': profile_form,
        }))


###########################
#      Asset Editing      #
###########################

class EditAssetForm(forms.Form):
    access = forms.ChoiceField(
        choices = models.access_choices,
        widget = forms.RadioSelect,
        )

    def clean_access(self):
        level = int(self.clean_data['access'])

        # Don't allow users to set ownership levels that aren't allowed yet
        if level not in (models.ACCESS.NONE, models.ACCESS.COMMUNITY):
            level = models.ACCESS.COMMUNITY

        return level

    def should_delete(self):
        """Are we removing access to this asset?"""
        return self.clean_data['access'] == models.ACCESS.NONE

    def delete(self, request, user_asset):
        """Delete this UserAsset, create a message indicating that we
           were successful, then redirect back to the 'add' page.
           """
        user_asset.delete()
        request.user.message_set.create(message="Removed access to %s" % user_asset.asset)
        return HttpResponseRedirect("/account/%s/add/" % 
                                    user_asset.asset._meta.asset_type)


###########################
#      Stats Assets       #
###########################

@authplus.login_required
def stats_asset(request, asset_type, asset_id):
    """Generic form for editing stats-based assets"""
    ctx = get_asset_edit_context(request, asset_type, asset_id)
    return render_to_response('accounts/asset_edit.html', RequestContext(request, ctx))

@authplus.login_required
def add_stats_asset(request, asset_type):
    """Generic form for adding stats-based assets"""
    ctx = get_asset_add_context(request, asset_type)
    return render_to_response('accounts/asset_add.html', RequestContext(request, ctx))
