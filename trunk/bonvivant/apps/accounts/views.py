#from bonvivant.apps.ext.forms import DojoTextField, DojoPasswordField, CaptchaField
from bonvivant.apps.recipes.models import Recipe

from django import oldforms
from django import newforms as forms
from django.contrib.auth import authenticate, login, logout, LOGIN_URL, REDIRECT_FIELD_NAME
from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.core import validators
from django.http import HttpResponseRedirect
from django.shortcuts import get_object_or_404, render_to_response
from django.template import RequestContext

def user_exists(username):
    return User.objects.filter(username=username).count()

class RegistrationForm(forms.Form):
    username = forms.RegexField(r"^[a-zA-Z0-9_\-\.]*$", max_length=30, error_message='Only A-Z, 0-9, "_", "-", and "." allowed.')
    password1 = forms.CharField(min_length=5, max_length=30, widget=forms.PasswordInput)
    password2 = forms.CharField(widget=forms.PasswordInput)

    def clean_username(self):
        if user_exists(self.clean_data['username']):
            raise forms.ValidationError("Sorry, this username is taken.")
        return self.clean_data['username']

    def clean_password2(self):
        if 'password1' in self.clean_data:
            if self.clean_data['password1'] != self.clean_data['password2']:
                raise forms.ValidationError('Passwords must match')
        return self.clean_data['password2']

    def save(self):
        if not self.errors:
            d = dict((k, v.encode("utf8")) for k, v in self.clean_data.iteritems())
            try:
                # I hate web sites that require email addresses, so I'm not
                # going to.  The email address may be optionally filled in later
                # if a user wants to be able to retrieve their password.
                user = User.objects.create_user(d['username'],
                                                '',
                                                d['password1'])
            except:
                # We also check for duplicate users here, since it's possible
                # that two users could race for a name.
                if user_exists(username=d['username']):
                    self.errors['username'] = forms.util.ErrorList(["Sorry, this username is taken."])
                else:
                    raise
            return user

def register(request):
    redirect_to = request.REQUEST.get(REDIRECT_FIELD_NAME, '')

    if request.POST:
        form = RegistrationForm(request.POST)

        if not request.session.test_cookie_worked():
            form.errors['submit'] = forms.util.ErrorList(["Cookies must be enabled."])

        if form.is_valid():
            user = form.save()
            if user:
                from django.contrib.auth import login
                user = authenticate(username=form.clean_data['username'],
                                    password=form.clean_data['password1'])
                assert user
                login(request, user)
                request.session.delete_test_cookie()

                if not redirect_to or '://' in redirect_to or ' ' in redirect_to:
                    redirect_to = '/accounts/profile/'
                return HttpResponseRedirect(redirect_to)
    else:
        form = RegistrationForm()

    request.session.set_test_cookie()
    return render_to_response('registration/register2.html', RequestContext(request, {'form': form}))

@login_required
def edit(request):
    try:
        manipulator = User.ChangeManipulator(request.user.id)
    except User.DoesNotExist:
        raise Http404

    user = manipulator.original_object

    if request.method == 'POST':
        new_data = request.POST.copy()
        errors = manipulator.get_validation_errors(new_data)
        if not errors:
            manipulator.do_html2python(new_data)
            manipulator.save(new_data)

            return HttpResponseRedirect('/recipes/edit/%i' % recipe.id)
    else:
        errors = {}
        new_data = manipulator.flatten_data()

    form = oldforms.FormWrapper(manipulator, new_data, errors)
    return render_to_response('accounts/edit.html',
                              {'form' : form},
                              context_instance=RequestContext(request))

@login_required
def profile(request):
    box = request.user.recipe_box.all()
    authored = Recipe.objects.filter(author=request.user.id)
    return render_to_response('accounts/profile.html',
                              {'box'      : box,
                               'authored' : authored},
                              context_instance=RequestContext(request))
