from django.conf.urls.defaults import *
from cia.apps.blog import models

urlpatterns = patterns('cia.apps.blog.views',
   (r'^(?P<year>\d{4})/(?P<month>\d{2})/(?P<slug>[-\w]+)/$', 'detail'),
   (r'^(?P<year>\d{4})/(?P<month>\d{2})/$',                  'archive'),
   (r'^$',                                                   'archive'),
   (r'^feed/$',                                              'blog_feed'),
   (r'^comment-feed/$',                                      'comment_feed'),
   (r'^post-comment/$',                                      'post_comment'),
   (r'^new-post/$',                                          'detail'),
)
