# Django settings for the CIA project.
#
# XXX: All database settings, DEBUG, and SECRET_KEY
#      need to be changed before deployment!
#

import django.contrib.auth
import os

DEBUG = True
TEMPLATE_DEBUG = DEBUG

ADMINS = (
    ('Micah Dowty', 'micah@navi.cx'),
)

DEFAULT_FROM_EMAIL = 'webmaster@cia.navi.cx'

MANAGERS = ADMINS

DATABASE_ENGINE = 'mysql'      # 'postgresql', 'mysql', 'sqlite3' or 'ado_mssql'.
DATABASE_NAME = 'ciadj'        # Or path to database file if using sqlite3.
DATABASE_USER = 'root'         # Not used with sqlite3.
DATABASE_PASSWORD = ''         # Not used with sqlite3.
DATABASE_HOST = ''             # Set to empty string for localhost. Not used with sqlite3.
DATABASE_PORT = ''             # Set to empty string for default. Not used with sqlite3.

#
# These settings control our connection with CIA. We use its XML-RPC interface
# for most management tasks, but we also need to talk directly to the bot server
# occasionally. We also need to know where to find login information for the RPC
# server.
#
CIA_RPC_URL = 'http://localhost:3910'
CIA_KEY = open(os.path.expanduser('~/.cia_key')).read()
CIA_BOT_SOCKET = os.path.join(os.path.abspath(os.path.split(__file__)[0]), 'bots.socket')

# Local time zone for this installation. All choices can be found here:
# http://www.postgresql.org/docs/current/static/datetime-keywords.html#DATETIME-TIMEZONE-SET-TABLE
TIME_ZONE = 'America/Pacific'

# Language code for this installation. All choices can be found here:
# http://www.w3.org/TR/REC-html40/struct/dirlang.html#langcodes
# http://blogs.law.harvard.edu/tech/stories/storyReader$15
LANGUAGE_CODE = 'en-us'

SITE_ID = 1

# Absolute path to the directory that holds media.
# Example: "/home/media/media.lawrence.com/"
MEDIA_ROOT = os.path.join(os.path.abspath(os.path.split(__file__)[0]),
                          'media')

# URL that handles the media served from MEDIA_ROOT.
# Example: "http://media.lawrence.com"
MEDIA_URL = '/media/'

LOGIN_URL = '/account/login/'

# URL prefix for admin media -- CSS, JavaScript and images. Make sure to use a
# trailing slash.
# Examples: "http://foo.com/media/", "/media/".
ADMIN_MEDIA_PREFIX = MEDIA_URL +'admin/'

# Make this unique, and don't share it with anybody.
SECRET_KEY = '9)%s0n5qdswni)wa6m6e-zg@au0eb-+f*b@j82dev8^oy=a22b'

# List of callables that know how to import templates from various sources.
TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.load_template_source',
    'django.template.loaders.app_directories.load_template_source',
#     'django.template.loaders.eggs.load_template_source',
)

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.middleware.doc.XViewMiddleware',
)

ROOT_URLCONF = 'cia.apps.urls'

TEMPLATE_DIRS = (
    os.path.join(os.path.abspath(os.path.split(__file__)[0]), 'templates'),
)

INSTALLED_APPS = (
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.sites',
    'django.contrib.admin',
    'cia.apps.accounts',
)
