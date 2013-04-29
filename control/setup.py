from setuptools import setup, find_packages
import sys, os

setup(name="motmot.odorcontrol",
	description="Odor delivery and video acquisition synchronization for FView",
	version="0.0.1",
	packages=find_packages(),
	author="Will Allen",
	author_email="we.allen@gmail.com",
	url="",
	entry_points = {
		"motmot.fview.plugins" : "control = motmot.control.control:OdorControl"
	},
)
