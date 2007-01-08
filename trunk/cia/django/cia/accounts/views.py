from cia.accounts import models
from django import newforms as forms
from django.conf import settings
from django.contrib import auth
from django.contrib.contenttypes.models import ContentType
from django.contrib.sessions.models import Session
from django.core.mail import send_mail
from django.http import HttpResponseRedirect, Http404
from django.shortcuts import render_to_response
from django.template import loader
from django.template.context import RequestContext, Context
import datetime, re, xmlrpclib


###########################
#       User Login        #
###########################

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


def internal_login(request, username, password):
    user = auth.authenticate(username=username, password=password)
    if user is None:
        return "Incorrect username or password."
    elif not user.is_active:
        return "This account is inactive."
    elif not request.session.test_cookie_worked():
        return "Cookies must be enabled."

    auth.login(request, user)
    request.session.delete_test_cookie()
    user.last_login = datetime.datetime.now()
    user.save()

def get_user(username):
    try:
        return auth.models.User.objects.get(username=username)
    except auth.models.User.DoesNotExist:
        return None

def login(request, next_page, template_name="accounts/login.html"):
    """Simple login form view which doesn't rely on Django's current
       inflexible oldforms-based auth view.
       """
    if request.POST:
        error = internal_login(request,
                               request.POST.get('username'),
                               request.POST.get('password'))
        if not error:
            return HttpResponseRedirect(next_page)
    else:
        error = None
    request.session.set_test_cookie()
    return render_to_response(template_name, RequestContext(request, {
        'error': error,
        'login_url': settings.LOGIN_URL,
        }))


###########################
#     Lost Password       #
###########################

def send_mail_to_user(user, template_name, **context_dict):
    """Send a single email message to a registered user. This formats their
       email address using their full name, and automatically treats the
       first non-blank line of the rendered template as a message subject.

       The template context will automatically include 'user'.
       """
    context = Context(context_dict)
    context['user'] = user
    subject, message = loader.render_to_string(template_name, context).lstrip().split("\n", 1)
    subject = subject.strip()
    message = message.strip()

    # Sanitize the user's name for inclusion in an RFC822 header
    full_name = re.sub("[!<>@:;\\\\'\"\[\]\r\n\t]", "", user.get_full_name().strip())
    send_mail(subject, message, None, ["%s <%s>" % (full_name, user.email)])

def lost(request, next_page, recovery_page):
    error = None
    if request.POST:
        user = get_user(request.POST.get('username'))
        if not user:
            error = "Incorrect username."
        if not error:
            # Password recovery works via a special kind of session
            # which is transmitted over e-mail, rather than via a
            # cookie. Create a session which includes the username
            # this request was generated for.

            key = Session.objects.get_new_session_key()
            expire_date = datetime.datetime.now() + datetime.timedelta(hours=12)
            Session.objects.save(key, {
                'account_recovery_session': True,
                'username': user.username,
                }, expire_date)
            
            send_mail_to_user(user, "accounts/recovery_mail.txt",
                              request = request,
                              recovery_path = recovery_page % key)
            return render_to_response('accounts/recovery_mail_sent.html')
    return render_to_response('accounts/recovery_form.html', RequestContext(request, {'error': error}))

def get_recovery_session(key):
    """Get an account recovery session, with extra checks to make sure
       it's the right type of session and it hasn't expired.
       """
    try:
        session = Session.objects.get(session_key=key)
    except Session.DoesNotExist:
        return None

    if session.expire_date < datetime.datetime.now():
        return None

    decoded = session.get_decoded()
    if not decoded.get('account_recovery_session'):
        return None
    return decoded

class ResetPasswordForm(forms.Form):
    password = forms.CharField(min_length=5, max_length=30)
    password2 = forms.CharField()

