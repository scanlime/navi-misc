from django import forms
from django.contrib.auth import authenticate, login, logout, LOGIN_URL, REDIRECT_FIELD_NAME
from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.core import validators
from django.http import HttpResponseRedirect
from django.shortcuts import get_object_or_404, render_to_response
from django.template import RequestContext

from bonvivant.recipes.models import Recipe

class RegistrationForm(forms.Manipulator):
    def __init__(self):
        self.fields = (
            forms.TextField(field_name='username',
                            length=30, maxlength=30,
                            is_required=True,
                            validator_list=[validators.isAlphaNumeric, self.isValidUsername]),

            forms.EmailField(field_name='email',
                             length=30, maxlength=70,
                             is_required=False),

            forms.PasswordField(field_name='password1',
                                length=30, maxlength=60,
                                is_required=True),

            forms.PasswordField(field_name='password2',
                                length=30, maxlength=60,
                                is_required=True,
                                validator_list=[validators.AlwaysMatchesOtherField('password1', 'Passwords must match.')]),
        )

    def isValidUsername(self, field_data, all_data):
        try:
            User.objects.get(username=field_data)
        except User.DoesNotExist:
            return;
        raise validators.ValidationError('The username "%s" is already taken.' % field_data)

    def save(self, new_data):
        u = User.objects.create_user(new_data['username'],
                                     '', # email address can be optionally filled in later
                                     new_data['password1'])
        u.save()
        return u

def register(request):
    if request.user.is_authenticated():
        return HttpResponseRedirect('/accounts/profile/')

    manipulator = RegistrationForm()
    redirect_to = request.REQUEST.get(REDIRECT_FIELD_NAME, '')

    if request.POST:
        new_data = request.POST.copy()
        errors = manipulator.get_validation_errors(new_data)

        if not errors:
            manipulator.do_html2python(new_data)
            new_user = manipulator.save(new_data)

            if not redirect_to or '://' in redirect_to or ' ' in redirect_to:
                redirect_to = '/accounts/profile/'
            from django.contrib.auth import login

            user = authenticate(username=new_data['username'], password=new_data['password1'])
            if user:
                login(request, user)
                request.session.delete_test_cookie()
                return HttpResponseRedirect(redirect_to)
            else:
                raise Exception("database integrity problem: newly created user can't authenticate")
    else:
        errors = {}
        new_data = {}
    request.session.set_test_cookie()
    form = forms.FormWrapper(manipulator, new_data, errors)
    return render_to_response('registration/register.html',
                              {'form': form},
                              context_instance=RequestContext(request))

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

    form = forms.FormWrapper(manipulator, new_data, errors)
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
