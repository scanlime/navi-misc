from django.conf import settings
from django.conf.urls.defaults import *
from bonvivant.apps.recipes.models import Recipe

urlpatterns = patterns('',
    # Built-in admin UI
    (r'^admin/', include('django.contrib.admin.urls')),

    # Recipes!
    (r'^recipes/$', 'django.views.generic.date_based.archive_index',
     {'queryset'      : Recipe.objects.all(),
      'date_field'    : 'ctime',
      'template_name' : 'recipes/archive.html'}),

    (r'^recipes/new/$',                     'bonvivant.apps.recipes.views.new'),
    (r'^recipes/edit/(?P<object_id>\d+)/$', 'bonvivant.apps.recipes.views.edit'),
    (r'^recipes/(?P<object_id>\d+)/$',      'bonvivant.apps.recipes.views.detail'),

    # Auth and stuff
    (r'^account/$',          'bonvivant.apps.accounts.views.profile'),
    (r'^account/login/$',    'djblets.auth.views.login', dict(next_page='/account/')),
    (r'^account/logout/$',   'django.contrib.auth.views.logout',    dict(template_name='accounts/logged_out.html')),
    (r'^account/register/$', 'djblets.auth.views.register'),
    #(r'^accounts/edit/$',     'bonvivant.apps.accounts.views.edit'),
    #(r'^accounts/edit2/$',    'bonvivant.apps.accounts.views.edit2'),
)

if settings.DEBUG:
    urlpatterns += patterns('',
        (r'^site_media/(?P<path>.*)$', 'django.views.static.serve',
         {'document_root' : '/home/david/projects/navi-misc/bonvivant/media',
          'show_indexes'  : True}),
    )
