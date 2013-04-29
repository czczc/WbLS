#!/usr/bin/env python
# encoding: utf-8
"""
monitor.py

Created by Chao Zhang on 2012-10-03.
"""

import sys, os, time
from random import choice

def monitor(dir):
    random = False;
    # random = True;
    oldDir = os.getcwd()
    os.chdir(dir)
    files = [x for x in os.listdir(dir) if x.endswith('.root')]
    # files.sort(key=lambda x: os.stat(x).st_mtime, reverse=True)
    files.sort(key=lambda x: int(x.rstrip('.root').lstrip('rootoutputfile')), reverse=True)
    
    # print files; print
    if len(files) > 1:
        newest = dir+'/'+files[1]
    else:
        newest = ""
        # return
    # print newest
    if random:
        newest = dir+'/'+choice(files)
    
    os.chdir(oldDir)
    f = open("lastfile.txt", "r")
    last = f.read().rstrip()
    f.close()
    # print last
    
    if last == newest: return
    f = open("lastfile.txt", "w")
    f.write(newest)
    f.close()
    print newest
    
if __name__ == '__main__':
    if (len(sys.argv)<2):
        # dir = '/Volumes/wblsdaq/WBLS_DATA/FADC_DATA/2012-10-05_ALandTEFLONcosmicsCOINc_1600V'
        dir = '/Volumes/wblsdaq/WBLS_DATA/FADC_DATA/beamrun'
    else:
        dir = sys.argv[1]
    while True:
        monitor(dir)
        time.sleep(1.5)
