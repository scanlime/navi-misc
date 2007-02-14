#
# Django settings for the CIA project.
#
# Note that these defaults read all sensitive data (DB passwords, CIA key, Django secret)
# from dotfiles in the current user's home directory. This file should remain in version
# control, so all information within must be public.
#

import django.contrib.auth
import os

def rel_path(p):
    return os.path.join(os.path.abspath(os.path.split(__file__)[0]), p)

DEBUG = os.path.isfile(rel_path("DEBUG"))
TEMPLATE_DEBUG = DEBUG

#
# This can be used to temporarily restrict the creation of new user accounts.
# I'm using this to put the site through a testing period in which it isn't
# fully public, but it's coexisting with the rest of CIA.
#
CIA_REGISTRATION_IS_CLOSED = False

#
# Optional integration with Google Analytics.
# Set this either to None or to the _uacct value from Google.
#
# XXX: This value must be defined separately for the old site, in official.web.tac
#
GOOGLE_ANALYTICS_ACCOUNT = "UA-247340-1"

ADMINS = (
    ('Micah Dowty', 'micah@navi.cx'),
)

DEFAULT_FROM_EMAIL = 'webmaster@cia.navi.cx'

MANAGERS = ADMINS

#
# A little code to read CIA's database settings file.
# It's a list of key-value pairs, one per line, with
# key and value separated by '='.
#
CIA_DB_SETTINGS = {}
for line in open(os.path.expanduser('~/.cia_db')):
    line = line.strip()
    try:
        key, value = line.split('=', 1)
        CIA_DB_SETTINGS[key.strip()] = value.strip()
    except ValueError:
        pass

DATABASE_ENGINE = 'mysql'
DATABASE_NAME = CIA_DB_SETTINGS.get('db', 'cia')
DATABASE_USER = CIA_DB_SETTINGS.get('user', 'root')
DATABASE_PASSWORD = CIA_DB_SETTINGS.get('passwd', '')
DATABASE_HOST = CIA_DB_SETTINGS.get('host', '') 
DATABASE_PORT = CIA_DB_SETTINGS.get('port', '') 

#
# CIA data files
#
CIA_DOC_PATH = rel_path('doc')
CIA_DATA_PATH = rel_path('data') 

#
# These settings control our connection with CIA. We use its XML-RPC interface
# for most management tasks, but we also need to talk directly to the bot server
# occasionally. We also need to know where to find login information for the RPC
# server.
#
CIA_RPC_URL = 'http://localhost:3910'
CIA_KEY = open(os.path.expanduser('~/.cia_key')).read().strip()
CIA_BOT_SOCKET = rel_path('bots.socket')

# Local time zone for this installation. All choices can be found here:
# http://www.postgresql.org/docs/current/static/datetime-keywords.html#DATETIME-TIMEZONE-SET-TABLE
TIME_ZONE = 'PST'

# Language code for this installation. All choices can be found here:
# http://www.w3.org/TR/REC-html40/struct/dirlang.html#langcodes
# http://blogs.law.harvard.edu/tech/stories/storyReader$15
LANGUAGE_CODE = 'en-us'

SITE_ID = 1

# Absolute path to the directory that holds media.
# Example: "/home/media/media.lawrence.com/"
MEDIA_ROOT = rel_path('media')

# URL that handles the media served from MEDIA_ROOT.
# Example: "http://media.lawrence.com"
MEDIA_URL = '/media/'

LOGIN_URL = '/account/login/'

# URL prefix for admin media -- CSS, JavaScript and images. Make sure to use a
# trailing slash.
# Examples: "http://foo.com/media/", "/media/".
ADMIN_MEDIA_PREFIX = MEDIA_URL +'admin/'

# Make this unique, and don't share it with anybody.
SECRET_KEY = open(os.path.expanduser('~/.django_secret')).read().strip()

# List of callables that know how to import templates from various sources.
TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.load_template_source',
    'django.template.loaders.app_directories.load_template_source',
#     'django.template.loaders.eggs.load_template_source',
)

TEMPLATE_CONTEXT_PROCESSORS = (
    'django.core.context_processors.auth',
    'django.core.context_processors.debug',
    'django.core.context_processors.i18n',
    'cia.apps.context_processors.analytics',
#    'django.core.context_processors.request',
)

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.middleware.http.SetRemoteAddrFromForwardedFor',    # We run behind a Pound proxy
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.middleware.doc.XViewMiddleware',
)

ROOT_URLCONF = 'cia.apps.urls'

TEMPLATE_DIRS = (
    rel_path('templates'),
)

INSTALLED_APPS = (
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.sites',
#    'django.contrib.admin',
    'cia.apps.accounts',
    'cia.apps.stats',
    'cia.apps.images',
    'cia.apps.repos',
)