def reset(request, key, next_page):
    session = get_recovery_session(key)
    if not session:
        return render_to_response('accounts/recovery_key_error.html', RequestContext(request))
    user = get_user(session.get('username'))

    if request.POST:
        form = ResetPasswordForm(request.POST)
        form.full_clean()
        validate_password_confirmation(form)
        validate_test_cookie(form, request)

        if not form.errors:
            user.set_password(form.clean_data['password'])
            user.save()

            # Try to log in using the new password
            loginError = internal_login(request, user.username, form.clean_data['password'])
            if loginError:
                # This might happen if the account is deactivated.
                form.errors['submit'] = forms.util.ErrorList([loginError])
            else:
                # We're in successfully. Expire the recovery session.
                Session.objects.save(key, None, datetime.datetime.now())
                return HttpResponseRedirect(next_page)
    else:
        form = None

    request.session.set_test_cookie()
    return render_to_response('accounts/reset_password.html', RequestContext(request, {
        'form_path': request.path,
        'username': user.username,
        'form': form,
        }))


###########################
#    User Registration    #
###########################

class RegistrationForm(forms.Form):
    first_name = forms.CharField(max_length=30)
    last_name = forms.CharField(max_length=30)
    email = forms.EmailField()
    username = forms.RegexField(r"^[a-zA-Z0-9_\-\.]*$", max_length=30,
                                error_message='Only A-Z, 0-9, "_", "-", and "." allowed.')
    password = forms.CharField(min_length=5, max_length=30)
    password2 = forms.CharField()

def validate_old_password(form, user, field_name='password'):
    if not form.errors.get(field_name) and not user.check_password(form.data.get(field_name)):
        form.errors[field_name] = forms.util.ErrorList(["Incorrect password."])

def validate_password_confirmation(form, field_name='password'):
    field2_name = field_name + '2'
    if not form.errors.get(field2_name) and form.data.get(field_name) != form.data.get(field2_name):
        form.errors[field2_name] = forms.util.ErrorList(["Your passwords do not match."])

def validate_test_cookie(form, request):
    if not request.session.test_cookie_worked():
        form.errors['submit'] = forms.util.ErrorList(["Cookies must be enabled."])

def register(request, next_page, template_name="accounts/register.html"):
    if request.POST:
        form = RegistrationForm(request.POST)
        form.full_clean()
        validate_password_confirmation(form)
        validate_test_cookie(form, request)

        if not form.errors:
            # The username still might be taken, but let's test for that atomically
            # as a side-effect of trying to create the user.
            user = auth.models.User(first_name = form.clean_data['first_name'].title(),
                                    last_name = form.clean_data['last_name'].title(),
                                    email = form.clean_data['email'],
                                    username = form.clean_data['username'])
            user.set_password(form.clean_data['password'])
            try:
                user.save()
            except:
                # The actual error we get for a duplicate username is database-specific.
                # To make this portable, we'll do a second check to see if the username
                # was taken. This is also slightly racy, but much less so than checking
                # beforehand. Easier to ask forgiveness than permission.
                if get_user(form.clean_data['username']):
                    form.errors['username'] = forms.util.ErrorList(["Sorry, this username is taken."])
                else:
                    # Something else happened.. pass on the exception
                    raise

        if not form.errors:
            # Something's wrong internally if we can't log in to the
            # account we just created...
            assert not internal_login(request, form.clean_data['username'], form.clean_data['password'])
            return HttpResponseRedirect(next_page)

    else:
        form = None

    request.session.set_test_cookie()
    return render_to_response(template_name, RequestContext(request, {'form': form}))


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
    """Returns a context for adding any asset. This raises
       a 404 error if the asset_type is not valid. This includes
       all context variables required for navigation.
       """
    model = get_asset_by_type(asset_type)
    return RequestContext(request, {
        'asset_types': get_user_asset_types(request, asset_type),
        'asset_type': asset_type,
        'asset_type_name': model._meta.verbose_name,
        'user_assets': model.objects.all_for_user(request.user),
        'add': True,
        'form_path': request.path,
        })

