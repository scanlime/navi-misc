from django.conf.urls.defaults import *
from cia.apps.doc import views

urlpatterns = patterns('',
    (r'^$', views.page, dict(path='')),
    (r'^(?P<path>[a-zA-Z0-9_\-]+(/[a-zA-Z0-9_\-]+)*)/$', views.page),
)
