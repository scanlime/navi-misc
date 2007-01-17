#!/usr/bin/env python
#
# Script to rebuild media/css and media/js from media-src.
# This should be run from the root of the project.
#

from lib import *

os.chdir('media-src')
DEST = '../media/'

merge(DEST + 'css/base.css', [
    'css/local/base.css',
    ], cssmin)

merge(DEST + 'css/login.css', [
    'css/local/login.css',
    ], cssmin)

merge(DEST + 'css/account.css', [
    'css/local/account.css',
    ], cssmin)

merge(DEST + 'css/tabtastic.css', [
    'css/local/tabtastic.css',
    ], cssmin)

merge(DEST + 'js/asset-edit.js', [
    'js/tabtastic-1.0.4/addclasskillclass.js',
    'js/tabtastic-1.0.4/attachevent.js',
    'js/tabtastic-1.0.4/addcss.js',
    'js/tabtastic-1.0.4/tabtastic.js',
    'js/local/on-text-changed.js',
    'js/local/preserve-tab.js',
    ], jsmin)

# Requires asset-edit.js
merge(DEST + 'js/bot-edit.js', [
    'js/yui-0.12.2/build/yahoo/yahoo.js',
    'js/yui-0.12.2/build/connection/connection.js',
    'js/json-20070110/json.js',
    'js/local/units.js',
    'js/local/bot-status.js',
    'js/local/validator.js',
    ], jsmin)

