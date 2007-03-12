from django import newforms as forms
from django.shortcuts import render_to_response
from models import Character

def character_form(request):
    character_form = forms.form_for_model(Character)
    if request.method == "POST":
        form = character_form(request.POST)
    else:
        form = forms.form_for_model(Character)
        context = {"form": form()}
        response = render_to_response("Character/character_form.html", context)

    response["Content-Type"] = "application/xhtml+xml"
    return response

# vim: ts=4:sw=4:et
