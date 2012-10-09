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
    files.sort(key=lambda x: os.stat(x).st_mtime, reverse=True)
    if len(files) > 1:
        newest = files[1]
    else:
        # newest = files[0]
        return
    # print newest
    if random:
        newest = choice(files)
    
    os.chdir(oldDir)
    f = open("lastfile.txt", "r")
    last = f.read().rstrip()
    f.close()
    # print last
    
    if last == newest: return
    f = open("lastfile.txt", "w")
    f.write(dir+'/'+newest)
    f.close()
    
if __name__ == '__main__':
    while True:
        monitor(sys.argv[1])
        time.sleep(1)
