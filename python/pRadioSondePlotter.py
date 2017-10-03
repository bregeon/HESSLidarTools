#!/bin/env python
import ROOT

ROOT.gStyle.SetCanvasColor(0);
ROOT.gStyle.SetStatBorderSize(1)

from pRadioSondeReader import *

class pRadioSondePlotter(object):

    def __init__(self, sounding):
        self.Data=sounding
    
    def fillPTGraphs(self):    
        N=self.Data.NPoints
        self.Pressure=ROOT.TGraph(N)
        self.Temperature=ROOT.TGraph(N)
        for i in range(N):
            self.Pressure.SetPoint(i, self.Data.Pressure[i], self.Data.Height[i])
            self.Temperature.SetPoint(i, self.Data.Temperature[i], self.Data.Height[i])

        self.Pressure.SetNameTitle("SondePressure_"+self.Data.getKey(), "Sonde Pressure")
        self.Pressure.GetXaxis().SetTitle("Pressure (mbar)")
        self.Pressure.GetYaxis().SetTitle("Altitude (m)")
        self.Pressure.SetMarkerStyle(ROOT.kOpenCircle)
        self.Pressure.SetMarkerSize(0.4)

        self.Temperature.SetNameTitle("SondeTemperature_"+self.Data.getKey(), "Sonde Temperature")
        self.Temperature.GetXaxis().SetTitle("Temperature (K)")
        self.Temperature.GetYaxis().SetTitle("Altitude (m)")
        self.Temperature.SetMarkerStyle(ROOT.kOpenSquare)
        self.Temperature.SetMarkerSize(0.4)
    
    def show(self):
        self.MainCanvas = ROOT.TCanvas("SondeCanvas","SondeCanvas",30, 50,650,850)
        self.MainCanvas.Divide(1,2)
        self.MainCanvas.cd(1)
        self.Pressure.Draw("AP")
        self.MainCanvas.cd(2)
        self.Temperature.Draw("AP")
        self.MainCanvas.cd()



if __name__ == '__main__':
    s=pRadioSondeReader()
    s.readFile("../data/radiosonde_68110_2009_2014.fsl")
    sounding=s.getSounding('2009-08-23')
    sounding.dump()
    
    p=pRadioSondePlotter(sounding)
    p.fillPTGraphs()
    p.show()
    
    
