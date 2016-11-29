#!/usr/bin/env python
from distutils.core import setup, Extension

traveller_module = Extension('_ga_traveller',
                             sources=['DPoint.cpp', 'CityMap.cpp', 'traveller_wrap.cxx'],
                             extra_compile_args=['-std=c++11', '-fPIC', '-O2', '-lc', '-lstdc++', '-pthread'],
                             )

setup(name='ga_traveller',
      version='0.1',
      author='yaojian@ga',
      description="""TSP algorithms with Generic Algorithm and BreakDown Concept, used in World Map""",
      ext_modules=[traveller_module],
      py_modules=['ga_traveller'],
      )
