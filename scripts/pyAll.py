#!/bin/env python

import sys,os,glob,time

import ROOT

ROOT.gROOT.SetBatch(1)
ROOT.gStyle.SetCanvasColor(0)
ROOT.gStyle.SetStatBorderSize(1)

def runAnalysis(filename, outdir='./', Sp355='50', Sp532='50'):
    run=filename.split('run_')[1].split('_')[0]
    seq=filename.split('Lidar_')[1][:3]
    print('Processing %s'%filename)    

    p = ROOT.LidarTools.LidarProcessor(filename, False)    
    rc=p.Init()
    if rc==0:
        p.OverwriteConfigParam("LidarTheta","15")
        p.OverwriteConfigParam("NBins","100")
        p.OverwriteConfigParam("LogBins","0")
        p.OverwriteConfigParam("SGFilter","1")        
        p.OverwriteConfigParam("AltMin","400")
        p.OverwriteConfigParam("AltMax","12000")
        p.OverwriteConfigParam("R0_355","8200")
        p.OverwriteConfigParam("R0_532","8200")
        p.OverwriteConfigParam("TauAltMin","400")
        p.OverwriteConfigParam("TauAltMax","8200");

        p.OverwriteConfigParam("Fernald_Sp355",Sp355)
        p.OverwriteConfigParam("Fernald_Sp532",Sp532)
        # Optimize inversion
        p.OverwriteConfigParam("BkgFudgeFactor","1.01")
        p.OverwriteConfigParam("SNRatioThreshold","3")
        p.OverwriteConfigParam("OptimizeR0","1")
        p.OverwriteConfigParam("Fernald_sratio","1.01")  
        p.OverwriteConfigParam("OptimizeAC","1")
        p.OverwriteConfigParam("OptimizeAC_Hmin","6000")
#        p.OverwriteConfigParam("AlignCorr_355","0.0")
#        p.OverwriteConfigParam("AlignCorr_532","0.0")

        #Process(Bool_t applyOffset=true, Bool_t display=false)
        arc=p.Process(True, True)
        if arc == 0:
            p.SaveAs(os.path.join(outdir,"analysis_"+run+"_"+seq+".root"))
        return arc
    

def processAll(outdir, Sp355='50', Sp532='70', aboverun=0):
    if not os.path.exists(outdir):
        os.mkdir(outdir)

    oldroot=glob.glob("/data/Hess/data/LidarData_2009_2010_ROOT/run_0*.root")
    root=glob.glob("/data/Hess/data/root_files/run_0*.root")
    txt=glob.glob("/data/Hess/data/alldata/run_0*.txt")
    oldroot.sort()
    root.sort()
    txt.sort()
    done=[]
    for fname in oldroot:
        run=int(fname.split('run_0')[1].split('_')[0])
        if run>=aboverun:
            runAnalysis(fname, outdir, Sp355, Sp532)
            done.append(int(run))
#    for fname in txt:
#        run=int(fname.split('run_0')[1].split('_')[0])
#        if run>=aboverun:
#            runAnalysis(fname, outdir, Sp355, Sp532)
#            done.append(int(run))
#    for fname in root:
#        run=int(fname.split('run_0')[1].split('_')[0])
#        if run not in done and run>=aboverun:
#            runAnalysis(fname, outdir, Sp355, Sp532)
#            done.append(int(run))
    print(len(done), ' runs processed')     

def processOne(run, outdir, Sp355='50', Sp532='50'):
    root=glob.glob("/data/Hess/data/root_files/run_0"+str(run)+"*.root")
    txt=glob.glob("/data/Hess/data/alldata/run_0"+str(run)+"*.txt")
    if len(root)==1:     
        filename=root[0]
    elif  len(txt)==1:
        filename=txt[0]
    else:
        print "no file found for run ", run
        sys.exit(1)
    print('Processing %s'%filename)    
    runAnalysis(filename, outdir, Sp355, Sp532)
    
def getTransmission(run, outdir, Sp355='50', Sp532='70'):
    root=glob.glob("/data/Hess/data/root_files/run_0"+str(run)+"*.root")
    txt=glob.glob("/data/Hess/data/alldata/run_0"+str(run)+"*.txt")
    if len(root)==1:     
        filename=root[0]
    elif  len(txt)==1:
        filename=txt[0]
    else:
        print "no file found for run ", run
        sys.exit(1)
    

# Main
if __name__ == '__main__':
    print sys.argv
    #processOne(sys.argv[1], './')
#    66755
#    processAll(int(sys.argv[1]))
    #processAll(sys.argv[1])
    # 73069    
    processAll(outdir='all_Sp5050_17022017', Sp355='50', Sp532='50', aboverun=76787)
