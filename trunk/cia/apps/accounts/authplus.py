from django import newforms as forms
from django.conf import settings
from django.contrib import auth
from django.contrib.auth.models import User
from django.http import HttpResponseRedirect
from django.shortcuts import render_to_response
from django.template import loader
from django.template.context import RequestContext, Context
from cia.apps.mailutil import send_mail_to_user
from cia.apps.token import TokenClass
import datetime, urllib


###########################
#       User Login        #
###########################

def login_url(next_page):
    if next_page:
        return settings.LOGIN_URL + "?next_page=" + urllib.quote(next_page)
    else:
        return settings.LOGIN_URL

def login_required(view_func):
    """Simplified version of auth.decorators.login_required,
       which works with our LOGIN_URL and removes the 'next'
       parameter which we don't need yet.
       """
    def _checklogin(request, *args, **kwargs):
        if request.user.is_authenticated() and request.user.is_active:

            # This is a big hammer for superusers: if you're logged
            # in as a superuser, you can add ?impersonate=username
            # to nearly any URL to morph your current session
            # into a session for that user. This is useful as an
            # abuse management tool.
            if request.user.is_superuser:
                impersonate = request.GET.get('impersonate')
                if impersonate:
                    user = User.objects.get(username=impersonate)
                    user.backend = "django.contrib.auth.backends.ModelBackend"
                    auth.login(request, user)

            return view_func(request, *args, **kwargs)
        else:
            return HttpResponseRedirect(login_url(request.path))
    _checklogin.__doc__ = view_func.__doc__
    _checklogin.__dict__ = view_func.__dict__
    return _checklogin


def internal_login(request, username, password):
    try:
        user = auth.authenticate(username = username.encode('ascii'),
                                 password = password)
    except UnicodeDecodeError:
        user = None
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
    next_page = request.GET.get('next_page', next_page)

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
        'login_url': login_url(next_page),
        }))


###########################
#     Lost Password       #
###########################

PasswordToken = TokenClass("account_recovery_session", expire_hours=12)

def lost(request, next_page, recovery_page):
    error = None
    if request.POST:
        user = get_user(request.POST.get('username'))
        if not user:
            error = "Incorrect username."
        if not error:
            send_mail_to_user(user, "accounts/recovery_mail.txt",
                              request = request,
                              recovery_path = recovery_page % PasswordToken.new({'username': user.username}))
            return render_to_response('accounts/recovery_mail_sent.html', RequestContext(request))
    return render_to_response('accounts/recovery_form.html', RequestContext(request, {'error': error}))

class ResetPasswordForm(forms.Form):
    password = forms.CharField(min_length=5, max_length=30)
    password2 = forms.CharField()

def reset(request, key, next_page):
    t = PasswordToken.get(key)
    if not t:
        return render_to_response('accounts/recovery_key_error.html', RequestContext(request))
    user = get_user(t.get('username'))

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
                PasswordToken.delete(key)
                return HttpResponseRedirect(next_page)
    else:
        form = None

    request.session.set_test_cookie()
    return render_to_response('accounts/reset_password.html', RequestContext(request, {
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
    if settings.CIA_REGISTRATION_IS_CLOSED:
        return render_to_response("accounts/registration_closed.html", RequestContext(request))

    if request.POST:
        form = RegistrationForm(request.POST, initial=request.GET)
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
        form = RegistrationForm(initial=request.GET)

    request.session.set_test_cookie()
    return render_to_response(template_name, RequestContext(request, {'form': form}))


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
