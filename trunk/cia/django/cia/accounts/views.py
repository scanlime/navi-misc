from django.shortcuts import render_to_response
from django.contrib import auth
from django.contrib.sessions.models import Session
from django.template.context import RequestContext, Context
from django.http import HttpResponseRedirect, Http404
from django.conf import settings
from django import newforms as forms
from django.template import loader
from django.core.mail import send_mail
from cia.accounts import models
import datetime, re


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
    return render_to_response(template_name, RequestContext(request, dict(
        error = error,
        login_url = settings.LOGIN_URL,
        )))


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
    full_name = re.sub("[!<>@:;\\\\'\"\[\]\r\n\t]", "", user.get_full_name())
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
            user.set_password(form.data['password'])
            user.save()

            # Try to log in using the new password
            loginError = internal_login(request, user.username, form.data['password'])
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
            user = auth.models.User(first_name = form.data['first_name'].title(),
                                    last_name = form.data['last_name'].title(),
                                    email = form.data['email'],
                                    username = form.data['username'])
            user.set_password(form.data['password'])
            try:
                user.save()
            except:
                # The actual error we get for a duplicate username is database-specific.
                # To make this portable, we'll do a second check to see if the username
                # was taken. This is also slightly racy, but much less so than checking
                # beforehand. Easier to ask forgiveness than permission.
                if get_user(form.data['username']):
                    form.errors['username'] = forms.util.ErrorList(["Sorry, this username is taken."])
                else:
                    # Something else happened.. pass on the exception
                    raise

        if not form.errors:
            # Something's wrong internally if we can't log in to the
            # account we just created...
            assert not internal_login(request, form.data['username'], form.data['password'])
            return HttpResponseRedirect(next_page)

    else:
        form = None

    request.session.delete_test_cookie()
    return render_to_response(template_name, RequestContext(request, {'form': form}))


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
        request.user.set_password(form.data['new_password'])
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
        for key, value in form.data.items():
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
