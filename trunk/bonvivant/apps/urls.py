from django.conf import settings
from django.conf.urls.defaults import *
from bonvivant.apps.recipes.models import Recipe

urlpatterns = patterns('',
    (r'^admin/', include('django.contrib.admin.urls')),
    (r'^account/', include('bonvivant.apps.accounts.urls')),

    # Recipes!
    (r'^recipes/$', 'django.views.generic.date_based.archive_index',
     {'queryset'      : Recipe.objects.all(),
      'date_field'    : 'ctime',
      'template_name' : 'recipes/archive.html'}),

    (r'^recipes/new/$',                     'bonvivant.apps.recipes.views.new'),
    (r'^recipes/edit/(?P<object_id>\d+)/$', 'bonvivant.apps.recipes.views.edit'),
    (r'^recipes/(?P<object_id>\d+)/$',      'bonvivant.apps.recipes.views.detail'),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^site_media/(?P<path>.*)$', 'django.views.static.serve',
         {'document_root' : settings.MEDIA_ROOT,
          'show_indexes'  : True}),
    )
