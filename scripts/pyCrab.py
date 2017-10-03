#!/bin/env python
from pyAll import *
from MergedQuery import *

import ROOT

import os, subprocess

crab_bad   = [78815, 78922, 79862, 80081, 80086, 80124, 80129]
crab_medium= [79556, 79858, 79859, 79860, 79884, 79885, 80026]
crab_good  = [79881, 79882, 80154]
crabruns=crab_bad+crab_medium+crab_good

SgrAx_runs=[66784, 66785, 66786, 66803, 67094, 67095, 67113, 67114, 67161, 67162, 67189, 67190,\
            70323, 70980, 70999, 71000, 71021, 71089, 71090, 71091, 71093, 71097, 71098, 71109,\
            71111, 71112, 71113, 71114, 71861, 71862, 71863, 71864, 71866, 71887, 71888, 71889,\
            71890, 71892, 71893, 71897, 71898, 71899, 71949, 71960, 71992, 71993, 71994, 71995,\
            73991, 73993, 74795, 74799, 74861, 74944, 75042, 75069, 75182, 75671, 75836, 77284,\
            77287]

All_Sources = crabruns+SgrAx_runs

def getGraphName(txt, run, seq, wl):
    print('%s_%s_%s_%s'%(txt, run, seq, wl))
    return '%s_%s_%s_%s'%(txt, run, seq, wl)

def getCanvasName(txt, run, seq):
    print('%s_%s_%s'%(txt, run, seq))
    return '%s_%s_%s'%(txt, run, seq)

def process(runslist, outdir='./Crab_Sp5070/', Sp355='50', Sp532='70'):
  if not os.path.exists(outdir):
    os.mkdir(outdir)
  for run in runslist:
    processOne(run, outdir, Sp355, Sp532)

def pdf(outdir='./Crab_Sp5070'):
  all=glob.glob(os.path.join(outdir,'analysis*.root'))
  all.sort()
  for f in all:
    print(f)
    rf=ROOT.TFile.Open(f)
    run=int(f.split('_')[-2])
    seq=int(f.split('_')[-1][:3])
    summ=rf.Get(getCanvasName("SummaryCanvas",run,seq))
    summ.SaveAs(os.path.join(outdir,'Summary_'+str(run)+'.pdf'))
    trans=rf.Get(getCanvasName("LidarDataCanvas",run,seq))
    trans.SaveAs(os.path.join(outdir,'Transmission_'+str(run)+'.pdf'))
    rf.Close()

def merge(outdir='./Crab_Sp5070', tag='Summary'):
  all=glob.glob(os.path.join(outdir,'%s_*.pdf'%tag))
  all.sort()
  name='%s/%s_%s.pdf'%(outdir,outdir.split('_')[0],tag)
  cmd='gs -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=%s '%name
  for f in all:
     cmd+='%s '%f
  subprocess.check_output(cmd.split())

def mergeList(runlist, tag='new', outdir='./Crab_Sp5070'):
  name='%s/%_Summary_%s.pdf '%(outdir,outdir.split('_')[0],tag)
  os.system('rm -f %s'%name)
  cmd='gs -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=%s '%name
  for run in runlist:
     fname=glob.glob(os.path.join(outdir,'Summary_%s.pdf'%run))[0]
     cmd+='%s '%fname
  subprocess.check_output(cmd.split())

def processAll(runslist, outdir, Sp355='50', Sp532='70'):
  os.mkdir(outdir)
  process(runslist, outdir, Sp355, Sp532)
  pdf(outdir)
  merge(outdir)

def getRunsInfo(runsList, user, pwd):
  txt=''
  for run in runsList:
    print run
    mq = MergedQuery(user, pwd, run, run, 'info_%s.txt'%run)
    rq=mq.query()[run]
    print rq
    try:
      txt+='%s %s %s %s %s %s %s\n'%\
       (rq['RunNumber'], rq['Meteo_MeanWindSpeed'], rq['Meteo_MeanWindDirection'],
        rq['MedianNSB'], rq['ZenithEqMeanSystemRate'], rq['Tracking_ZenithMean'],
        len(rq['TelsInRun'].split(',')))
    except:
      pass
  return txt

def getTC(runsList):
    import ROOT
    f=ROOT.TFile('../data/TransparencyCoefficient_Christian.root')
    tree=f.Get('CT')
    cut=''
    for run in runsList:
      cut+='RunNumber==%s || '%run
    cut+='0'
    tree.Scan('RunNumber:TransparencyCoefficient',cut)

