#!/usr/bin/env python
# encoding: utf-8
"""
monitor.py

Created by Chao Zhang on 2012-10-03.
"""

import sys, os
from glob import glob


def monitor(dir):
    files = glob(dir+"/*.root")
    files.sort(key=lambda x: os.stat(x).st_mtime, reverse=True)
    if len(files) > 1:
        newest = files[1]
    else:
        newest = files[0]
    # print newest
    
    f = open("lastfile.txt", "r")
    last = f.read().rstrip()
    f.close()
    # print last
    
    if last == newest: return
    f = open("lastfile.txt", "w")
    f.write(newest)
    f.close()
    
if __name__ == '__main__':
    monitor(sys.argv[1])

