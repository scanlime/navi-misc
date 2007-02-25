from django import newforms as forms
from django.shortcuts import render_to_response
from django.template.context import RequestContext
from django.core.mail import send_mail
from django.http import HttpResponseRedirect
from django.conf import settings
from cia.apps.mailutil import get_email_with_name
import re

class FeedbackForm(forms.Form):
    name = forms.CharField(required=False)
    email = forms.EmailField(required=False)
    comment = forms.CharField(required=True)
    referrer = forms.CharField()

def send_feedback_mail(form):
    if form.clean_data.get('email'):
        from_addr = get_email_with_name(form.clean_data.get('name'),
                                        form.clean_data.get('email'))
    else:
        from_addr = settings.SERVER_EMAIL

    subject = "%s User feedback: %s..." % (
        settings.EMAIL_SUBJECT_PREFIX,
        re.sub(r'\s+', ' ', form.clean_data['comment'])[:30].strip())

    message = ("Name: %(name)s\n"
               "E-mail: %(email)s\n"
               "Referrer: %(referrer)s\n"
               "\n%(comment)s" % form.clean_data)

    send_mail(subject, message, from_addr, [a[1] for a in settings.MANAGERS])

def feedback(request, referrer='/'):
    referrer = request.GET.get('ref', referrer)

    if request.POST:
        form = FeedbackForm(request.POST)
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
    
    return render_to_response('feedback.html', RequestContext(request, {
        'form': form,
        }))
