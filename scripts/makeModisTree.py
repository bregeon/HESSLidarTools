#!/bin/env python
"""
Script to create a ModisTree with synthetic results from the Modis data analysis
Each entry corresponds to a Modis data taking, matching a HESS run date

@author: J. Bregeon - 2016
"""

import ROOT
import numpy
import glob
from copy import copy
import datetime
import sys, os
   
# Generates Run-Date dictionary
def genRunDateDict():
	content=open('../data/Run_Date_list.txt').readlines()
	run_date_dict=dict()
	for line in content[1:]:
		run=int(line.split()[0])
		strdate=line.split()[1]
		#2011-07-12_03:47:30
		#date=datetime.datetime.strptime(strdate,'%Y-%m-%d_%H:%M:%S') 
		date=ROOT.TDatime(strdate.replace('_',' '))
		run_date_dict[run]=date	
	return run_date_dict
    
# generate Run-Date dictionary
run_date_dict=genRunDateDict()

# from 2000 to 1970
date2000=datetime.datetime.strptime("2000-01-01 00:00:00","%Y-%m-%d %H:%M:%S")
date1970=datetime.datetime.strptime("1970-01-01 00:00:00","%Y-%m-%d %H:%M:%S")
delta=date2000-date1970
norm_time_offset=delta.total_seconds()

# input data - output from Modis package
# starting from line 1948, i.e. run 49753 from January 1st 2009
modis_data=open('../data/All_runs_Modis_data_08022017.txt').readlines()[1948:]

# output
outputRootFile='ModisMerit_tree.root'
outFile = ROOT.TFile(outputRootFile , 'RECREATE' )
tree = ROOT.TTree( 'ModisMerit', 'Modis data analysis NTuple' )

branchDict={}

# Branches
# Run Date dhours1 sep1 aetype1 cloudfrac1 dbod1 dbae1 dhours2 sep2 aetype2 cloudfrac2 dbod2 dbae2

branchesI=['RunNumber']
for b in branchesI:
	branchDict[b]=numpy.empty((1,),'int32')
	tree.Branch( b, branchDict[b], '%s/I'%b )
branchesUI=['DateTime']
for b in branchesUI:
	branchDict[b]=numpy.empty((1,),'uint32')
	tree.Branch( b, branchDict[b], '%s/i'%b )
branchesF=['Month', 'Hour', 'HoursNight', 'dHours1', 'dHours2',\
           'dSep1', 'dSep2', 'AEType1', 'AEType2', \
           'CloudFrac1','CloudFrac2', 'DBOD1', 'DBOD2',\
           'DBAE1', 'DBAE2']
for b in branchesF:
    branchDict[b]=numpy.empty((1,), 'float32')
    tree.Branch( b, branchDict[b], '%s/F'%b )

# fill tree
# S/N at 10km above site, S/Bkg at 10 km, Noise at 10km
# MedianNSB
counter=0
for line in modis_data:
  run=line.split()[0]
  print("\r{}".format(run)),
  sys.stdout.flush()

  branchDict['RunNumber'][0] = int(run)

  try:
	  branchDict['DateTime'][0] = run_date_dict[int(run)].Convert() - norm_time_offset
	  branchDict['Month'][0] = run_date_dict[int(run)].GetMonth()+run_date_dict[int(run)].GetDay()/30.
	  branchDict['Hour'][0] = run_date_dict[int(run)].GetHour()+run_date_dict[int(run)].GetMinute()/60.
	  HoursNight=run_date_dict[int(run)].GetHour()+run_date_dict[int(run)].GetMinute()/60.
	  if HoursNight<14:
		  HoursNight = HoursNight + 24
	  HoursNight = HoursNight - 14
	  branchDict['HoursNight'][0] = HoursNight
  except:
	  branchDict['DateTime'][0] = 0
	  branchDict['Month'][0] = 0
	  branchDict['Hour'][0] = 0.
	  branchDict['HoursNight'][0] = 0.

  # Modis data
  # Run Date dhours1 sep1 aetype1 cloudfrac1 dbod1 dbae1 dhours2 sep2 aetype2 cloudfrac2 dbod2 dbae2
  branchDict['dHours1'][0]=float(line.split()[3])
  branchDict['dSep1'][0]=float(line.split()[4])
  branchDict['AEType1'][0]=float(line.split()[5])
  branchDict['CloudFrac1'][0]=float(line.split()[6])
  branchDict['DBOD1'][0]=float(line.split()[7])
  branchDict['DBAE1'][0]=float(line.split()[8])
  
  branchDict['dHours2'][0]=float(line.split()[9])
  branchDict['dSep2'][0]=float(line.split()[10])
  branchDict['AEType2'][0]=float(line.split()[11])
  branchDict['CloudFrac2'][0]=float(line.split()[12])
  branchDict['DBOD2'][0]=float(line.split()[13])
  branchDict['DBAE2'][0]=float(line.split()[14])

  # fill tree
  tree.Fill()
  counter=counter+1

outFile.cd()
tree.Write()
outFile.Close()

print("%s runs found."%counter)

