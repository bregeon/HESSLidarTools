#!/bin/env python

HEADER='RunNumber RunDate	Meteo_MeanTemperature	Meteo_MeanWindSpeed	Meteo_MeanWindDirection	Meteo_MeanRelativeHumidity	Meteo_MeanPressure	Lidar_Opacity_355nm	Lidar_Opacity_532nm	TC	MeanSystemRate	ZenithEqMeanSystemRate	ZenithEqMeanSystemRate/MeanSystemRate	Telescope	TriggerRate	CorrectedTriggerRate	MedianNSB	NSBRelativeVariation	NSBRelativeDispersion	CTRadiometer_MeanTemp	CTRadiometer_TempRelativeVariation	CTRadiometer_TempRelativeDispersion	MedianScaler	ScalerRelativeVariation	ScalerRelativeDispersion	MedianTriggerRate	TriggerRateVariation	TriggerRateDispersion	Tracking_ZenithMean'
import ROOT
import re
import numpy

inputTxtFile= '../data/Dump_Lidar_Meteo_runs_45000_115000_23022016.txt' 
outputRootFile='HessLidarMeteo_all_runs_45000_115000_23022016.root'

f = ROOT.TFile(outputRootFile , 'RECREATE' )
tree = ROOT.TTree( 'LidarMeteo', 'Lidar data analysis' )

branchDict={}

h='RunNumber'
branchDict[h]=numpy.empty((1,),'int32')
tree.Branch( h, branchDict[h], '%s/I'%h )

for h in HEADER.split()[2:]:
    h=h.replace('/','_')
    branchDict[h]=numpy.empty((1,), 'float32')
    tree.Branch( h, branchDict[h], '%s/F'%h )

for line in open(inputTxtFile).readlines()[1:]:
    t = filter( lambda x: x, re.split( '\s+', line ) )
    branchDict['RunNumber'][0] = int(t[0])
    k=2
    for h in HEADER.split()[2:]:
		h=h.replace('/','_')
		branchDict[h][0]=float(t[k])
		k+=1
    
    tree.Fill()

tree.Write()
f.Close()

#out=ROOT.TFile( 'HessLidarMeteo.root')
#newtree=out.Get("LidarMeteo")
#newtree.Scan("RunNumber:Meteo_MeanTemperature")
#out.Close()