def plotCrabExtinction(outdir, runsList):
    seq=1
    c1=ROOT.TCanvas("CrabRunsExtinction", "Crab Runs Extinction", 30, 50,950,850)
    c1.Divide(2)
    for run in runsList:
      tf=ROOT.TFile(os.path.join(outdir,'analysis_%06d_001.root'%run) )
      
      extmodel=tf.Get( getName("ExtinctionModel", run, seq, "355"))
      extT=tf.Get(getName("Extinction_T", run, seq, "355"))      
      extP=tf.Get(getName("Extinction_P", run, seq, "355"))            
      extM=tf.Get(getName("Extinction_M", run, seq, "355"))     

      extmodel532=tf.Get( getName("ExtinctionModel", run, seq, "532"))
      extT532=tf.Get(getName("Extinction_T", run, seq, "532"))      
      extP532=tf.Get(getName("Extinction_P", run, seq, "532"))            
      extM532=tf.Get(getName("Extinction_M", run, seq, "532"))     

      if run == runsList[0]:
        c1.cd(1)
        extFake=ROOT.TGraph(2)
        extFake.SetPoint(0,1e-6,1650)
        extFake.SetPoint(1,2e-4,10000)
        extFake.Draw("AP")       
        extFake.GetYaxis().SetRangeUser(1650,10000)
        extFake.SetTitle("Atmosphere Extinction profiles @ 355 nm")
        extFake.GetYaxis().SetTitle("Altitude above sea level (m)")
        extFake.GetXaxis().SetTitle("Extinction (m^-1)")
        extFake.GetYaxis().SetTitleSize(0.03)
        extFake.GetYaxis().SetLabelSize(0.025)
        extFake.GetYaxis().SetTitleOffset(1.6)
        extmodel.Draw("SL")        
        extM.Draw("SL")
        extT.SetLineColor(ROOT.kBlack)        
        extT.Draw("SL")        
        extT.SetLineColor(ROOT.kGreen)        

        c1.cd(2)
        extFake532=ROOT.TGraph(2)
        extFake532.SetPoint(0,1e-6,1650)
        extFake532.SetPoint(1,1e-4,10000)
        extFake532.Draw("AP")       
        extFake532.GetYaxis().SetRangeUser(1650,10000)        
        extFake532.GetXaxis().SetRangeUser(1e-7,1e-2)
        extFake532.GetYaxis().SetRangeUser(1650,10000)
        extFake532.SetLineColor(ROOT.kRed)
        extFake532.SetTitle("Atmosphere Extinction profile @ 532 nm")
        extFake532.GetYaxis().SetTitle("Altitude above sea level (m)")
        extFake532.GetXaxis().SetTitle("Extinction (m^-1)")
        extFake532.GetYaxis().SetTitleSize(0.03)
        extFake532.GetYaxis().SetLabelSize(0.025)
        extFake532.GetYaxis().SetTitleOffset(1.6)

        extmodel532.Draw("SL")               
        extM532.Draw("SL")
        extT532.SetLineColor(ROOT.kBlack)        
        extT532.Draw("SL")        
        extT532.SetLineColor(ROOT.kBlue)        
      else:
        c1.cd(1)
        extT.Draw("SL")
        extT.SetLineColor(ROOT.kGreen)
        c1.cd(2)
        extT532.Draw("SL")
        extT532.SetLineColor(ROOT.kBlue)
        #extP.Draw("SL")
        #extM.Draw("SL")
    c1.SaveAs("crabExt.root")
    return c1
        


