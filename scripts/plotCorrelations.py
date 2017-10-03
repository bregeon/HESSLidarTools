#!/bin/env python
"""
Plot correlations between atmospheric parameters derived from
the Lidar data analysis and weather related quantities

@author: J. Bregeon
"""

import ROOT

ROOT.gStyle.SetPalette(1)

f=ROOT.TFile.Open('../data/HessLidarMeteo_all_runs_65159_70605_18022014.root')
tree=f.Get('LidarMeteo')

good355='Lidar_Opacity_355nm>0.3 && Lidar_Opacity_355nm<0.6'
good532='Lidar_Opacity_532nm>0.03 && Lidar_Opacity_532nm<0.1'
goods='%s && %s'%(good355, good532)

cG=ROOT.TCanvas('cLidarGlobal','Lidar Global view',30,50,950,1050)
cG.Divide(1,3)
cG.cd(1)
tree.Draw('Lidar_Opacity_355nm:RunNumber','','*')
txt355=ROOT.TText()
txt355.DrawTextNDC(0.63,0.5,"Cooling Issue")
cG.cd(2)
tree.Draw('Lidar_Opacity_532nm:RunNumber','','*')
txt532=ROOT.TText()
txt532.DrawTextNDC(0.63,0.5,"Cooling Issue")
cG.cd(3)
tree.Draw('Lidar_Opacity_355nm:Lidar_Opacity_532nm>>hWLCorr',goods,"colz")
txt532=ROOT.TText()
txt532.DrawTextNDC(0.2,0.2,"Corr. Factor = %0.2f"%ROOT.hWLCorr.GetCorrelationFactor())

c=ROOT.TCanvas('Lidar','Lidar',30,50,1250,1050)
c.Divide(2,4)
c.cd(1)
tree.Draw('Lidar_Opacity_355nm>>h355nm(100,-0.1,0.9)')
c.cd(2)
tree.Draw('Lidar_Opacity_532nm>>h532nm(70,-0.1,0.6)')
c.cd(3)
tree.Draw('Lidar_Opacity_355nm:RunNumber',"","colz")
c.cd(4)
tree.Draw('Lidar_Opacity_532nm:RunNumber',"","colz")
c.cd(5)
tree.Draw('Lidar_Opacity_355nm:RunNumber',good355,'colz')
c.cd(6)
tree.Draw('Lidar_Opacity_532nm:RunNumber',good532,'colz')
c.cd(7)
tree.Draw('Lidar_Opacity_355nm:ZenithEqMeanSystemRate',good355, 'colz')
c.cd(8)
tree.Draw('Lidar_Opacity_532nm:ZenithEqMeanSystemRate',good532,'colz')


ROOT.gStyle.SetOptFit(111111)

cTC=ROOT.TCanvas('LidarTC','LidarTC',30,50,1250,1050)
cTC.Divide(2,3)
cTC.cd(1)
hTC355h=ROOT.TH2F('hTC355h','Opacity 355 nm vs TC',100,0.5,1.1,100,0.3,0.6)
hTC355h.GetXaxis().SetTitle('TC')
hTC355h.GetYaxis().SetTitle('Lidar_Opacity_355nm')
hTC355h.GetYaxis().SetTitleOffset(1.5)
tree.Project('hTC355h','Lidar_Opacity_355nm:TC')
hTC355h.Draw("colz")
txt355=ROOT.TText()
txt355.DrawTextNDC(0.2,0.2,"Corr. Factor = %0.2f"%hTC355h.GetCorrelationFactor())
cTC.cd(2)
hTC532h=ROOT.TH2F('hTC532h','Opacity 532 nm vs TC',100,0.5,1.1,100,0.05,0.08)
hTC532h.GetXaxis().SetTitle('TC')
hTC532h.GetYaxis().SetTitle('Lidar_Opacity_532nm')
hTC532h.GetYaxis().SetTitleOffset(1.5)
tree.Project('hTC532h','Lidar_Opacity_532nm:TC')
hTC532h.Draw("colz")
txt532=ROOT.TText()
txt532.DrawTextNDC(0.2,0.2,"Corr. Factor = %0.2f"%hTC532h.GetCorrelationFactor())
cTC.cd(3)
hTC355h_pfx=hTC355h.ProfileX()
hTC355h_pfx.Draw()
hTC355h_pfx.Fit("pol1")
hTC355h_pfx.GetYaxis().SetTitle('Lidar_Opacity_355nm')
hTC355h_pfx.GetYaxis().SetTitleOffset(1.5)
cTC.cd(4)
hTC532h_pfx=hTC532h.ProfileX()
hTC532h_pfx.Draw()
hTC532h_pfx.Fit("pol1")
hTC532h_pfx.GetYaxis().SetTitle('Lidar_Opacity_532nm')
hTC532h_pfx.GetYaxis().SetTitleOffset(1.5)
cTC.cd(5)
hTC355h_pfx_hTC=hTC355h.ProfileX("hTC355h_pfx_hTC")
hTC355h_pfx_hTC.Draw()
hTC355h_pfx_hTC.Fit("pol1", "","",0.9,1.1)
hTC355h_pfx_hTC.GetYaxis().SetTitle('Lidar_Opacity_355nm')
hTC355h_pfx_hTC.GetYaxis().SetTitleOffset(1.5)
cTC.cd(6)
hTC532h_pfx_hTC=hTC532h.ProfileX("hTC532h_pfx_hTC")
hTC532h_pfx_hTC.Draw()
hTC532h_pfx_hTC.Fit("pol1", "","",0.9,1.1)
hTC532h_pfx_hTC.GetYaxis().SetTitle('Lidar_Opacity_532nm')
hTC532h_pfx_hTC.GetYaxis().SetTitleOffset(1.5)


# ROOT correlation factor
print "Correlation Factor 355 nm = %0.2f"%hTC355h.GetCorrelationFactor()
print "Correlation Factor 532 nm = %0.2f"%hTC532h.GetCorrelationFactor()
