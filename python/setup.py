#!/usr/bin/python

from distutils.core import setup
from distutils.extension import Extension

setup(name="FlyTracker",
	ext_modules=[
		Extension("BGSubtractor", ["background/bgsubtractor.cpp", "background/background.cpp"],
		libraries = ["boost_numpy", "boost_python", "opencv_gpu", "opencv_nonfree", "opencv_core"])
	])
