#!/bin/env python
import ROOT
from math import exp,pow
ROOT.gROOT.SetStyle("Plain")

run=78815
f=ROOT.TFile.Open('run_78815_Filtered_200LogB_Sp2020_R12.root')

lnrpw=f.Get("LidarLnRawPWProfile_78815_355")
lnfpw=f.Get("LidarLnFilteredPWProfile_78815_355")
lnbpw=f.Get("LidarLnBinnedPWProfile_78815_355")

ry=lnrpw.GetX()
fy=lnfpw.GetX()
ht=lnfpw.GetY()

hres10=ROOT.TH1F("hres10","Filter correction (h>10 km)",100,0,2)
hres7=ROOT.TH1F("hres7","Filter correction (10>h>7 km)",100,0,2)
hres5=ROOT.TH1F("hres5","Filter correction (7>h>5 km)",100,0,2)
hres2=ROOT.TH1F("hres2","Filter correction (5>h>2 km)",100,0,2)

for i in range(0,lnrpw.GetN()):
    raw=exp(ry[i])
    filt=exp(fy[i])
    h=ht[i]
    val=filt/raw #pow((filt-raw)/raw, 2)
    print ht[i], ry[i], fy[i], raw, filt, val
    if h>10000:
        hres10.Fill(val)    
    elif h>7000:
        hres7.Fill(val)    
    elif h>5000:
        hres5.Fill(val)    
    elif h>2000:
        hres2.Fill(val)
            
can=ROOT.TCanvas('CanFilter','Filter', 30,50,650,1050)
can.Divide(2,2)
can.cd(1)
lnrpw.Draw("APL")
can.cd(2)
lnfpw.Draw("APL")
can.cd(3)
lnbpw.Draw("APL")
can.cd(4)
hres10.Draw()
hres10.GetXaxis().SetTitle("Filtered/Raw")
#ROOT.gPad.SetLogy(True)

can2=ROOT.TCanvas('CanFilterDetails','Filter Residuals', 30,50,650,1050)
can2.Divide(2,2)
can2.cd(1)
hres10.Draw()
hres10.GetXaxis().SetTitle("Filtered/Raw")
can2.cd(2)
hres7.Draw()
hres7.GetXaxis().SetTitle("Filtered/Raw")
can2.cd(3)
hres5.Draw()
hres5.GetXaxis().SetTitle("Filtered/Raw")
can2.cd(4)
hres2.Draw()
hres2.GetXaxis().SetTitle("Filtered/Raw")
