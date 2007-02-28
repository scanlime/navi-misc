from cia.apps.accounts import models, authplus, formtools
from cia.apps.api.util import json_result
from cia.apps.images.widgets import ImageWidget
from cia.apps.stats.models import StatsTarget
from cia.apps.repos.models import Repository
from cia.apps.mailutil import get_email_for_user, render_to_email
from django.core.paginator import ObjectPaginator
from django.core.mail import mail_managers, send_mail
from django.http import Http404, HttpResponseRedirect
from django.shortcuts import render_to_response
from django.template.context import RequestContext, Context
from django.contrib.contenttypes.models import ContentType
import django.newforms as forms
from django.newforms.util import smart_unicode
from django.template import loader
import re


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
        'ACCESS': models.ACCESS,
        'add': True,
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
        'ACCESS': models.ACCESS,
        'asset_id': asset_id,
        'user_asset': user_asset,
        'other_community_user_assets': user_asset.asset.assets.exclude(user=request.user),
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

    def __init__(self, data=None):
        forms.Form.__init__(self, data)
        self.access_levels = formtools.RadioChoices(self['access'], models.ACCESS)

    def clean_access(self):
        return int(self.clean_data['access'])

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

    def apply(self, cset, request, user_asset):
        """Apply changes to a UserAsset, saving information about
           those changes in the provided changeset.
           """
        new_access = self.clean_data['access']
        if new_access != user_asset.access:

            if new_access == models.ACCESS.NONE:
                # We'll actually do the deletion later

                cset.set_meta('_lost_access')

            elif new_access == models.ACCESS.COMMUNITY:
                # Decreasing the access level

                request.user.message_set.create(message="You now have community-level access to %s" % user_asset.asset)
                assert user_asset.access > new_access
                cset.set_meta('_community_access')
                user_asset.access = new_access
                user_asset.save()

            elif new_access == models.ACCESS.EXCLUSIVE:
                # Increasting the access level. Gaining exclusive
                # access also means revoking access from any other
                # (community-access) users!
                
                request.user.message_set.create(message="You have taken exclusive access to %s" % user_asset.asset)
                cset.set_meta('_exclusive_access')
                user_asset.access = new_access
                user_asset.save()
                user_asset.asset.assets.exclude(user=request.user).delete()


@authplus.login_required
@json_result
def changes(request, asset_type, asset_id, page_number, num_per_page=10):
    """Return a paginated list of changes to a particular asset. This
       is called by some AJAX code in order to populate an asset's
       Change History box.
       """
    # Don't bother checking whether the user owns this asset, change
    # history should be public information anyway.
    changes = models.AssetChangeset.objects.filter(
        content_type = ContentType.objects.get_for_model(get_asset_by_type(asset_type)),
        object_id = int(asset_id),
        ).order_by('-id')

    paginator = ObjectPaginator(changes,
                                num_per_page = num_per_page,
                                orphans = num_per_page / 2)
    return {
        'remaining': paginator.hits - paginator.last_on_page(page_number),

        'html': smart_unicode(loader.render_to_string(
            'accounts/asset_changes.html',
            RequestContext(request, {
                'changesets': paginator.get_page(page_number),
            }))),
        }


def send_conflict_message(request, user_asset, message):
    """Send an e-mail message for conflict resolution purposes.
       The disputed user_asset must be provided, as well as a message
       to be sent to that user_asset's owner. A customized moderator
       message will be sent to all managers.
       """
    ctx = Context(locals())
    
    subject, message = render_to_email("accounts/conflict_mail_managers.txt", ctx)
    mail_managers(subject, message)

    subject, message = render_to_email("accounts/conflict_mail_user.txt", ctx)
    send_mail(subject, message, get_email_for_user(request.user),
              [get_email_for_user(user_asset.user)])

class ConflictForm(forms.Form):
    message = forms.CharField(widget=forms.Textarea)

@authplus.login_required
def conflict(request, asset_type, asset_id):
    """Asset conflict resolution. We redirect to this page when an
       'add asset' page encounters an exclusive access restriction.
       This informs the user of the situation, and gives them an
       opportunity to complain.
       """
    disable_submit = False
    model = get_asset_by_type(asset_type)
    ctx = get_asset_add_context(request, asset_type)
    try:
        asset = model.objects.get(pk=int(asset_id))
    except model.DoesNotExist:
        raise Http404

    # Find the exclusive owner of this asset. If there is none, this
    # page is not valid: return a 404.
    try:
        owner_ua = asset.assets.get(access__gte = models.ACCESS.EXCLUSIVE)
    except models.UserAsset.DoesNotExist:
        raise Http404
    assert owner_ua.asset == asset

    form = ConflictForm(request.POST)
    if request.POST:
        form.full_clean()
        if form.is_valid():
            send_conflict_message(request, owner_ua, form.clean_data['message'])
            request.user.message_set.create(message="Message sent.")

            # Make it less convenient to send a bunch of rapid-fire messages
            disable_submit = True
    
    ctx.update({
        'disable_submit': disable_submit,
        'form': form,
        'asset': asset,
        'owner_ua': owner_ua,
        })
    return render_to_response(('accounts/%s_conflict.html' % model._meta.object_name.lower(),
                               'accounts/asset_conflict.html'), RequestContext(request, ctx))


