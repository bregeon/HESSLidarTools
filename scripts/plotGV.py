#!/bin/env python
"""
Plot George atmosphere profiles against Johan's results
in particular to verify the overlap function

@author: J. Bregeon
"""

import ROOT
ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetStatBorderSize(1)

# salt(kl),ap355(kl),erap355(kl)/5.,0.,am355(kl),
# ap532(kl),erap532(kl)/5.,0.,am532(kl),ie3
content=open('../data/GV_run_67220_Sp2020_12k_newover.txt').readlines()

run=int(content[1].strip())

print(run)

gAP=ROOT.TGraph()
gAM=ROOT.TGraph()
k=0
for i in range(2, len(content)-2,2):
  data=content[i].split()
  data+=content[i+1].split()
  #print data
  alt=float(data[0])+1800.
  ap532=float(data[5])
  am532=float(data[8])
  #print alt,ap532,am532
  gAP.SetPoint(k,ap532+am532,alt)
  gAM.SetPoint(k,am532,alt)
  k+=1

gFake=ROOT.TGraph(2)
gFake.SetPoint(1,1e-6,1);
gFake.SetPoint(2,0.05e-3,13000);
gFake.SetNameTitle("GV532ApF","Comparison run "+str(run))
gFake.GetXaxis().SetTitle("Exctinction (m-1)")
gFake.GetYaxis().SetTitle("Altitude s.l. (m)")
gFake.GetYaxis().SetTitleOffset(1.7)
gFake.GetYaxis().SetTitleSize(0.03)
gFake.GetYaxis().SetLabelSize(0.025)
gFake.GetXaxis().SetTitleSize(0.03);
gFake.GetXaxis().SetLabelSize(0.025);

c=ROOT.TCanvas("CompareJBGV","CompareJBGV",30,50,650,850)
gFake.Draw("AP")

gAP.Draw("PS*")
gAP.SetNameTitle("GV532Ap","GV532Ap")
gAP.SetMarkerColor(ROOT.kBlue)
gAM.Draw("PSL")
gAM.SetLineColor(4)
gAM.SetLineStyle(2)
gAM.SetLineWidth(2)

# Johan
jbf=ROOT.TFile.Open("run_67220_Sp2020_12k_nb500.root")
jbAP=jbf.Get("Extinction_T_67220_532")
jbAP.SetMarkerColor(ROOT.kRed)
jbAP.Draw("PS")
jbModel=jbf.Get("ExtinctionModel_67220_532")
jbModel.Draw("SL")
jbAM=jbf.Get("Extinction_M_67220_532")
jbAM.SetLineColor(2)
jbAM.SetLineStyle(2)
jbAM.Draw("SL")

leg=ROOT.TLegend(0.3,0.6,0.9,0.9)
header="Run "
header+=str(run)
leg.SetHeader(header)
leg.AddEntry(gAP,"GV Extinction (ap532+am532)","P")
leg.AddEntry(gAM,"GV Rayleigh (am532)","L")
leg.AddEntry(jbAP,"JB Total Extinction","P")
leg.AddEntry(jbAM,"JB Rayleigh","L")
leg.AddEntry(jbModel,"JB Model (MODTRAN)","L")     
leg.SetTextSize(0.03)
leg.Draw("same")
