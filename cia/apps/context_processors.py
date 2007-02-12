from django.conf import settings

def analytics(request):
    return {
        'GOOGLE_ANALYTICS_ACCOUNT': settings.GOOGLE_ANALYTICS_ACCOUNT,
        }
