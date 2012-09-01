# -*- coding: utf-8 -*-
"""
Created on Sat Sep  1 11:41:26 2012

@author: hendrik
"""

import subprocess
from subprocess import Popen as Popen
from subprocess import STDOUT as STDOUT
from subprocess import PIPE as PIPE

def executeCommand(command):
    p = Popen(command, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    return p.stdout.read()

findIconNamesCmd = "grep fromTheme `find ../../ -name \"*cpp\"`"
iconNames = [i[i.find("\"",i.find("fromTheme"))+1:i.find("\"",i.find("\"",i.find("fromTheme"))+1)]+".png" for i in executeCommand(findIconNamesCmd).split("\n") ][:-1]

print "<!DOCTYPE RCC><RCC version=\"1.0\">\n<qresource prefix=\"icons/oxygen\">\n\t<file alias=\"index.theme\">icons/oxygen/index.theme</file>\n"

for iconName in iconNames:
    findIconCmd = "find /usr/share/icons/oxygen/16x16/ -name " + iconName
    fileName = executeCommand(findIconCmd)
    executeCommand("cp " + fileName[:-1] + " ./")
    print "\t<file alias=\"16x16/" + iconName + "\">icons/oxygen/" +\
    iconName + "</file>"

print "</qresource>\n</RCC>"