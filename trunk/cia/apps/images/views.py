from cia.apps.images import models
from django.http import Http404
from django.shortcuts import render_to_response
from django.template.context import RequestContext
from cStringIO import StringIO
import Image


def upload(request):

    if request.POST.get('remove'):
        # User requested that we remove the currently posted image
        image = None

    elif request.GET.get('image-id'):
        # Preload with a supplied image ID
        try:
            image = models.Source.objects.get(id=request.GET['image-id'])
        except models.Source.DoesNotExist:
            raise Http404

    elif request.FILES and request.user.is_authenticated():
        # Upload a new image
        image = models.Instance.objects.create_original(Image.open(
            StringIO(request.FILES['file']['content'])), request.user)

    else:
        # No image uploaded yet
        image = None

    return render_to_response('image_upload.html', RequestContext(request, {
        'image': image,
        }))
