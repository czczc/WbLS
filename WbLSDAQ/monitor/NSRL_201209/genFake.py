#!/usr/bin/env python
# encoding: utf-8
"""
monitor.py

Created by Chao Zhang on 2012-10-03.
"""

import sys, os
import time

def fakeone(filename):
    command = 'root -b -q \'fakeDAQ.C+("'
    command += filename
    command += '")\''
    print command
    os.system(command)
    
if __name__ == '__main__':
    count = 1;
    while True:
        fakeone('~/Projects/LBNE/WbLS/software/WbLSDAQ/data/test/fakedata%d.root' % count)
        count += 1
        time.sleep(1)