###########################
#      Stats Assets       #
###########################

class StatsMetadataForm(forms.Form):
    title = forms.CharField(
        widget = forms.TextInput(attrs = {'class': 'text'}),
        )
    subtitle = forms.CharField(
        required = False,
        widget = forms.TextInput(attrs = {'class': 'text'}),
        )
    url = forms.URLField(
        required = False,
        widget = forms.TextInput(attrs = {'class': 'text'}),
        # Disabled for now, since it seems to imply required=True.
        # verify_exists = True,
        )
    description = forms.CharField(
        required = False,
        widget = forms.Textarea,
        )
    photo_id = forms.IntegerField(
        required = False,
        widget = ImageWidget,
        )
    icon_id = forms.IntegerField(
        required = False,
        widget = ImageWidget,
        )

    def clean_subtitle(self):
        return self.clean_data.get('subtitle') or None

    def clean_url(self):
        return self.clean_data.get('url') or None

    def clean_description(self):
        return self.clean_data.get('description') or None

    def clean_photo_id(self):
        return self.clean_data.get('photo_id') or None

    def clean_icon_id(self):
        return self.clean_data.get('icon_id') or None

    def apply(self, cset):
        target = cset.asset.target
        cset.set_field_dict(self.clean_data, prefix='target.')

        if target.photo:
            target.photo.reference()
        if target.icon:
            target.icon.reference()

@authplus.login_required
def stats_asset(request, asset_type, asset_id):
    """Generic form for editing stats-based assets."""
    ctx = get_asset_edit_context(request, asset_type, asset_id)
    user_asset = ctx['user_asset']
    asset = user_asset.asset
    asset.target.enforce_defaults()

    form = formtools.MultiForm(request.POST)
    form.validate(EditAssetForm, user_asset)
    form.validate(StatsMetadataForm, asset.target)
    ctx['form'] = form

    if request.POST and form.is_valid():
        cset = models.AssetChangeset.objects.begin(request, asset)
        form.StatsMetadataForm.apply(cset)
        form.EditAssetForm.apply(cset, request, user_asset)
        cset.finish()

        if form.EditAssetForm.should_delete():
            return form.EditAssetForm.delete(request, user_asset)

    return render_to_response('accounts/stats_asset_edit.html', RequestContext(request, ctx))


class AddStatsAssetForm(forms.Form):
    name = forms.CharField(
        widget = forms.TextInput(attrs = {'class': 'text'}),
        )

@authplus.login_required
def add_stats_asset(request, asset_type, prefix, template, name=None):
    """Generic form for adding stats-based assets"""
    model = get_asset_by_type(asset_type)
    ctx = get_asset_add_context(request, asset_type)
    form = formtools.MultiForm(request.POST)
    form.validate(AddStatsAssetForm)

    if request.POST:
        if form.is_valid():
            name = form.clean_data['name']
    else:
        # Don't show errors if the form hasn't been submitted once
        form.errors = None

    # Names may be supplied via the form or via the URL.
    #
    # XXX: Generally it's a bad idea for GET requests to have side-effects
    #      like this. Our excuse is that the operation requires login and
    #      that the result is idempotent. If this turns out to be a problem
    #      anyway, we could redirect the user if the asset exists but only
    #      pre-fill the form if it doesn't yet exist.
    #
    if name:
        # Get/create the stats target
        target = StatsTarget.objects.get_or_create(path = prefix + name)[0]
        target.enforce_defaults()

        # Now get/create the matching asset
        asset, created_asset = model.objects.get_or_create(target = target)
        cset = models.AssetChangeset.objects.begin(request, asset)
        if created_asset:
            cset.set_meta('_created')

        # Finally, create a new UserAsset.
        user_asset = models.UserAsset.objects.get_or_create_if_allowed(request.user, asset, cset)

        cset.finish()

        # Redirect either to the new UserAsset or to a conflict resolution page
        if user_asset is None:
            return HttpResponseRedirect("/account/conflict/%s/%s/" % (asset_type, asset.id))
        else:
            return HttpResponseRedirect("/account/%s/%s/" % (asset_type, user_asset.id))

    ctx.update({
        'form': form,
        })
    return render_to_response(template, RequestContext(request, ctx))


###########################
#     Project Editing     #
###########################

