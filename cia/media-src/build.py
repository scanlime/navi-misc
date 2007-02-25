#!/usr/bin/env python
#
# Script to rebuild media/css and media/js from media-src.
# This should be run from the root of the project.
#

from lib import *

os.chdir('media-src')
DEST = '../media/'

# All of our CSS files so far have single individual sources
for single_css in [
    'base', 'login', 'account',
    'tabtastic', 'doc', 'image-upload',
    'feedback', 'stats',
    ]:
    merge('%scss/%s.css' % (DEST, single_css), [
    'css/local/%s.css' % single_css,
    ], cssmin)

merge(DEST + 'js/asset-edit.js', [
    'js/tabtastic-1.0.4/addclasskillclass.js',
    'js/tabtastic-1.0.4/attachevent.js',
    'js/tabtastic-1.0.4/addcss.js',
    'js/tabtastic-1.0.4/tabtastic.js',
    'js/local/on-text-changed.js',
    'js/local/preserve-tab.js',
    'js/yui-0.12.2/build/yahoo/yahoo.js',
    'js/yui-0.12.2/build/connection/connection.js',
    'js/json-20070110/json.js',
    'js/local/change-history.js',
    'js/local/iframe.js',
    'js/local/autohide.js',
    ], jsmin)

# Requires asset-edit.js
merge(DEST + 'js/bot-edit.js', [
    'js/local/units.js',
    'js/local/bot-status.js',
    'js/local/validator.js',
    ], jsmin)

# XXX: For testing
merge(DEST + 'js/fidtool.js', [
    'js/yui-0.12.2/build/yahoo/yahoo.js',
    'js/yui-0.12.2/build/connection/connection.js',
    'js/json-20070110/json.js',
    'js/local/fidtool.js',
    ], str)
