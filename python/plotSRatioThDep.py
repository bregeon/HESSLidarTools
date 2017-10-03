#!/bin/env python

import ROOT

def Sratio(lbd, ratio):
    if lbd == 355:
        return ratio*ROOT.TMath.Power(355./532.,-1.3)
    elif lbd == 532:
        return  ratio*ROOT.TMath.Power(532./355.,-1.3)

if __name__ == '__main__':
    
    can=ROOT.TCanvas() 
    tg=ROOT.TGraph(50)
    tg.SetNameTitle('SratioCanvas','Sratio dependency for desertic aerosols (lambda**-1.3)')
    for i in range(1,101,2):
        val=Sratio(532, i)
        print val
        tg.SetPoint(i, float(i), val)
    tg.Draw('AP*')
    tg.GetXaxis().SetTitle("Sratio @ 355 nm")
    tg.GetYaxis().SetTitle("Sratio @ 532 nm")
    can.Update()
    
    
