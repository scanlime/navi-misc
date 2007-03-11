from django.conf import settings
from django.contrib.sites.models import Site

def analytics(request):
    return {
        'GOOGLE_ANALYTICS_ACCOUNT': settings.GOOGLE_ANALYTICS_ACCOUNT,
        }


def site(request):
    return {
        'site': Site.objects.get_current(),
        }

