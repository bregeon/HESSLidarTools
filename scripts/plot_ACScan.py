#!/bin/env python
"""
Plot the result of an Alignement Correction factor scan

@author: J. Bregeon
"""

import glob
import ROOT

ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetStatBorderSize(1)


scans=glob.glob('acScan/scan*.root')
scans.sort()

tf0=ROOT.TFile.Open(scans[0])
extM355=tf0.Get("ExtinctionModel_67220_1_355")
extM532=tf0.Get("ExtinctionModel_67220_1_532")  
ray355=tf0.Get("Extinction_M_67220_1_355")
ray532=tf0.Get("Extinction_M_67220_1_532")

c=ROOT.TCanvas("ACScan","ACScan", 30, 50, 850,850)
c.Divide(2)
c.cd(1)
fake0=ROOT.TGraph(2)
fake0.SetPoint(1,1e-6,1);
fake0.SetPoint(2,0.16e-3,13000);
fake0.Draw("AP")
fake0.SetTitle("Correction factor Scan : 532 nm")
fake0.GetXaxis().SetTitle("Extinction (m^{-1})");
fake0.GetXaxis().SetTitleSize(0.03);
fake0.GetXaxis().SetLabelSize(0.025);
fake0.GetYaxis().SetTitle("Altitude (m)");
fake0.GetYaxis().SetTitleOffset(1.7);
fake0.GetYaxis().SetTitleSize(0.03);
fake0.GetYaxis().SetLabelSize(0.025);

extM355.Draw("PSL")
ray355.Draw("PSL")
c.cd(2)
fake1=ROOT.TGraph(2)
fake1.SetPoint(1,1e-6,1);
fake1.SetPoint(2,0.08e-3,13000);
fake1.Draw("AP")
fake1.SetTitle("Correction factor Scan : 355 nm")
fake1.GetXaxis().SetTitle("Extinction (m^{-1})");
fake1.GetXaxis().SetTitleSize(0.03);
fake1.GetXaxis().SetLabelSize(0.025);
fake1.GetYaxis().SetTitle("Altitude (m)");
fake1.GetYaxis().SetTitleOffset(1.7);
fake1.GetYaxis().SetTitleSize(0.03);
fake1.GetYaxis().SetLabelSize(0.025);

extM532.Draw("PSL")
ray532.Draw("PSL")


for f in scans:
  print(f)
  tf=ROOT.TFile.Open(f)  
  try:
      extT355=tf.Get("Extinction_T_67220_1_355")
      extT532=tf.Get("Extinction_T_67220_1_532")  
      c.cd(1)
      extT355.Draw("SL")
      c.cd(2)
      extT532.Draw("SL")
  except:
      print("failed to open ",f)