class ProjectForm(forms.Form):
    use_repository = forms.BooleanField(
        required = False,
        widget = forms.CheckboxInput(attrs = {'class': 'checkbox'}),
        )

    def apply(self, cset, request, repos):
        use_repository = bool(self.clean_data.get('use_repository'))
        if cset.asset.repos and cset.asset.repos.is_active and not use_repository:
            # Deactivate repository

            cset.set_field('repos.is_active', False)

        elif use_repository:
            # Activate repository, using either the asset's existing
            # repository or a temporary repository we created before
            # we knew it would definitely be needed.

            if repos.id is None:
                repos.save()
            cset.set_field('repos', repos, quiet=True)
            cset.set_field('repos.is_active', True)

            # If we're activating an old repository, re-probe() it.
            # This will make sure the server still exists, but more
            # importantly it will update our latest-rev. Otherwise,
            # we'll soon start downloading all the revisions we missed
            # while the repository was inactive!

            if repos.location:
                repos.get_client().probe()

class RepositoryForm(forms.Form):
    location = forms.CharField(
        widget = forms.TextInput(attrs = {'class': 'text-wide'}),
        )
    enable_polling = forms.BooleanField(
        required = False,
        widget = forms.CheckboxInput(attrs = {'class': 'checkbox'}),
        )
    forward_pinger_mail = forms.BooleanField(
        required = False,
        widget = forms.CheckboxInput(attrs = {'class': 'checkbox'}),
        )
    poll_frequency = forms.IntegerField(
        widget = forms.TextInput(attrs = {'class': 'text'}),
        )
    revision_url = forms.CharField(
        required = False,
        widget = forms.TextInput(attrs = {'class': 'text-wide'}),
        )
    path_regexes = forms.CharField(
        required = False,
        widget = forms.Textarea,
        )

    def clean_location(self):
        # On success, this will set up internal state in the
        # model but *not* set the location itself. That will
        # be done during apply(), so that it's included in
        # the changeset.

        location = self.clean_data['location']
        if location != self.data.model.location:
            self.data.model.get_client().probe(location)
        return location

    def clean_revision_url(self):
        return self.clean_data.get('revision_url') or None

    def clean_path_regexes(self):
        # Remove extra whitespace and blank lines, then parse the resulting regexes.
        regexes = []
        line_no = 1
        for line in self.clean_data.get('path_regexes', '').split('\n'):
            line = line.strip()
            if line:
                try:
                    re.compile(line, re.VERBOSE)
                except re.error, e:
                    raise forms.ValidationError("Syntax error on line %d: %s" % (line_no, e))
                regexes.append(line)
            line_no += 1
        if regexes:
            return '\n'.join(regexes)

    def apply(self, cset):
        cset.set_field_dict(self.clean_data, prefix='repos.')


@authplus.login_required
def project(request, asset_type, asset_id):
    """Generic form for editing stats-based assets."""
    ctx = get_asset_edit_context(request, asset_type, asset_id)
    user_asset = ctx['user_asset']
    asset = user_asset.asset
    asset.target.enforce_defaults()

    # Non-top-level projects are currently only editable as generic stats items.
    if not asset.is_top_level():
        return stats_asset(request, asset_type, asset_id)

    form = formtools.MultiForm(request.POST)
    ctx['form'] = form
    form.validate(EditAssetForm, user_asset)
    form.validate(StatsMetadataForm, asset.target)
    form.validate(ProjectForm,
                  post_defaults = {'use_repository': False},
                  defaults = {'use_repository': asset.repos and asset.repos.is_active})

    use_repository = bool(form.ProjectForm.clean_data.get('use_repository'))
    if use_repository:
        repos = asset.repos
        if not repos:
            # Create a blank Repository immediately and bind it to the RepositoryForm,
            # but don't save it to the database until we know the submission is valid.
            # We need to do this because the process of probing a repository must occur
            # before we know the form is valid, but it changes model state which needs
            # to be saved if the form turns out to be valid.
    
            repos = Repository(
                created_by = request.user,
                project_name = asset.get_name(),
                pinger_name = Repository.objects.get_new_pinger_name(),
                is_active = False,
                )

        form.validate(RepositoryForm, repos,
                      post_defaults = {'enable_polling': False,
                                       'forward_pinger_mail': False})
    else:
        # It's okay to use a blank one here, we're guaranteed not to save it.
        repos = asset.repos or Repository()
        form.add(RepositoryForm, repos)

    if request.POST and form.is_valid():
        cset = models.AssetChangeset.objects.begin(request, asset)
        form.StatsMetadataForm.apply(cset)
        form.EditAssetForm.apply(cset, request, user_asset)
        form.ProjectForm.apply(cset, request, repos)
        if use_repository:
            form.RepositoryForm.apply(cset)
        cset.finish()

        if form.EditAssetForm.should_delete():
            return form.EditAssetForm.delete(request, user_asset)

    return render_to_response('accounts/project_edit.html', RequestContext(request, ctx))

