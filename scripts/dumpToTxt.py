#!/usr/bin/python
## \file dumpToTxt.py
## 
## Script for run list generation
##
##Created on Thu Nov  3 09:57:44 2016
##
## \author Johan Bregeon

import sys, os
import ROOT

fileName=sys.argv[1]

rfile=ROOT.TFile.Open(fileName)


