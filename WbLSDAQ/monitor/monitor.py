#!/usr/bin/env python
# encoding: utf-8
"""
monitor.py

Created by Chao Zhang on 2012-10-03.
"""

import sys, os

def monitor(dir):
    oldDir = os.getcwd()
    os.chdir(dir)
    files = [x for x in os.listdir(dir) if x.endswith('.root')]
    files.sort(key=lambda x: os.stat(x).st_mtime, reverse=True)
    if len(files) > 1:
        newest = files[1]
    else:
        newest = files[0]
    # print newest
    
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
    monitor(sys.argv[1])

