from distutils.core import setup, Extension
import os

# get compile info for glib from pkg-config
pkgconfig = os.popen ('pkg-config --cflags glib-2.0')
glib_cflags = pkgconfig.readline ().strip ().split (' ')
pkgconfig.close ()
pkgconfig = os.popen ('pkg-config --libs glib-2.0')
glib_libs = pkgconfig.readline ().strip ().split (' ')
pkgconfig.close ()

algorithms = Extension ('algorithms_c', sources=['dijkstra.c', 'a_star.c', 'depth_limited.c', 'utilities.c', 'path_tree.c', 'algorithms.c'], extra_compile_args=glib_cflags + ['-g', '-std=c99'], extra_link_args=glib_libs)

setup (name='Graph',
       version='1.0.0',
       maintainer='David Trowbridge',
       maintainer_email='trowbrds@gmail.com',
       license='GPL',
       description='Package for using graphs in python',
       py_modules=['Observable', 'Data', 'Algorithms', '__init__'],
       extra_path='Graph',
       ext_modules=[algorithms])
