#~ MERGE_COLS_TO_DUMP=['RunNumber', 'RunDate', 'Meteo_MeanTemperature','Meteo_MeanWindSpeed','Meteo_MeanWindDirection','Meteo_MeanRelativeHumidity',\
              #~ 'Meteo_MeanPressure','Lidar_Opacity_355nm','Lidar_Opacity_532nm', 'TC','Crab',\
              #~ 'MeanSystemRate','ZenithEqMeanSystemRate','ZenithEqMeanSystemRate/MeanSystemRate',\
              #~ 'Telescope','TriggerRate', 'CorrectedTriggerRate',\
              #~ 'MedianNSB', 'NSBRelativeVariation', 'NSBRelativeDispersion',\
              #~ 'CTRadiometer_MeanTemp', 'CTRadiometer_TempRelativeVariation', 'CTRadiometer_TempRelativeDispersion',\
              #~ 'MedianScaler', 'ScalerRelativeVariation', 'ScalerRelativeDispersion',\
              #~ 'MedianTriggerRate', 'TriggerRateVariation', 'TriggerRateDispersion',\
              #~ 'Tracking_ZenithMean']
              
import ROOT
T0=ROOT.TDatime(2000,01,01,00,00,00);
ROOT.gStyle.SetTimeOffset(T0.Convert());
ROOT.gStyle.SetPalette(1)
ROOT.gStyle.SetCanvasColor(10)
ROOT.gStyle.SetFrameBorderMode(0)
ROOT.gStyle.SetFrameFillColor(0)
ROOT.gStyle.SetPadBorderMode(0)
ROOT.gStyle.SetPadColor(0)
ROOT.gStyle.SetHistFillStyle(0)
ROOT.gStyle.SetStatColor(10)
ROOT.gStyle.SetStatFont(42)
ROOT.gStyle.SetGridColor(16)
ROOT.gStyle.SetLegendBorderSize(1)
ROOT.gStyle.SetOptTitle(0)
#ROOT.gStyle.SetOptStat(111110)
ROOT.gStyle.SetPaintTextFormat("1.2g")

# meteo
modisFN="HESS_ModisMerit_15022017.root"
modisF=ROOT.TFile.Open(modisFN)
modisT=modisF.Get("ModisMerit")

# meteo
meteoFN="../data/HessLidarMeteo_all_runs_45000_115000_23022016.root"
meteoF=ROOT.TFile.Open(meteoFN)
meteoT=meteoF.Get("LidarMeteo")

# Bkg
#bkgFN='HessLidar_Merit_all_08022016.root'HessLidar_Merit_all_22022016.root
bkgFN='HESS_LidarMerit_Sp5050_R010_15022017.root'
bkgF = ROOT.TFile.Open(bkgFN)
bkgT = bkgF.Get( 'LidarMerit')

# Friend and Index
modisT.BuildIndex('RunNumber')
meteoT.BuildIndex('RunNumber')
bkgT.BuildIndex('RunNumber')
bkgT.AddFriend(modisT)
bkgT.AddFriend(meteoT)

#bkgT.Scan("RunNumber:BkgMean_355:BkgMean_532:MedianNSB")

#cut="LidarMeteo.RunNumber>0 && MedianNSB>0 && Tracking_ZenithMean<30"
#cut="LidarMeteo.RunNumber>0 && MedianNSB>0"
cut="DateTime>0"

cBkg=ROOT.TCanvas("BkgCan","BkgCanvas", 30,50,650,850)
cBkg.Divide(2,4)
cBkg.cd(1)
bkgT.Draw('BkgMean_355:DateTime>>h2Mean_Time_355',cut,'colz')
ROOT.h2Mean_Time_355.GetXaxis().SetTimeDisplay(1)
ROOT.h2Mean_Time_355.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkg.cd(2)
bkgT.Draw('BkgMean_532:DateTime>>h2Mean_Time_532',cut,'colz')
ROOT.h2Mean_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2Mean_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkg.cd(3)
bkgT.Draw('BkgMean_355:MedianNSB',cut,'colz')
cBkg.cd(4)
bkgT.Draw('BkgMean_532:MedianNSB',cut,'colz')
cBkg.cd(5)
bkgT.Draw('BkgMean_355:CTRadiometer_MeanTemp',cut,'colz')
cBkg.cd(6)
bkgT.Draw('BkgMean_532:CTRadiometer_MeanTemp',cut,'colz')
cBkg.cd(7)
bkgT.Draw('BkgMean_355:ZenithEqMeanSystemRate',cut,'colz')
cBkg.cd(8)
bkgT.Draw('BkgMean_532:ZenithEqMeanSystemRate',cut,'colz')

