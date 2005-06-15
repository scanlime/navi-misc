from distutils.core import setup, Extension

load_amc = Extension('load_amc', sources=['load_amc.c', 'motion.c'])

setup (name='Motion', version='1.0', description='Modules for dealing with motion-capture data', ext_modules=[load_amc])
