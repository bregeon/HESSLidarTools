#!/bin/env python
import ROOT

ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetStatBorderSize(1)

from pRadioSondeReader import *
from pRadioSondePlotter import *


if __name__ == '__main__':

    out=ROOT.TFile("RadioSondeData_2009-2014.root","RECREATE")

    # ingest data
    reader=pRadioSondeReader()
    reader.readFile("../data/radiosonde_68110_2009_2014.fsl")
    
    # create plots
    plottersList=[]
    for sounding in reader.SoundingDict.values():
        p=pRadioSondePlotter(sounding)
        p.fillPTGraphs()
        plottersList.append(p)
        p.Pressure.Write()
        p.Temperature.Write()
    
    # Create a reference canvas
    Pressure=ROOT.TGraph(2)
    Pressure.SetNameTitle("SondesPressure", "Sondes Pressure 2009-2014")
    Pressure.SetMarkerStyle(ROOT.kDot)
    Pressure.SetMarkerSize(0.1)
    Pressure.SetPoint(0,1013,0)
    Pressure.SetPoint(1,0,35000)
    
    Temperature=ROOT.TGraph(2)
    Temperature.SetNameTitle("SondeTemperature", "Sondes Temperature 2009-2014")
    Temperature.SetMarkerStyle(ROOT.kDot)
    Temperature.SetPoint(0,320,0)
    Temperature.SetPoint(1,180,35000)
    
    MainCanvas = ROOT.TCanvas("AllSondesCanvas","All Sondes",30, 50,650,850)
    MainCanvas.Divide(1,2)
    MainCanvas.cd(1)
    Pressure.Draw("AP")
    Pressure.GetXaxis().SetTitle("Pressure (mbar)")
    Pressure.GetYaxis().SetTitle("Altitude (m)")
    MainCanvas.cd(2)
    Temperature.Draw("AP")
    Temperature.GetXaxis().SetTitle("Temperature (K)")
    Temperature.GetYaxis().SetTitle("Altitude (m)")
    MainCanvas.cd()
    
    
    #plot Pressures
    MainCanvas.cd(1)
    for p in plottersList:
        p.Pressure.Draw("PS")

    #plot Temperatures
    MainCanvas.cd(2)
    for p in plottersList:
        p.Temperature.Draw("PS")
    
    Pressure.Write()
    Temperature.Write()
    MainCanvas.Write()
    
    out.Write()