def plotCrabTransmission(outdir, runsList):
    seq=1
    c1=ROOT.TCanvas("CrabRunsTransmission", "Crab Runs Transmission", 30, 50,1150,850)
    c1.Divide(2)
    for run in runsList:
      tf=ROOT.TFile(os.path.join(outdir,'analysis_%06d_001.root'%run) )
      
      transmodel=tf.Get( getName("TransmissionModel", run, seq, "355"))
      trans=tf.Get(getName("Transmission", run, seq, "355"))      
      
      transmodel532=tf.Get( getName("TransmissionModel", run, seq, "532"))
      trans532=tf.Get(getName("Transmission", run, seq, "532"))      

      if run == runsList[0]:
        c1.cd(1)
        transmodel.Draw("AL")       
        transmodel.GetXaxis().SetRangeUser(0,12000)
        transmodel.GetYaxis().SetRangeUser(0.6,1)
        transmodel.SetLineColor(ROOT.kRed)
        transmodel.SetTitle("Atmosphere Transmission probability @ 355 nm")
        transmodel.GetXaxis().SetTitle("Altitude above sea level (m)")
        transmodel.GetYaxis().SetTitle("Transmission Probability")
        transmodel.GetYaxis().SetTitleSize(0.03)
        transmodel.GetYaxis().SetLabelSize(0.025)
        transmodel.GetYaxis().SetTitleOffset(1.6)
        trans.Draw("SL")        
        trans.SetLineColor(ROOT.kGreen)        

        c1.cd(2)
        transmodel532.Draw("AL")       
        transmodel532.GetXaxis().SetRangeUser(0,12000)
        transmodel532.GetYaxis().SetRangeUser(0.8,1)
        transmodel532.SetLineColor(ROOT.kRed)
        transmodel532.SetTitle("Atmosphere Transmission probability @ 532 nm")
        transmodel532.GetXaxis().SetTitle("Altitude above sea level (m)")
        transmodel532.GetYaxis().SetTitle("Transmission Probability")
        transmodel532.GetYaxis().SetTitleSize(0.03)
        transmodel532.GetYaxis().SetLabelSize(0.025)
        transmodel532.GetYaxis().SetTitleOffset(1.6)
        trans532.Draw("SL")        
        trans532.SetLineColor(ROOT.kBlue)        
      else:
        c1.cd(1)
        trans.Draw("SL")
        trans.SetLineColor(ROOT.kGreen)
        c1.cd(2)
        trans532.Draw("SL")
        trans532.SetLineColor(ROOT.kBlue)
        #extP.Draw("SL")
        #extM.Draw("SL")
    c1.SaveAs("crabTrans.root")
    return c1
      
def plotCrabTrans(runslist):
    plotters=[]
    can=ROOT.TCanvas("Optical Depth")
    i=0
    for run in runslist:
        print run
        p=ROOT.LidarTools.AtmoPlotter(1800)
        p.InitAbsorption('crab_trans/atm_trans_%05d.dat'%run)
        p.FillTauGraph(532)
        p.FillAlphaGraph(532)
        plotters.append(p)
        if i==0:
            p.GetAlphaGraph(532).Draw("APL")
        else:
            p.GetAlphaGraph(532).Draw("PSL")
        i+=1
    return can

      
if __name__ == '__main__':
#  mergeList(bad, tag='Bad_Sp5070', outdir='./Crab_Sp5070')
#  mergeList(good, tag='Good_Sp5070', outdir='./Crab_Sp5070')
#  mergeList(medium, tag='Medium_Sp5070', outdir='./Crab_Sp5070')
#  mergeList(bad, tag='Bad_Sp2020', outdir='./Crab_Sp2020')
#  mergeList(good, tag='Good_Sp2020', outdir='./Crab_Sp2020')
#  mergeList(medium, tag='Medium_Sp2020', outdir='./Crab_Sp2020')

#  txt='RunNumber WindSpeed WindDir NSB ZenithEqRate Zenith NTelsInRun\n'
#  txt+=getRunsInfo(good, 'hess', 'CT5io!')
#  txt+=getRunsInfo(medium, 'hess', 'CT5io!')
#  txt+=getRunsInfo(bad, 'hess', 'CT5io!')
#  print txt

#  getTC(good)
#  getTC(medium)
#  getTC(bad)

  process(crabruns, outdir='./Crab_130217_Sp5050/', Sp355='50', Sp532='50')

#  c=plotCrabExtinction('./Crab_010916_Sp5070/',crabruns)
#  c=plotCrabTransmission('./Crab_010916_Sp5070/',crabruns)

# SgrA*
#  processAll(SgrAx_runs, './SgrAx_Sp11', Sp355='1', Sp532='1')
#  processAll(SgrAx_runs, './SgrAx_Sp1010', Sp355='10', Sp532='10')
#  processAll(SgrAx_runs, './SgrAx_Sp2020', Sp355='20', Sp532='20')
#  processAll(SgrAx_runs, './SgrAx_Sp5070', Sp355='50', Sp532='70')
#  merge('./SgrAx_Sp11', 'Transmission')
#  merge('./SgrAx_Sp1010', 'Transmission')
#  merge('./SgrAx_Sp2020', 'Transmission')
#  merge('./SgrAx_Sp5070', 'Transmission')

# All Sources
#    processAll(All_Sources, './All_Sources_Sp2020', Sp355='20', Sp532='20')
#    merge('./All_Sources_Sp2020_NoFilter', 'Transmission')
#    merge('./All_Sources_Sp2020_Filtered', 'Transmission')
#    c=plotTransmission('./All_Sources_Sp2020_NoFilter/',All_Sources)

#    processAll(All_Sources, './All_Sources_Sp5070', Sp355='50', Sp532='70')
#    merge('./All_Sources_Sp5070', 'Transmission')
#    c=plotTransmission('./All_Sources_Sp5070/',All_Sources)
