from django.conf.urls.defaults import *

urlpatterns = patterns('',
        (r'^$', 'django.contrib.auth.views.login',
            {'template_name': 'login.html'}),
        (r'^accounts/profile/$', 'character_vitae.views.profile'),
        (r'^character/$', include('character_vitae.Character.urls')),
        (r'^site_media/(?P<path>.*)$', 'django.views.static.serve',
            {'document_root': 'site_media'}),
        (r'^admin/', include('django.contrib.admin.urls')),
)

# vim: ts=4:sw=4:et