cBkgRMS=ROOT.TCanvas("BkgRMSCan","BkgRMSCanvas", 30,50,650,850)
cBkgRMS.Divide(2,4)
cBkgRMS.cd(1)
bkgT.Draw('BkgRMS_355:DateTime>>h2RMS_Time_355',cut,'colz')
ROOT.h2RMS_Time_355.GetXaxis().SetTimeDisplay(1)
ROOT.h2RMS_Time_355.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgRMS.cd(2)
bkgT.Draw('BkgRMS_532:DateTime>>h2RMS_Time_532',cut,'colz')
ROOT.h2RMS_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2RMS_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgRMS.cd(3)
bkgT.Draw('BkgRMS_355:MedianNSB',cut,'colz')
cBkgRMS.cd(4)
bkgT.Draw('BkgRMS_532:MedianNSB',cut,'colz')
cBkgRMS.cd(5)
bkgT.Draw('BkgRMS_355:CTRadiometer_MeanTemp',cut,'colz')
cBkgRMS.cd(6)
bkgT.Draw('BkgRMS_532:CTRadiometer_MeanTemp',cut,'colz')
cBkgRMS.cd(7)
bkgT.Draw('BkgRMS_355:ZenithEqMeanSystemRate',cut,'colz')
cBkgRMS.cd(8)
bkgT.Draw('BkgRMS_532:ZenithEqMeanSystemRate',cut,'colz')

cBkgOne=ROOT.TCanvas("BkgCanOne","BkgCanvasOne", 30,50,650,850)
cBkgOne.Divide(1,3)
cBkgOne.cd(1)
bkgT.Draw('BkgMean_532:DateTime>>h2MeanTrend_532(200)',cut,'colz')
ROOT.h2MeanTrend_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2MeanTrend_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgOne.cd(2)
bkgT.Draw('BkgRMS_532:DateTime>>h2RMSTrend_532(200)',cut,'colz')
ROOT.h2RMSTrend_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2RMSTrend_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgOne.cd(3)
bkgT.Draw('R0_532:DateTime>>h2R0Trend_532(200)',cut,'colz')
ROOT.h2R0Trend_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2R0Trend_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
ROOT.h2R0Trend_532.GetYaxis().SetTitle("R_0")

cBkgExt=ROOT.TCanvas("BkgCanExt","BkgCanvasExt", 30,50,650,850)
cBkgExt.Divide(1,3)
cBkgExt.cd(1)
bkgT.Draw('Ext_T_4_532:DateTime>>h2E4_Time_532(200)',cut,'colz')
ROOT.h2E4_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2E4_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgExt.cd(2)
bkgT.Draw('Ext_T_6_532:DateTime>>h2E6_Time_532(200)',cut,'colz')
ROOT.h2E6_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2E6_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgExt.cd(3)
bkgT.Draw('Ext_T_8_532:DateTime>>h2E8_Time_532(200)',cut,'colz')
ROOT.h2E8_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2E8_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");

cBkgTrans=ROOT.TCanvas("BkgCanTrans","BkgCanvasTrans", 30,50,650,850)
cBkgTrans.Divide(1,3)
cBkgTrans.cd(1)
bkgT.Draw('TransProb_4_532:DateTime>>h2P4_Time_532(200)',cut,'colz')
ROOT.h2P4_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2P4_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgTrans.cd(2)
bkgT.Draw('TransProb_6_532:DateTime>>h2P6_Time_532(200)',cut,'colz')
ROOT.h2P6_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2P6_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");
cBkgTrans.cd(3)
bkgT.Draw('TransProb_8_532:DateTime>>h2P8_Time_532(200)',cut,'colz')
ROOT.h2P8_Time_532.GetXaxis().SetTimeDisplay(1)
ROOT.h2P8_Time_532.GetXaxis().SetTimeFormat("%d\/%m\/%Y");

# bkgT.Draw("Ext_T_6_532:Month>>h(13,1,13)","Month>0 && Ext_T_6_532>1e-6 && Ext_T_6_532 < 100e-6","colz")
# bkgT.Draw("BkgMean_532:HoursNight>>h(18,1,19)","Month>0","colz")
cAOD=ROOT.TCanvas("AODCan","AODCanvas", 30,50,650,850)
cAOD.Divide(1,2)
cAOD.cd(1)
bkgT.Draw("AOD_532>>hAOD532(100)","","")
bkgT.Draw("AOD_355>>hAOD355(100)","","sames")
ROOT.hAOD355.SetLineColor(ROOT.kGreen)
ROOT.gPad.SetLogy(True)
cAOD.cd(2)
bkgT.Draw("AOD_355/AOD_532>>h2AOD(100,0,5)","AOD_355>0 && AOD_532>0","")
