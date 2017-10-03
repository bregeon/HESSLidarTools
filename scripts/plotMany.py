#!/bin/env python
"""
Script to plot more extinction profiles on one plot

@author: J. Bregeon - 2016
"""

import ROOT

ROOT.gStyle.SetCanvasBorderMode(0);
ROOT.gStyle.SetPadBorderMode(0);
ROOT.gStyle.SetPadColor(0);
ROOT.gStyle.SetCanvasColor(0);
ROOT.gStyle.SetTitleColor(1);
ROOT.gStyle.SetStatColor(0);
ROOT.gStyle.ToggleEventStatus();

import glob

all=glob.glob("htest*355*.root")

mem=[]

for f in all:
  of=ROOT.TFile.Open(f)
  mem.append(of.Get("LidarOpacity_355"))

c=ROOT.TCanvas()
i=1
mem[0].Draw("APL")
mem[0].GetXaxis().SetTitle("Extinction 1/km")
mem[0].GetYaxis().SetTitle("Altitude (km)")

for g in mem:
  g.SetMarkerColor(i)
  g.SetLineColor(i)
  g.Draw("PSL")
  i+=1

