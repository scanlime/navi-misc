from django import newforms as forms
from django.contrib.auth.models import User
from django.shortcuts import render_to_response
from models import Character

def character_form(request):
    user = User.objects.get(username=request.META["USER"])
    character_form = forms.form_for_model(Character)

    if request.method == "POST":
        form = character_form(request.POST)
    else:
        form = forms.form_for_model(Character)
        context = {
                "form": form(),
                "user": "%s %s" % (user.first_name, user.last_name)
        }
        response = render_to_response("Character/character_form.html", context)

    response["Content-Type"] = "application/xhtml+xml"
    return response

# vim: ts=4:sw=4:et
