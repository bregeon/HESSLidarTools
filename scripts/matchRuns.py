#!/bin/env python
"""
Script to find Lidar data analysis results for run corresponding to a given run list

@author: J. Bregeon
"""

from glob import glob

files=glob('all_Sp4070_15112016/analysis*.root')
allruns=[int(l.split('analysis_')[1][:6]) for l in files]
print('Nb Runs available: ', len(allruns))

runslist=open('../data/CrabList_loose_13032014.txt').readlines()[10:]
#runslist=open('../data/SgrAx_RunsList_loose.txt').readlines()[6:]
runs=[int(l.strip()) for l in runslist]
print('Crab runs in loose list: ',len(runs))

good=[]
for run in runs:
    if run in allruns:
      good.append(run)

print('found %s runs'%len(good))

print(good)

#[66784, 66785, 66786, 66803, 67094, 67095, 67113, 67114, 67161, 67162, 67189, 67190,
#  70323, 70980, 70999, 71000, 71021, 71089, 71090, 71091, 71093, 71097, 71098, 71109,
#  71111, 71112, 71113, 71114, 71861, 71862, 71863, 71864, 71866, 71887, 71888, 71889,
#  71890, 71892, 71893, 71897, 71898, 71899, 71949, 71960, 71992, 71993, 71994, 71995,
#  73991, 73993, 74795, 74799, 74861, 74944, 75042, 75069, 75182, 75671, 75836, 77284, 77287]

# Crab runs
# [54566, 54588, 54589, 54603, 54604, 54623, 54624, 54648, 54653, 54706, 54707, 54734
#, 54735, 54771, 54772, 54789, 54809, 78815, 78922, 79556, 79858, 79859, 79860, 79862
#, 79881, 79882, 79884, 79885, 80026, 80081, 80086, 80124, 80129, 80154, 80192]
