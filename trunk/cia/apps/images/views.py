from cia.apps.images import models
from django.http import Http404
from django.conf import settings
from django.shortcuts import render_to_response
from django.template.context import RequestContext


def upload(request):
    error = False
    image = None

    if request.POST.get('remove'):
        # User requested that we remove the currently posted image
        pass

    elif request.GET.get('image-id'):
        # Preload with a supplied image ID
        try:
            image = models.ImageSource.objects.get(id=request.GET['image-id'])
        except models.ImageSource.DoesNotExist:
            raise Http404

    elif request.FILES and request.user.is_authenticated():
        # Upload a new image
        try:
            image = models.ImageInstance.objects.create_original(
                request.FILES['file'], request.user)
        except models.ImageException:
            error = True
            if settings.DEBUG:
                raise

    return render_to_response('image_upload.html', RequestContext(request, {
        'image': image,
        'error': error,
        }))
