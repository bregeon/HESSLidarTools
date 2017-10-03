#!/bin/env python

"""
Script to merge Lidar data analysis results in a single ROOT file
with all profiles and histograms.

@author: J. bregeon
"""

import os,sys,glob

all=glob.glob(sys.argv[1]+'/*.root')
all.sort()

i=0
list=[]
for l in all:
  list.append(l)
  if len(list)==500:
    cmd='hadd ana_'+str(i)+'.root '
    for f in list:
      cmd+='%s '%f
    os.system(cmd)
    list=[]
    i=i+1
