#!/usr/bin/env python

from setuptools import setup

setup(name='pru_speak',
      version='0.0',
      description='BotSpeak interpreter for BeagleBone Black PRU-ICSS',
      install_requires=['ply'],
      author='Deepak Karki',
      author_email='deepak6k@gmail.com',
      url='https://github.com/deepakkarki/pruspeak',
      packages=['pru_speak']
     )
