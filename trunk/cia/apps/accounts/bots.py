from cia.apps.accounts import models, assets, authplus, formtools
from django import newforms as forms
from django.conf import settings
from django.contrib.contenttypes.models import ContentType
from django.http import HttpResponseRedirect
from django.shortcuts import render_to_response
from django.template.context import RequestContext


def get_channel_from_location(location):
    """The channel names used in irc:// URIs have
       an implicit '#' at the beginning. This function
       emulates the backend's behaviour: add a single '#'
       if no prefix is present, but if the channel already
       has a prefix, leave it intact.
       """
    if location.startswith('#'):
        return location
    else:
        return '#' + location

def normalize_channel_to_location(channel):
    """The backend's irc:// URIs support multiple
       representations of any one channel, since the
       leading '#' is optional. This function normalizes
       a channel into the format used by all previous
       admin tools, to prevent compatibility problems
       between rulesets.
       """
    if channel.startswith('#') and not channel.startswith('##'):
        return channel[1:]
    else:
        return channel


###########################
#        Add Bot          #
###########################

class AddBotForm(forms.Form):
    network = forms.CharField()
    channel = forms.RegexField(r"^[^\s\x00-\x1f,%]+$", max_length=63,
                               error_message='Must be a valid IRC channel name.')

    def is_other_network(self):
        return self.data.get('network') == '_other'

    def clean_network(self):
        if not self.is_other_network():
            try:
                return models.Network.objects.get(pk=int(self.clean_data['network']))
            except (ValueError, models.Network.DoesNotExist):
                raise forms.ValidationError("Select a network.")

class AddNetworkForm(forms.Form):
    netname = forms.CharField(max_length=200)
    server = forms.RegexField(r"^([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,6}(:\d+)?$", max_length=120,
                               error_message='Must be a valid hostname with optional port.')

    def get_or_create(self, request):
        """Look up an existing network using this server name,
           creating one if it doesn't already exist.
           """
        uri = "irc://%s/" % self.clean_data['server']
        return models.Network.objects.get_or_create(
            uri__iexact = uri,
            defaults = dict(uri = uri,
                            description = self.clean_data['netname'],
                            created_by = request.user),
            )[0]

@authplus.login_required
def add_bot(request, asset_type):
    form = formtools.MultiForm(request.POST)

    if request.POST:
        form.validate(AddBotForm)
        if form.AddBotForm.is_other_network():
            form.validate(AddNetworkForm)

        if form.is_valid():
            meta = []

            # Get/create the network, now that we know all forms validated
            network = form.clean_data['network'] or form.AddNetworkForm.get_or_create(request)
            location = normalize_channel_to_location(form.clean_data['channel'])

            # Now look up a matching bot. We might have to create this too.
            defaults = dict(
                network = network,
                location = location,
                )
            bot, created_bot = models.Bot.objects.get_or_create(
                network = network,
                location__iexact = location,
                defaults = defaults,
                )
            if created_bot:
                meta.append('_created')
                changes = defaults
            else:
                changes = {}

            # Finally, create a new UserAsset.
            user_asset, created_user_asset = models.UserAsset.objects.get_or_create(
                user = request.user,
                content_type = ContentType.objects.get_for_model(models.Bot),
                object_id = bot.id,
                )
            if created_user_asset:
                meta.append('_gained_access')

            # Record these changes
            models.AssetChangeset.objects.store_changes(
                user = request.user,
                asset = bot,
                meta = meta,
                changes = changes,
                )

            return HttpResponseRedirect("/account/%s/%s/" % (
                user_asset.asset._meta.asset_type, user_asset.id,
                ))

    # Automatically initialize the Network list, if it's empty.
    allNetworks = list(models.Network.objects.all())
    if not allNetworks:
        models.Network.objects.importNetworks()
        allNetworks = list(models.Network.objects.all())

    ctx = assets.get_asset_add_context(request, asset_type)
    ctx.update({
        'networks': allNetworks,
        'form': form,
        })
    return render_to_response('accounts/bot_add.html', RequestContext(request, ctx))


###########################
#        Edit Bot         #
###########################

class EditBotForm(forms.Form):
    filter_mode = forms.ChoiceField(
        choices = models.filter_mode_choices,
        widget = forms.RadioSelect,
        )
    project_list = forms.CharField(
        required = False,
        widget = forms.Textarea,
        )
    show_project_names = forms.BooleanField(
        required = False,
        widget = forms.CheckboxInput(attrs = {'class': 'checkbox'}),
        )

    def clean_filter_mode(self):
        return int(self.clean_data['filter_mode'])

    def clean_project_list(self):
        # Remove extra whitespace, split into individual projects.
        projects = []
        for line in self.clean_data['project_list'].split('\n'):
            line = line.strip()
            if line:
                projects.append(line)

        # Is the project list required?
        if int(self.data['filter_mode']) == models.FILTER.PROJECT_LIST and not projects:
            raise forms.ValidationError("Please enter at least one project.")

        return '\n'.join(projects)

    custom_ruleset = forms.CharField(
        required = False,
        widget = forms.Textarea,
        )

    def clean_custom_ruleset(self):
        # Empty rulesets are okay if we aren't using them. We never
        # allow a ruleset to be stored if it isn't well-formed and
        # valid.
        allow_empty = int(self.data['filter_mode']) != models.FILTER.CUSTOM

        # Use LibCIA to validate the ruleset. It would be nice to
        # hilight errors, or even interactively validate rulesets on
        # the client side.. but this is sufficient for now.
        return models.validate_ruleset(self.clean_data['custom_ruleset'], allow_empty)


@authplus.login_required
def bot(request, asset_type, asset_id):
    ctx = assets.get_asset_edit_context(request, asset_type, asset_id)
    user_asset = ctx['user_asset']
    bot = user_asset.asset
    bot.syncFromServer()

    form = formtools.MultiForm(request.POST)
    form.validate(EditBotForm, bot, post_defaults={
        # Checkboxes must be explicitly defaulted to zero here, since
        # an unchecked box will not show up in our POST data.
        'show_project_names': 0,
        })
    form.validate(assets.EditAssetForm, user_asset)

    if request.POST and form.is_valid():
        meta = []

        if form.EditAssetForm.should_delete():
            meta.append('_lost_access')

        models.AssetChangeset.objects.apply_changes(
            user = request.user,
            asset = bot,
            changes = form.EditBotForm.clean_data,
            meta = meta,
            )
        bot.syncToServer()

        if form.EditAssetForm.should_delete():
            return form.EditAssetForm.delete(request, user_asset)
        else:
            # XXX: This message should be more detailed, and it should
            #      maybe be generated by apply_changes().
            request.user.message_set.create(message="Your bot was updated successfully.")

    ctx.update({
        'form': form,

        'FILTER': models.FILTER,
        'modes': formtools.RadioChoices(form['filter_mode'], models.FILTER),

        'ACCESS': models.ACCESS,
        'levels': formtools.RadioChoices(form['access'], models.ACCESS),

        'network_host': bot.network.getHost('irc'),
        'channel': get_channel_from_location(bot.location),
        })
    return render_to_response('accounts/bot_edit.html', RequestContext(request, ctx))
