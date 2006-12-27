from django.conf import settings
from django.conf.urls.defaults import *
from bonvivant.recipes.models import Recipe

urlpatterns = patterns('',
    # Built-in admin UI
    (r'^admin/', include('django.contrib.admin.urls')),

    # Recipes!
    (r'^recipes/$', 'django.views.generic.date_based.archive_index',
     {'queryset'      : Recipe.objects.all(),
      'date_field'    : 'ctime',
      'template_name' : 'recipes/archive.html'}),

    (r'^recipes/new/$',                     'bonvivant.recipes.views.new2'),
    (r'^recipes/edit/(?P<object_id>\d+)/$', 'bonvivant.recipes.views.edit'),
    (r'^recipes/(?P<object_id>\d+)/$',      'bonvivant.recipes.views.detail'),

    # Auth stuff
    (r'^login/$',            'django.contrib.auth.views.login'),
    (r'^logout/$',           'django.contrib.auth.views.logout'),
    (r'^register/$',         'bonvivant.accounts.views.register'),

    # Account info
    (r'^accounts/profile/$', 'bonvivant.accounts.views.profile'),
    (r'^accounts/edit/$',    'bonvivant.accounts.views.edit'),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^site_media/(?P<path>.*)$', 'django.views.static.serve',
         {'document_root' : '/home/david/projects/navi-misc/bonvivant/media',
          'show_indexes'  : True}),
    )
