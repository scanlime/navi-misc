from django import newforms as forms
from django.conf import settings
from django.contrib import auth
from django.http import HttpResponseRedirect
from django.shortcuts import render_to_response
from django.template import RequestContext

from bonvivant.apps.recipes.models import Recipe

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

def get_user(username):
    try:
        return auth.models.User.objects.get(username=username)
    except auth.models.User.DoesNotExist:
        return None

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
        'error' : error,
        'login_url' : settings.LOGIN_URL,
        }))

class RegistrationForm(forms.Form):
    username = forms.RegexField(r"^[a-zA-Z0-9_\-\.]*$", max_length=30, error_message='Only A-Z, 0-9, "_", "-", and "." allowed.')
    password1 = forms.CharField(min_length=5, max_length=30, widget=forms.PasswordInput)
    password2 = forms.CharField(widget=forms.PasswordInput)
    email = forms.EmailField()

    def clean_password2(self):
        if 'password1' in self.clean_data:
            if self.clean_data['password1'] != self.clean_data['password2']:
                raise forms.ValidationError('Passwords must match')
        return self.clean_data['password2']

    def save(self):
        if not self.errors:
            d = dict((k, v.encode("utf8")) for k, v in self.clean_data.iteritems())
            try:
                user = auth.models.User.objects.create_user(d['username'],
                                                            d['email'],
                                                            d['password1'])
                return user
            except:
                # We check for duplicate users here instead of clean, since it's
                # possible that two users could race for a name.
                if get_user(username=d['username']):
                    self.errors['username'] = forms.util.ErrorList(["Sorry, this username is taken."])
                else:
                    raise

def register(request):
    redirect_to = request.REQUEST.get(auth.REDIRECT_FIELD_NAME, '')

    if request.POST:
        form = RegistrationForm(request.POST)

        if not request.session.test_cookie_worked():
            form.errors['submit'] = forms.util.ErrorList(["Cookies must be enabled."])

        if form.is_valid():
            user = form.save()
            if user:
                user = auth.authenticate(username=form.clean_data['username'],
                                         password=form.clean_data['password1'])
                assert user
                auth.login(request, user)
                request.session.delete_test_cookie()

                if not redirect_to or '://' in redirect_to or ' ' in redirect_to:
                    redirect_to = '/account/'
                return HttpResponseRedirect(redirect_to)
    else:
        form = RegistrationForm()

    request.session.set_test_cookie()
    return render_to_response('accounts/register.html', RequestContext(request, {'form': form}))

@login_required
def profile(request):
    box = request.user.recipe_box.all()
    authored = Recipe.objects.filter(author=request.user.id)
    return render_to_response('accounts/profile.html', RequestContext(request, {
        'box'      : box,
        'authored' : authored,
        }))