def get_asset_edit_context(request, asset_type, asset_id):
    """Returns a context for editing any asset. This raises a 404
       error if the asset_type is not valid or if the asset_id does
       not refer to a UserAsset for the current user. The result
       includes all context variables required for navigation.
       """
    model = get_asset_by_type(asset_type)
    asset_id = int(asset_id)
    user_asset = model.objects.all_for_user(request.user).get(pk=asset_id)

    # Set as the default asset for this type
    request.session['default_' + asset_type] = asset_id

    return RequestContext(request, {
        'asset_types': get_user_asset_types(request, asset_type),
        'asset_type': asset_type,
        'asset_type_name': model._meta.verbose_name,
        'user_assets': model.objects.all_for_user(request.user),
        'asset_id': asset_id,
        'user_asset': user_asset,
        'form_path': request.path,
        })


###########################
#     Profile Editing     #
###########################

class ChangePasswordForm(forms.Form):
    old_password = forms.CharField()
    new_password = forms.CharField(min_length=5, max_length=30)
    new_password2 = forms.CharField()

def do_change_password(request):
    form = ChangePasswordForm(request.POST)
    form.full_clean()
    validate_password_confirmation(form, 'new_password')
    validate_old_password(form, request.user, 'old_password')
    if not form.errors:
        request.user.set_password(form.clean_data['new_password'])
        request.user.save()
        request.user.message_set.create(message="Your password was changed successfully.")
    return form

class ChangeProfileForm(forms.Form):
    first_name = forms.CharField(max_length=30)
    last_name = forms.CharField(max_length=30)
    email = forms.EmailField()

def do_change_profile(request):
    form = ChangeProfileForm(request.POST)
    form.full_clean()
    if not form.errors:
        for key, value in form.clean_data.items():
            setattr(request.user, key, value)
        request.user.save()
        request.user.message_set.create(message="Your profile was updated successfully.")
    return form

@login_required
def profile(request):
    if 'change-password' in request.POST:
        password_form = do_change_password(request)
    else:
        password_form = None

    if 'change-profile' in request.POST:
        profile_form = do_change_profile(request)
    else:
        profile_form = ChangeProfileForm(request.user)

    return render_to_response('accounts/profile.html', RequestContext(request, {
        'profile': True,
        'asset_types': get_user_asset_types(request),
        'password_form': password_form,
        'profile_form': profile_form,
        }))


###########################
#      Stats Assets       #
###########################

@login_required
def stats_asset(request, asset_type, asset_id):
    """Generic form for editing stats-based assets"""
    ctx = get_asset_edit_context(request, asset_type, asset_id)
    return render_to_response('accounts/stats_asset.html', ctx)

@login_required
def add_stats_asset(request, asset_type):
    """Generic form for adding stats-based assets"""
    ctx = get_asset_add_context(request, asset_type)
    return render_to_response('accounts/add_stats_asset.html', ctx)


###########################
#        IRC Bots         #
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
        if inst.clean_data:
            self.clean_data.update(inst.clean_data.items())

def validate_network(form, field_name='network'):
    try:
        return models.Network.objects.get(pk=int(form.data.get('network')))
    except (ValueError, models.Network.DoesNotExist):
        form.errors['network'] = forms.util.ErrorList(["Select a network."])

@login_required
def add_bot(request, asset_type):
    form = MultiForm(request.POST)

    if request.POST:
        form.validate(AddBotForm)
        if form.clean_data.get('network') == '_other':
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

            # Add the "#" in front of the channel, if necessary
            channel = form.clean_data['channel']
            if channel[0] not in '#&':
                channel = '#' + channel

            # Now look up a matching bot. We might have to create this too.
            bot = models.Bot.objects.get_or_create(
                network = network,
                location__iexact = channel,
                defaults = dict(location=channel),
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

    ctx = get_asset_add_context(request, asset_type)
    ctx.update({
        'networks': models.Network.objects.all(),
        'form': form,
        })
    return render_to_response('accounts/add_bot.html', ctx)

@login_required
def bot(request, asset_type, asset_id):
    ctx = get_asset_edit_context(request, asset_type, asset_id)
    return render_to_response('accounts/bot.html', ctx)


