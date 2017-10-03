import ROOT

ROOT.gStyle.SetCanvasBorderMode(0);
ROOT.gStyle.SetPadBorderMode(0);
ROOT.gStyle.SetPadColor(0);
ROOT.gStyle.SetCanvasColor(0);
ROOT.gStyle.SetTitleColor(1);
ROOT.gStyle.SetStatColor(0);
ROOT.gStyle.ToggleEventStatus();

import glob

# TC
TCFileName="/data/Hess/work/pro/LidarTools/data/TransparencyCoefficient_Christian_2015.root"
tcfile=ROOT.TFile.Open(TCFileName)
tctree=tcfile.Get("CT")
tctree.BuildIndex("RunNumber")

# Analysis results
filtered=glob.glob("All_Sources_Sp2020_Filtered/analysis*.root")
filtered.sort()
nofilter=glob.glob("All_Sources_Sp2020_NoFilter/analysis*.root")
nofilter.sort()
sp5070=glob.glob("All_Sources_Sp5070/analysis*.root")
sp5070.sort()

ref_altitude=8000

mem=[]

def getProb(tg, altitude=8000):
    pVec=tg.GetY()
    hVec=tg.GetX()
    h=30000
    i=tg.GetN()
    while h>altitude:
      i-=1
      h=hVec[i]
    return (pVec[i],h)
      

def fillProbs(flist, tgProb, thProb):
    i=0
    for f in flist:
      print(f)
      of=ROOT.TFile.Open(f)
      run=float(f.split('/')[-1][:-5].split('_')[1])
      tg=of.Get("Transmission_%05d_532"%run)
      p,h=getProb(tg, ref_altitude)
      #print(run, p, h)
      tgProb.SetPoint(i, run, p)
      thProb.Fill(p)
      i+=1

# Filter vs no Filter
def plotFnoF():
    fGProb=ROOT.TGraph()
    fHProb=ROOT.TH1F("fHProb","Filter/NoFilter Prob",30,0.85,1.05)
    fillProbs(filtered, fGProb, fHProb)

    nofGProb=ROOT.TGraph()
    nofHProb=ROOT.TH1F("nofHProb","nofHProb",30,0.85,1.05)
    fillProbs(nofilter, nofGProb, nofHProb)

    can=ROOT.TCanvas("ProbCanvas","Probability Canvas",30,50,650,850)
    can.Divide(2)
    can.cd(1)
    fGProb.Draw("AP")
    fGProb.GetXaxis().SetTitle("Run Number")
    fGProb.GetYaxis().SetTitle("Probability from %s km"%ref_altitude)
    fGProb.SetMarkerStyle(4)
    nofGProb.Draw("PS")
    nofGProb.SetMarkerStyle(5)

    can.cd(2)
    fHProb.Draw()
    fHProb.GetXaxis().SetTitle("Probability from %s km"%ref_altitude)
    nofHProb.Draw("sames")
    nofHProb.SetLineColor(2)
    nofHProb.SetLineStyle(2)
    can.Update()
    can.cd()
    
    mem.append([fGProb, fHProb, nofGProb, nofHProb, can])

# TC
def getTC(tctree, run):
    tctree.GetEntryWithIndex(int(run))
    return tctree.TransparencyCoefficient

def fillProbAndTC(flist, gProb, gTC, gCorr):
    i=0
    k=0
    for f in flist:
      print(f)
      # Prob
      of=ROOT.TFile.Open(f)
      run=int(f.split('/')[-1][:-5].split('_')[1])
      tg532=of.Get("Transmission_%05d_532"%run)
      p,h=getProb(tg532, ref_altitude)
      #print(run, p, h)
      gProb.SetPoint(i, run, p)
      # TC
      t=getTC(tctree, run)
      gTC.SetPoint(i, run, t)
      gCorr.SetPoint(i, p, t)
#      if t<0.8:
#          print(t, p)
#      else:
#          gCorr.SetPoint(k, p, t)
#          k+=1
      i+=1

# Plot TC
def plotTC(filelist):    
    gProb=ROOT.TGraph()
    gTC=ROOT.TGraph()    
    gCorr=ROOT.TGraph()
    fillProbAndTC(filelist, gProb, gTC, gCorr)
    gProb.SetPoint(len(filelist), 66000, 0.85)
    gProb.SetPoint(len(filelist)+1, 81000, 1.1)    
    can2=ROOT.TCanvas("CorrCanvas","Correlation Canvas",30,50,850,1050)
    can2.Divide(1, 2)
    can2.cd(1)
    gProb.Draw("AP")
    gProb.GetXaxis().SetTitle("Run Number")
    gProb.GetYaxis().SetTitle("TC / Probability from %s km"%ref_altitude)
    gProb.SetMarkerStyle(24)
    gTC.Draw("PS")
    gTC.SetMarkerStyle(25)
    gTC.SetMarkerColor(2)
    
    can2.cd(2)
    gCorr.Draw("AP")
    gCorr.GetXaxis().SetTitle("Probability from %s km"%ref_altitude)
    gCorr.GetYaxis().SetTitle("TC")
    gCorr.SetMarkerStyle(26)
    gCorr.Fit("pol1")
    can2.Update()
    can2.cd()
    
    print "Correlation Factor 532 nm = %0.2f"%gCorr.GetCorrelationFactor()

    mem.append([gProb, gTC, gCorr, can2])
     
#plotTC(sp5070)
plotFnoF()
