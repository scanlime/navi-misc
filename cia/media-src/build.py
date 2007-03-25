#!/usr/bin/env python
#
# Script to rebuild media/css and media/js from media-src.
# This should be run from the root of the project.
#

from lib import *

os.chdir('media-src')
DEST = '../media/'

jsmin = str

for single_css in [
    'login', 'account', 'tabtastic',
    'doc', 'image-upload', 'stats',
    ]:
    merge('%scss/%s.css' % (DEST, single_css), [
    'css/local/%s.css' % single_css,
    ], csspp)

merge(DEST + 'css/base.css', [
    'css/local/base.css',
    'css/local/search-results.css',
    ], csspp)

merge(DEST + 'css/old-site.css', [
    'css/local/old-site.css',
    'css/local/search-results.css',
    ], csspp)

merge(DEST + 'js/base.js', [
    'js/yui-0.12.2/build/yahoo/yahoo.js',
    'js/yui-0.12.2/build/dom/dom.js',
    'js/yui-0.12.2/build/connection/connection.js',
    'js/yui-0.12.2/build/event/event.js',
    'js/yui-0.12.2/build/container/container_core.js',
    'js/yui-0.12.2/build/animation/animation.js',
    'js/local/json.js',
    'js/local/html.js',
    'js/local/search.js'
    ], jsmin)

merge(DEST + 'js/asset-edit.js', [
    'js/tabtastic-1.0.4/attachevent.js',
    'js/tabtastic-1.0.4/addclasskillclass.js',
    'js/tabtastic-1.0.4/addcss.js',
    'js/tabtastic-1.0.4/tabtastic.js',
    'js/local/preserve-tab.js',
    'js/local/change-history.js',
    'js/local/iframe.js',
    'js/local/autohide.js',
    ], jsmin)

# Requires asset-edit.js
merge(DEST + 'js/bot-edit.js', [
    'js/local/on-text-changed.js',
    'js/local/units.js',
    'js/local/bot-status.js',
    'js/local/validator.js',
    ], jsmin)

merge(DEST + 'js/stats.js', [
    'js/tabtastic-1.0.4/addclasskillclass.js',
    'js/yui-0.12.2/build/event/event.js',
    'js/local/expander.js',
    ], jsmin)

merge(DEST + 'js/blog-admin.js', [
    'js/local/iframe.js',
    ], jsmin)

merge(DEST + 'js/login.js', [
    'js/local/form-state.js',
    ], jsmin)

# XXX: For testing
merge(DEST + 'js/fidtool.js', [
    'js/local/fidtool.js',
    ], str)
