from distutils.core import setup, Extension

load_amc = Extension('motion_c', sources=['motion_c.c', 'motion.c'])

setup (name='Motion', version='1.0', description='Modules for dealing with motion-capture data', ext_modules=[load_amc])
