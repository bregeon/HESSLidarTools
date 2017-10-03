#!/bin/env python
"""
Script to create a LidarTree with synthetic results from the Lidar data analysis
Each entry corresponds to a RunNumber/SequenceNumber pair

@author: J. Bregeon - 2016
"""

HEADER='RunNumber RunDate	Meteo_MeanTemperature	Meteo_MeanWindSpeed	Meteo_MeanWindDirection	Meteo_MeanRelativeHumidity	Meteo_MeanPressure	Lidar_Opacity_355nm	Lidar_Opacity_532nm	TC	MeanSystemRate	ZenithEqMeanSystemRate	ZenithEqMeanSystemRate/MeanSystemRate	Telescope	TriggerRate	CorrectedTriggerRate	MedianNSB	NSBRelativeVariation	NSBRelativeDispersion	CTRadiometer_MeanTemp	CTRadiometer_TempRelativeVariation	CTRadiometer_TempRelativeDispersion	MedianScaler	ScalerRelativeVariation	ScalerRelativeDispersion	MedianTriggerRate	TriggerRateVariation	TriggerRateDispersion	Tracking_ZenithMean'
import ROOT
import numpy
import glob
from copy import copy
import datetime
import sys, os

Config_Dict={'QualityThr': 0, 'BkgFFactor': 1, 'SNRatioThreshold':2,\
             'OptimizeAC_Hmin':3, 'Fernald_sratio':4, 'R0_355':5, 'R0_532':6,\
            'Sp_355':7, 'Sp_532':8, 'AC_355':9, 'AC_532':10\
            }

def getName(txt, run, seq, wl):
    return '%s_%s_%s_%s'%(txt, int(run), int(seq), wl)
    
# Generates Run-Date dictionary
def genRunDateDict():
	content=open('../data/Run_Date_list.txt').readlines()
	run_date_dict=dict()
	for line in content[1:]:
		run=int(line.split()[0])
		strdate=line.split()[1]
		#2011-07-12_03:47:30
		#date=datetime.datetime.strptime(strdate,'%Y-%m-%d_%H:%M:%S') 
		date=ROOT.TDatime(strdate.replace('_',' '))
		run_date_dict[run]=date	
	return run_date_dict

# get background drift in % from 20 to 25 km
def getBkgDrift(bkgProf, bkgHist):
    pol1=ROOT.TF1("mypol1","pol1",20000,25000)    
    bkgProf.Fit("mypol1", "Q")    
    drift = -1    
    try:
        drift=ROOT.TMath.Abs(pol1.GetParameter(1)*5000/bkgHist.GetMean()*100)
    except:
        pass
    #print("Bkg drift is %02d pc"%drift)    
    return drift
    
# generate Run-Date dictionary
run_date_dict=genRunDateDict()

# from 2000 to 1970
date2000=datetime.datetime.strptime("2000-01-01 00:00:00","%Y-%m-%d %H:%M:%S")
date1970=datetime.datetime.strptime("1970-01-01 00:00:00","%Y-%m-%d %H:%M:%S")
delta=date2000-date1970
norm_time_offset=delta.total_seconds()

# input data
all_runs=glob.glob("all_Sp5050_13022017//analysis*.root")
all_runs.sort()

# Meteo and conditions ROOT file
#meteoFN="/data/Hess/work/pro/LidarTools/data/HessLidarMeteo_all_runs_65159_70605_18022014.root"
#meteoF=ROOT.TFile.Open(meteoF)
#meteoT.Get("LidarMeteo")

# output
#outputRootFile='HessLidar_Merit_all_16032016.root'
outputRootFile='LidarMerit_tree.root'
outFile = ROOT.TFile(outputRootFile , 'RECREATE' )
tree = ROOT.TTree( 'LidarMerit', 'Lidar data analysis NTuple' )

branchDict={}

# Branches
branchesI=['RunNumber', 'SeqNumber', 'Index']
for b in branchesI:
	branchDict[b]=numpy.empty((1,),'int32')
	tree.Branch( b, branchDict[b], '%s/I'%b )
branchesUI=['DateTime']
for b in branchesUI:
	branchDict[b]=numpy.empty((1,),'uint32')
	tree.Branch( b, branchDict[b], '%s/i'%b )
