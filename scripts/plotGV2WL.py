#!/bin/env python
"""
Plot George atmosphere profiles against Johan's results
compare both wavelength

@author: J. Bregeon, Nov 2016
"""

import ROOT
ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetStatBorderSize(1)

gA355=ROOT.TGraph()
gA532=ROOT.TGraph()
run=80081

def readGVText():
    #Alt,a355,r355,a532,r532,vr2-355,vr2-532,od355,od532,Angstrom,oubli355,oublie532,RunNumber 
    content=open('../data/run_80081_GV_results_16112016.txt').readlines()
    
    k=0
    for i in range(1, len(content)-1):
      data=content[i].split()
      #print data
      alt=float(data[0])#+1800.
      a355=float(data[1])
      r355=float(data[2])
      a532=float(data[3])
      r532=float(data[4])
      od355=float(data[7])
      od532=float(data[8])
      angs=float(data[9])
      
      #print alt,ap532,am532
      gA355.SetPoint(k,a355+r355,alt)
      gA532.SetPoint(k,a532+r532,alt)
      k+=1      


readGVText()

# Fake histograms for axis
gFake355=ROOT.TGraph(2)
gFake355.SetPoint(1,1e-6,1);
gFake355.SetPoint(2,1e-4,13000);
gFake355.SetNameTitle("GV355ApF","Comparison run "+str(run)+" @ 355 nm")
gFake355.GetXaxis().SetTitle("Exctinction (m-1)")
gFake355.GetYaxis().SetTitle("Altitude s.l. (m)")
gFake355.GetYaxis().SetTitleOffset(1.7)
gFake355.GetYaxis().SetTitleSize(0.03)
gFake355.GetYaxis().SetLabelSize(0.025)
gFake355.GetXaxis().SetTitleSize(0.03);
gFake355.GetXaxis().SetLabelSize(0.025);

gFake532=ROOT.TGraph(2)
gFake532.SetPoint(1,1e-6,1);
gFake532.SetPoint(2,0.05e-3,13000);
gFake532.SetNameTitle("GV532ApF","Comparison run "+str(run)+" @ 532 nm")
gFake532.GetXaxis().SetTitle("Exctinction (m-1)")
gFake532.GetYaxis().SetTitle("Altitude s.l. (m)")
gFake532.GetYaxis().SetTitleOffset(1.7)
gFake532.GetYaxis().SetTitleSize(0.03)
gFake532.GetYaxis().SetLabelSize(0.025)
gFake532.GetXaxis().SetTitleSize(0.03);
gFake532.GetXaxis().SetLabelSize(0.025);

# Canvas for comparison
c=ROOT.TCanvas("CompareJBGV","CompareJBGV",30,50,850,850)
c.Divide(2)
c.cd(1)
gFake355.Draw("AP")
gA355.Draw("PS*")
gA355.SetNameTitle("GV355A","GV355A")
gA355.SetMarkerColor(ROOT.kBlue)
gA355.Draw("PSL")
c.cd(2)
gFake532.Draw("AP")
gA532.Draw("PS*")
gA532.SetNameTitle("GV532A","GV532A")
gA532.SetMarkerColor(ROOT.kBlue)
gA532.Draw("PSL")

# Johan
jbf=ROOT.TFile.Open("../data/run_80081_CompareGV_16112016.root")

# 355
c.cd(1)
jbA355=jbf.Get("Extinction_T_80081_1_355")
jbA355.SetMarkerColor(ROOT.kRed)
jbA355.Draw("PS")
jbModel355=jbf.Get("ExtinctionModel_80081_1_355")
jbModel355.Draw("SL")
jbRayleigh355=jbf.Get("Extinction_M_80081_1_355")
jbRayleigh355.Draw("SL")

leg355=ROOT.TLegend(0.5,0.6,0.9,0.9)
header="Run "
header+=str(run)
leg355.SetHeader(header)
leg355.AddEntry(gA355,"GV Extinction (a355)","P")
leg355.AddEntry(jbA355,"JB Total Extinction","P")
leg355.AddEntry(jbRayleigh355,"JB Rayleigh","L")
leg355.AddEntry(jbModel355,"JB Model (MODTRAN)","L")     
leg355.SetTextSize(0.03)
leg355.Draw("same")

# 532
c.cd(2)
jbA532=jbf.Get("Extinction_T_80081_1_532")
jbA532.SetMarkerColor(ROOT.kRed)
jbA532.Draw("PS")
jbModel532=jbf.Get("ExtinctionModel_80081_1_532")
jbModel532.Draw("SL")
jbRayleigh532=jbf.Get("Extinction_M_80081_1_532")
jbRayleigh532.Draw("SL")

leg532=ROOT.TLegend(0.5,0.6,0.9,0.9)
header="Run "
header+=str(run)
leg532.SetHeader(header)
leg532.AddEntry(gA532,"GV Extinction (a532)","P")
leg532.AddEntry(jbA532,"JB Total Extinction","P")
leg532.AddEntry(jbRayleigh532,"JB Rayleigh","L")
leg532.AddEntry(jbModel532,"JB Model (MODTRAN)","L")     
leg532.SetTextSize(0.03)
leg532.Draw("same")


