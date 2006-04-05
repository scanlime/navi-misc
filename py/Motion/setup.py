from distutils.core import setup, Extension
import os

# get compile info for glib from pkg-config
pkgconfig = os.popen ('pkg-config --cflags glib-2.0')
glib_cflags = pkgconfig.readline ().strip ().split (' ')
pkgconfig.close ()
pkgconfig = os.popen ('pkg-config --libs glib-2.0')
glib_libs = pkgconfig.readline ().strip ().split (' ')
pkgconfig.close ()

# build!
motion_c = Extension ('motion_c', sources=['motion.c'], extra_compile_args=glib_cflags + ['-g'], extra_link_args=glib_libs)

setup (name='Motion',
       version='0.0.1',
       maintainer='David Trowbridge',
       maintainer_email='trowbrds@gmail.com',
       license='GPL',
       description='Modules for dealing with motion-capture data',
       py_modules=['ASFReader', 'Bone', '__init__'],
       extra_path='Motion',
       ext_modules=[motion_c])