branchesF=['Month', 'Hour', 'HoursNight', 'QualityThr', 'BkgFFactor',\
           'SNRatioThr', 'AC_Hmin', 'Fernald_Sratio']
for b in branchesF:
    branchDict[b]=numpy.empty((1,), 'float32')
    tree.Branch( b, branchDict[b], '%s/F'%b )
# per wavelenght branches
branchesF_wl=['RawSignal_R0', 'BinPow_R0', 'BinPowDev_R0', 'LnBinPow_R0', \
              'RawSignal_10', 'BinPow_10', 'BinPowDev_10', 'LnBinPow_10', \
              'BkgMean', 'BkgRMS', 'BkgDrift', 'R0', 'ExtM_R0',\
              'Fernald_Sp', 'AC',\
              'Ext_T_8', 'Ext_T_6', 'Ext_T_4',\
              'TransProb_8', 'TransProb_6', 'TransProb_4',\
              'OD_Water', 'OD', 'AOD', "OD_Model", "AOD_Model"]
for b in branchesF_wl:    
    for wl in ['355', '532']:
        name=b+'_%s'%wl
        branchDict[name]=numpy.empty((1,), 'float32')
        tree.Branch( name, branchDict[name], '%s/F'%name )

# fill tree
# S/N at 10km above site, S/Bkg at 10 km, Noise at 10km
# MedianNSB
counter=0
for f in all_runs:
  rf=ROOT.TFile.Open(f)
  bname=os.path.basename(f)  
  run=bname.split('_')[1]
  seq=bname.split('_')[2].split('.')[0]
  print("\r{}".format(run)),
  sys.stdout.flush()

  branchDict['RunNumber'][0] = int(run)
  branchDict['SeqNumber'][0] = int(seq)
  branchDict['Index'][0] = int(run)*100+int(seq)
  
  # Config
  Config_Dict={'QualityThr': 0, 'BkgFFactor': 1, 'SNRatioThr':2,\
             'AC_Hmin':3, 'Fernald_Sratio':4, 'R0_355':5, 'R0_532':6,\
            'Fernald_Sp_355':7, 'Fernald_Sp_532':8, 'AC_355':9, 'AC_532':10\
            }
  gConfig=copy(rf.Get("ConfigTGraph_%s_%s"%(int(run),int(seq))))
  branchDict['QualityThr'][0] = gConfig.GetY()[Config_Dict['QualityThr']]
  branchDict['BkgFFactor'][0] = gConfig.GetY()[Config_Dict['BkgFFactor']]
  branchDict['SNRatioThr'][0] = gConfig.GetY()[Config_Dict['SNRatioThr']]
  branchDict['AC_Hmin'][0] = gConfig.GetY()[Config_Dict['AC_Hmin']]
  branchDict['Fernald_Sratio'][0] = gConfig.GetY()[Config_Dict['Fernald_Sratio']]
  branchDict['Fernald_Sp_355'][0] = gConfig.GetY()[Config_Dict['Fernald_Sp_355']]
  branchDict['Fernald_Sp_532'][0] = gConfig.GetY()[Config_Dict['Fernald_Sp_532']]
  branchDict['R0_355'][0] = gConfig.GetY()[Config_Dict['R0_355']]
  branchDict['R0_532'][0] = gConfig.GetY()[Config_Dict['R0_532']]
  branchDict['AC_355'][0] = gConfig.GetY()[Config_Dict['AC_355']]
  branchDict['AC_532'][0] = gConfig.GetY()[Config_Dict['AC_532']]
  
  try:
	  branchDict['DateTime'][0] = run_date_dict[int(run)].Convert() - norm_time_offset
	  branchDict['Month'][0] = run_date_dict[int(run)].GetMonth()+run_date_dict[int(run)].GetDay()/30.
	  branchDict['Hour'][0] = run_date_dict[int(run)].GetHour()+run_date_dict[int(run)].GetMinute()/60.
	  HoursNight=run_date_dict[int(run)].GetHour()+run_date_dict[int(run)].GetMinute()/60.
	  if HoursNight<14:
		  HoursNight = HoursNight + 24
	  HoursNight = HoursNight - 14
	  branchDict['HoursNight'][0] = HoursNight
  except:
	  branchDict['DateTime'][0] = 0
	  branchDict['Month'][0] = 0
	  branchDict['Hour'][0] = 0.
	  branchDict['HoursNight'][0] = 0.

  # Bkg value	  
  h355=copy(rf.Get( getName("LidarBkgHist", run, seq, "355") ))
  h532=copy(rf.Get( getName("LidarBkgHist", run, seq, "532") ))
  branchDict['BkgMean_355'][0] = h355.GetMean()
  branchDict['BkgRMS_355'][0] = h355.GetRMS()
  branchDict['BkgMean_532'][0] = h532.GetMean()
  branchDict['BkgRMS_532'][0] = h532.GetRMS()  
  # Bkg drift
  gBkgProf355=copy(rf.Get( getName("LidarBkgProfile", run, seq, "355") ))  
  gBkgProf532=copy(rf.Get( getName("LidarBkgProfile", run, seq, "532") ))  
  branchDict['BkgDrift_355'][0] = getBkgDrift(gBkgProf355, h355)
  branchDict['BkgDrift_532'][0] = getBkgDrift(gBkgProf532, h532)  
  
  
  # Extinction
  branchDict['Ext_T_8_355'][0] = 0
  branchDict['Ext_T_8_532'][0] = 0
  branchDict['Ext_T_6_355'][0] = 0
  branchDict['Ext_T_6_532'][0] = 0
  branchDict['Ext_T_4_532'][0] = 0
  branchDict['Ext_T_4_355'][0] = 0 
  # Get Altitude Binning Index
  hE355=copy(rf.Get( getName("Extinction_T", run, seq, "355")))
  hE532=copy(rf.Get( getName("Extinction_T", run, seq, "532")))  
  n_bin=hE532.GetN()
  alt_bin=hE532.GetY()   # get rw buffer  
  alt_bin.SetSize(n_bin) # set size
  altbin_np=numpy.array(alt_bin, copy=True) # convert to numpy array
  hi8=n_bin-sum(altbin_np>8000)-1 
  hi6=n_bin-sum(altbin_np>6000)-1 
  hi4=n_bin-sum(altbin_np>4000)-1

  # Extinction
  branchDict['Ext_T_8_355'][0] = hE355.GetX()[hi8]
  branchDict['Ext_T_8_532'][0] = hE532.GetX()[hi8]
  branchDict['Ext_T_6_355'][0] = hE355.GetX()[hi6]
  branchDict['Ext_T_6_532'][0] = hE532.GetX()[hi6]
  branchDict['Ext_T_4_532'][0] = hE532.GetX()[hi4]
  branchDict['Ext_T_4_355'][0] = hE355.GetX()[hi4]

  # Transmission
  branchDict['TransProb_8_355'][0] = rf.Get(getName("Transmission", run, seq, "355")).Eval(8000)
  branchDict['TransProb_8_532'][0] = rf.Get(getName("Transmission", run, seq, "532")).Eval(8000)
  branchDict['TransProb_6_355'][0] = rf.Get(getName("Transmission", run, seq, "355")).Eval(6000)
  branchDict['TransProb_6_532'][0] = rf.Get(getName("Transmission", run, seq, "532")).Eval(6000)
  branchDict['TransProb_4_355'][0] = rf.Get(getName("Transmission", run, seq, "355")).Eval(4000)
  branchDict['TransProb_4_532'][0] = rf.Get(getName("Transmission", run, seq, "532")).Eval(4000)

  # Values at R0 and 10 km: raw signal, binpower, binpowerdev, lnbinpower
  # Get Altitude Binning Index
  R0_355=branchDict['R0_355'][0]
  R0_532=branchDict['R0_532'][0]
  raw=rf.Get(getName("RawProfile", run, seq, "355"))
  n_raw=raw.GetN()   # get size
  alt=raw.GetY()       # get rw buffer  
  alt.SetSize(n_raw) # set size
  alt_np=numpy.array(alt, copy=True) # convert to numpy array
  iR0_355=n_raw-sum(alt_np>R0_355) -1 
  iR0_532=n_raw-sum(alt_np>R0_532) -1 
  i10=n_raw-sum(alt_np>10000) -1
  
  branchDict['RawSignal_R0_355'][0] = rf.Get(getName("RawProfile", run, seq, "355")).GetX()[iR0_355]
  branchDict['BinPow_R0_355'][0]    = rf.Get(getName("LidarBinnedPWProfile", run, seq, "355")).GetX()[iR0_355]
  branchDict['BinPowDev_R0_355'][0] = rf.Get(getName("LidarBinnedPWDevProfile", run, seq, "355")).GetX()[iR0_355]
  branchDict['LnBinPow_R0_355'][0]  = rf.Get(getName("LidarLnBinnedPWProfile", run, seq, "355")).GetX()[iR0_355]
  
  branchDict['RawSignal_R0_532'][0] = rf.Get(getName("RawProfile", run, seq, "532")).GetX()[iR0_532]
  branchDict['BinPow_R0_532'][0]    = rf.Get(getName("LidarBinnedPWProfile", run, seq, "532")).GetX()[iR0_532]
  branchDict['BinPowDev_R0_532'][0] = rf.Get(getName("LidarBinnedPWDevProfile", run, seq, "532")).GetX()[iR0_532]
  branchDict['LnBinPow_R0_532'][0]  = rf.Get(getName("LidarLnBinnedPWProfile", run, seq, "532")).GetX()[iR0_532]

  branchDict['RawSignal_10_355'][0] = rf.Get(getName("RawProfile", run, seq, "355")).GetX()[i10]
  branchDict['BinPow_10_355'][0]    = rf.Get(getName("LidarBinnedPWProfile", run, seq, "355")).GetX()[i10]
  branchDict['BinPowDev_10_355'][0] = rf.Get(getName("LidarBinnedPWDevProfile", run, seq, "355")).GetX()[i10]
  branchDict['LnBinPow_10_355'][0]  = rf.Get(getName("LidarLnBinnedPWProfile", run, seq, "355")).GetX()[i10]
  
  branchDict['RawSignal_10_532'][0] = rf.Get(getName("RawProfile", run, seq, "532")).GetX()[i10]
  branchDict['BinPow_10_532'][0]    = rf.Get(getName("LidarBinnedPWProfile", run, seq, "532")).GetX()[i10]
  branchDict['BinPowDev_10_532'][0] = rf.Get(getName("LidarBinnedPWDevProfile", run, seq, "532")).GetX()[i10]
  branchDict['LnBinPow_10_532'][0]  = rf.Get(getName("LidarLnBinnedPWProfile", run, seq, "532")).GetX()[i10]
  
  # Extinction Model at R0
  M_355=rf.Get(getName("Extinction_M", run, seq, "355"))
  branchDict['ExtM_R0_355'][0] = M_355.GetX()[M_355.GetN()-1]
  M_532=rf.Get(getName("Extinction_M", run, seq, "532"))
  branchDict['ExtM_R0_532'][0] = M_532.GetX()[M_532.GetN()-1]

  # OD - AOD
  od355=rf.Get(getName("OD", run, seq, "355"))
  branchDict['OD_Water_355'][0]=od355.GetY()[0]
  branchDict['OD_355'][0]=od355.GetY()[1]
  branchDict['AOD_355'][0]=od355.GetY()[2]
  branchDict['OD_Model_355'][0]=od355.GetY()[3]
  branchDict['AOD_Model_355'][0]=od355.GetY()[4]

  od532=rf.Get(getName("OD", run, seq, "532"))
  branchDict['OD_Water_532'][0]=od532.GetY()[0]
  branchDict['OD_532'][0]=od532.GetY()[1]
  branchDict['AOD_532'][0]=od532.GetY()[2]
  branchDict['OD_Model_532'][0]=od532.GetY()[3]
  branchDict['AOD_Model_532'][0]=od532.GetY()[4]

  # close file  
  rf.Close()
  # fill tree
  tree.Fill()
  counter=counter+1

outFile.cd()
tree.Write()
outFile.Close()

print("Analyzed %s runs"%counter)
#out=ROOT.TFile( 'HessLidarMeteo.root')
#newtree=out.Get("LidarMeteo")
#newtree.Scan("RunNumber:Meteo_MeanTemperature")
#out.Close()

