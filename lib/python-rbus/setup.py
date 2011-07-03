from distutils.core import setup, Extension

setup(name="rbus", version="0.1",
      ext_modules=[
         Extension("rbus", 
             sources=["rbusmodule.c"],
             libraries = ["rbus"],
         ),
         ])
