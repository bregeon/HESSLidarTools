#!/bin/env python
"""
Script to create a pdf of all summary canvas
from the Lidar data analysis

@author: J. Bregeon
"""

import glob
import ROOT


all=glob.glob("all_Sp5070_25032016/analysis*.root")
all.sort()
for f in all:
  rf=ROOT.TFile.Open(f)
  run=f.split('/')[-1].split('_')[1]
  seq=f.split('/')[-1].split('_')[2].split('.')[0]

  summ=rf.Get("SummaryCanvas_%s_%s"%(int(run), int(seq)))
  summ.SaveAs('all_Sp5070_25032016/Summary_25032016_Sp5070_%s_%s.pdf'%(run,seq))
  rf.Close()
