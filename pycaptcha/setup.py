#!/usr/bin/env python
from distutils.core import setup
import os

# Recursively find all data files
dataList = []
def addFiles(results, dir="Captcha/data"):
    files = []
    for fileName in os.listdir(dir):
        if fileName.startswith(".") or fileName.endswith("~"):
	    continue
        filePath = os.path.join(dir, fileName)
        if os.path.isdir(filePath):
	    addFiles(results, filePath)
	else:
	    files.append(filePath)
    results.append((dir, files))
addFiles(dataList)

setup (name = "PyCAPTCHA",
       version = "0.2-pre",
       description = "A Python framework for CAPTCHA tests",
       maintainer = "Micah Dowty",
       maintainer_email = "micah@navi.cx",
       license = "LGPL",
       packages = [
           'Captcha',
           'Captcha.Visual',
       ],
       data_files = dataList,
       )

