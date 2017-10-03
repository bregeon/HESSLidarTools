#!/bin/env python

# Simple script to plot all background histograms
# and mean value for each run as a function of Run Number
#

import glob
import ROOT
from copy import copy

ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetStatBorderSize(1)

#all=glob.glob("all_Sp5070_21102014//analysis*.root")
all=glob.glob("all_Sp2020_08022016//analysis*.root")

mem355=[]
mem532=[]

# for bkg
g355=ROOT.TGraph(len(all))
g355.SetTitle("All runs")
h355=ROOT.TH1F("Bkg355","All runs Bkg 355 nm",10,-0.0015,-0.0005)
g532=ROOT.TGraph(len(all))
g532.SetTitle("All runs")
h532=ROOT.TH1F("Bkg532","All runs Bkg 532 nm",10,-0.002,-0.001)

# fill me
i=0
for f in all:
  rf=ROOT.TFile.Open(f)
  run=f.split('analysis_')[1].split('.')[0]
  mem355.append(copy(rf.Get("LidarBkgHist_"+run+"_355")))
  mem532.append(copy(rf.Get("LidarBkgHist_"+run+"_532")))
  g355.SetPoint(i, int(run), mem355[-1].GetMean())
  h355.Fill(mem355[-1].GetMean())
  g532.SetPoint(i, int(run), mem532[-1].GetMean())
  h532.Fill(mem532[-1].GetMean())
  rf.Close()
  i+=1

c=ROOT.TCanvas("AllBkgDist","All Bkg Distribution", 30,50,650,950)
c.Divide(1,3)
c.cd(1)
mem355[0].Draw()
mem355[0].GetXaxis().SetRangeUser(-0.002,-0.007)
mem355[0].GetYaxis().SetRangeUser(0,400)
for h in mem355[1:]:
  h.Draw("same")

c.cd(2)
mem532[0].Draw()
mem532[0].GetXaxis().SetRangeUser(-0.002,-0.007)
mem532[0].GetYaxis().SetRangeUser(0,400)
for h in mem532[1:]:
  h.Draw("same")
  
c.cd(3)
mem355[0].Draw()
for h in mem355[1:]:
  h.Draw("same")
for h in mem532:
  h.Draw("same")
c.cd()
c.Update()

cbkg355=ROOT.TCanvas("AllBkg355","All Bkg", 30,50,650,950)
cbkg355.Divide(1,2)
cbkg355.cd(1)
g355.Draw("AP*")
g355.GetXaxis().SetTitle("Run Number")
g355.GetYaxis().SetTitle("Raw Average Bkg (mV)")
cbkg355.cd(2)
h355.Draw()
h355.GetXaxis().SetTitle("Raw Average Bkg (mV)")
cbkg355.cd()
cbkg355.Update()

cbkg532=ROOT.TCanvas("AllBkg532","All Bkg", 30,50,650,950)
cbkg532.Divide(1,2)
cbkg532.cd(1)
g532.Draw("AP*")
g532.GetXaxis().SetTitle("Run Number")
g532.GetYaxis().SetTitle("Raw Average Bkg (mV)")
cbkg532.cd(2)
h532.Draw()
h532.GetXaxis().SetTitle("Raw Average Bkg (mV)")
cbkg532.cd()
cbkg532.Update()
