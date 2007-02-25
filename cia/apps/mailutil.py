from django.template import loader
from django.core.mail import send_mail
from django.template.context import Context
import re

def get_email_for_user(user):
    """Sanitize the user's name for inclusion in an RFC822 header,
       and combine that with the user's registered e-mail address.
       """
    return get_email_with_name(user.get_full_name(), user.email)

def get_email_with_name(name, email):
    """Sanitize a full name and combine it with an email address,
       for inclusion in an RFC822 header.
       """
    name = re.sub("[!<>@:;\\\\'\"\[\]\r\n\t]", "", name.strip())
    return "%s <%s>" % (name, email)

def render_to_email(template_name, context):
    """Render a template, splitting the result into subject and message.
       The first non-blank line is taken as the message's subject.
       """
    subject, message = loader.render_to_string(template_name, context).lstrip().split("\n", 1)
    return subject.strip(), message.strip()

def send_mail_to_user(user, template_name, from_email=None, **context_dict):
    """Send a single email message to a registered user. This formats their
       email address using their full name, and automatically treats the
       first non-blank line of the rendered template as a message subject.

       The template context will automatically include 'user'.
       """
    context = Context(context_dict)
    context['user'] = user
    subject, message = render_to_email(template_name, context)
    send_mail(subject, message, from_email, [get_email_for_user(user)])
