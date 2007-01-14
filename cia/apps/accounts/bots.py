from cia.apps.accounts import models, assets, authplus
from django import newforms as forms
from django.conf import settings
from django.contrib.contenttypes.models import ContentType
from django.http import HttpResponseRedirect, Http404
from django.shortcuts import render_to_response


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

class AddNetworkForm(forms.Form):
    netname = forms.CharField(max_length=200)
    server = forms.RegexField(r"^([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,6}(:\d+)?$", max_length=120,
                               error_message='Must be a valid hostname with optional port.')

class MultiForm:
    """A simple wrapper for conditionally validating multiple forms
       with the same data and error dictionaries.
       """
    def __init__(self, bindTo):
        self.bindTo = bindTo
        self.data = {}
        self.clean_data = {}
        self.errors = {}

    def validate(self, form):
        inst = form(self.bindTo)
        inst.full_clean()
        self.errors.update(inst.errors)
        self.data.update(inst.data.items())
        if hasattr(inst, 'clean_data'):
            self.clean_data.update(inst.clean_data.items())

def validate_network(form, field_name='network'):
    try:
        return models.Network.objects.get(pk=int(form.data.get('network')))
    except (ValueError, models.Network.DoesNotExist):
        form.errors['network'] = forms.util.ErrorList(["Select a network."])

@authplus.login_required
def add_bot(request, asset_type):
    form = MultiForm(request.POST)

    if request.POST:
        form.validate(AddBotForm)
        if form.data.get('network') == '_other':
            form.validate(AddNetworkForm)
            network = None
        else:
            network = validate_network(form)

        if not form.errors:
            if not network:
                # No network was explicitly mentioned in the
                # form- the user picked "Other". We'll look
                # up an existing network using the supplied
                # hostname, creating a new one if necessary.

                uri = "irc://%s/" % form.clean_data['server']
                network = models.Network.objects.get_or_create(
                    uri__iexact = uri,
                    defaults = dict(uri = uri,
                                    description = form.clean_data['netname'],
                                    created_by = request.user),
                    )[0]

            location = normalize_channel_to_location(form.clean_data['channel'])

            # Now look up a matching bot. We might have to create this too.
            bot = models.Bot.objects.get_or_create(
                network = network,
                location__iexact = location,
                defaults = dict(location=location),
                )[0]

            # Finally, create a new UserAsset.
            user_asset = models.UserAsset.objects.get_or_create(
                user = request.user,
                content_type = ContentType.objects.get_for_model(models.Bot),
                object_id = bot.id,
                )[0]

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
    return render_to_response('accounts/add_bot.html', ctx)


###########################
#        Edit Bot         #
###########################

class EditBotForm(forms.Form):
    filter_mode = forms.ChoiceField(models.filter_mode_choices, widget=forms.RadioSelect)
    custom_ruleset = forms.CharField()
    project_list = forms.CharField()
    show_project_names = forms.BooleanField()

@authplus.login_required
def bot(request, asset_type, asset_id):
    ctx = assets.get_asset_edit_context(request, asset_type, asset_id)
    user_asset = ctx['user_asset']
    bot = user_asset.asset

    form = EditBotForm(request.POST or bot)

    ctx.update({
        'form': form,
        'network_host': bot.network.getHost('irc'),
        'channel': get_channel_from_location(bot.location),
        })
    return render_to_response('accounts/bot.html', ctx)
