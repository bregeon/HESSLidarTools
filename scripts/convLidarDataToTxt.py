#!/bin/env python
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 18 11:31:13 2016

Quite a stupid script to convert Lidara data ROOT files
to standard simple ASCII file.
Used to convert old ASCII data files that I tried (unsuccessfully) to convert
corretly to ROOT Sash format, to new ASCII file format.

@author: bregeon
"""

import glob

import ROOT

ROOT.gROOT.SetBatch(1)
all=glob.glob("/data/Hess/data/LidarData_2009_2010_ROOT/*.root")
all.sort()

for one in all:
    lidar= ROOT.LidarTools.LidarFile(one)
    lidar.Read()
    lidar.WriteToAscii()

