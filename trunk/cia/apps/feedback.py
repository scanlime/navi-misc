from django import newforms as forms
from django.shortcuts import render_to_response
from django.template.context import RequestContext
from django.core.mail import send_mail
from django.http import HttpResponseRedirect
from django.conf import settings
from cia.apps.mailutil import get_email_with_name
from cia.apps.token import TokenClass
import re

FeedbackToken = TokenClass("feedback_token")

class FeedbackForm(forms.Form):
    name = forms.CharField(required=False)
    email = forms.EmailField(required=False)
    comment = forms.CharField(required=True)
    referrer = forms.CharField()
    token = forms.CharField()

    def clean_token(self):
        if FeedbackToken.get(self.clean_data['token']):
            return self.clean_data['token']
        else:
            # We should only get here in the event of:
            #  1. A spam bot
            #  2. A human user that left the form open for a very long time
            #
            # In the event of (2), let's give them a second chance by
            # generating a new token. We're hoping spam bots aren't
            # smart enough yet to parse the new form.

            raise forms.ValidationError("Your session expired. Please try again.")

def send_feedback_mail(form):
    if form.clean_data.get('email'):
        from_addr = get_email_with_name(form.clean_data.get('name'),
                                        form.clean_data.get('email'))
    else:
        from_addr = settings.SERVER_EMAIL

    subject = "%s User feedback: %s..." % (
        settings.EMAIL_SUBJECT_PREFIX,
        re.sub(r'\s+', ' ', form.clean_data['comment'])[:30].strip())

    message = unicode("Name: %(name)s\n"
                      "E-mail: %(email)s\n"
                      "Referrer: %(referrer)s\n"
                      "\n%(comment)s" % form.clean_data).encode("utf-8")

    send_mail(subject, message, from_addr, [a[1] for a in settings.MANAGERS])

def feedback(request, referrer='/'):
    referrer = request.GET.get('ref', referrer)

    if request.POST:
        data = {}
        data.update(request.POST.items())
        form = FeedbackForm(data)
        if form.is_valid():
            send_feedback_mail(form)
            return HttpResponseRedirect(form.clean_data['referrer'])

    elif request.user.is_authenticated():
        form = FeedbackForm({
            'name': request.user.get_full_name(),
            'email': request.user.email,
            'referrer': referrer,
            })

    else:
        form = FeedbackForm({
            'referrer': referrer,
            })

    form.data['token'] = FeedbackToken.new()
    return render_to_response('feedback.html', RequestContext(request, {
        'form': form,
        }))
