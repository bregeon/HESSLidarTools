#!/bin/env python
"""
Script to associate old lidar text files to a HESS DAQ run number

@author: J. Bregeon
"""

import sys
from glob import glob
from bisect import bisect
import datetime

content=open('../data/Run_Date_list.txt').readlines()
runs=[]
dates=[]
for line in content[1:]:
  runs.append(int(line.split()[0]))
  strdate=line.split()[1]
  #2011-07-12_03:47:30
  date=datetime.datetime.strptime(strdate,'%Y-%m-%d_%H:%M:%S') 
  dates.append(date)

for run,date in zip(runs,dates)[:10]:
  print run, date
print

#onetime=datetime.strptime("2011-07-19_18:00:30",'%Y-%m-%d_%H:%M:%S')
#print onetime, runs[bisect(dates, onetime)-1],\
#               runs[bisect(dates, onetime)],runs[bisect(dates, onetime)+1]

allruns=glob('/data/Hess/data/LidarData_2009_2010/resultsm*')
allruns.sort()

text=''
for filename in allruns:
    #print filename
    onetime = datetime.datetime.strptime(filename.split('resultsm_')[1],'%Y%m%d_%H%M')
    therun=runs[bisect(dates, onetime)]
    thedate=dates[bisect(dates, onetime)]
    deltat=thedate-onetime
    #print onetime, therun, thedate, deltat
    if deltat>datetime.timedelta(hours=3):
        therun=runs[bisect(dates, onetime)-1]
        thedate=dates[bisect(dates, onetime)-1]
        deltat=thedate-onetime
        if deltat>datetime.timedelta(hours=3):
            print('Too large deltat %s'%deltat)
            therun=0
    text+='%s %s\n'%(filename, therun)
open('list.txt','w').write(text)
