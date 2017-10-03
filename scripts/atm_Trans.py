#!/bin/env python

# Plot the atmospheric transmission and extinction coefficient
# for a given Modtran like atmospheric transmission table
#

import ROOT
import numpy
import os
from scipy import interpolate

def getName(txt, run, seq, wl):
    return '%s_%s_%s_%s'%(txt, int(run), int(seq), wl)

def readAtmoTrans(fname='../data/atm_trans_1800_1_10_0_0_1800.dat'):
    ''' Read atmospheric transmission table
    '''
    if os.getenv('HESSUSER'):
        absName=os.path.join(os.getenv("HESSUSER"),'LidarTools/data/atm_trans_1800_1_10_0_0_1800.dat')
    elif os.getenv('HESSROOT'):
        absName=os.path.join(os.getenv("HESSROOT"),'LidarTools/data/atm_trans_1800_1_10_0_0_1800.dat')
    else:
        absName='../data/atm_trans_1800_1_10_0_0_1800.dat'
    content=open(absName).readlines()
    
    opt_depth={}
    extinction={}
    
    for l in content[14:]:
        all=l.split()
        if l[0]=='#':
            obs_alt=float(all[2].strip(','))
            altitudes=numpy.array(all[4:],'f') #a.s.l.
            altitudes=altitudes*1000 # in meters
        else:
            opt_depth[float(all[0])]=numpy.array(all[1:],'f')
    
    for wl,depth in opt_depth.items():
        alpha=[]
        for i in range(len(depth)-1):
            alpha.append((depth[i+1]-depth[i])/(altitudes[i+1]-altitudes[i]))
        extinction[wl]=numpy.array(alpha,'f')
    
    return content[:15], obs_alt, altitudes, opt_depth, extinction


def writeAtmoTrans(run, header, scaled_od, fname=None):
    ''' Dump the scaled optical depth table to an ASCII file
    in Kaskade/Corsika input file format
    '''
    if fname is None:
        fname='atm_trans_Lidar_%s.dat'%run
    content=header
    for wl, od_prof in scaled_od.items():
        txt='        %03d     '%wl
        for od in od_prof:
            if od<10:            
                txt+='%10.6f'%od
            else:
                txt+='%10.2f'%od
        content.append(txt+'\n')
    open(fname,'w').writelines(content)    
    print('Done.')


def getLidarODData(fname='HESS_Lidar_Profiles_Sp5050_17022017.root', run=67220, wl=532):
    ''' Get Lidar Transmission profile with altitudes
    '''
    try:
        f=ROOT.TFile.Open(fname)
    except:
        print('Could not open file %s'%f)
        raise(f)
    tg=f.Get(getName('Opacity', run, seq=1, wl=wl))
    print('Found %s'%tg.GetName())
    N=tg.GetN()

    od=tg.GetY()    
    od.SetSize(N*8) #hardcoded 8 bit float
    npod=numpy.frombuffer(od,float)

    alt=tg.GetX()
    alt.SetSize(N*8)
    npalt=numpy.frombuffer(alt,float)
    
    return npod,npalt


def getLidarODValues(npalt, npod, newalt, opt_depth_wl):
    ''' Get Transmission values for a given array of altitudes
    '''
    # x=alt y=nptrans
    tck = interpolate.splrep(npalt, npod, s=0)        
    newod = interpolate.splev(newalt, tck, der=0)
    # no extrapolation, use model outside measurements
    deltaod=0
    for i in range(len(newalt)):
        #print('%s alt %s %s %s'%(i, newalt[i], npalt[0], npalt[-1]))
        if newalt[i]<npalt[0]: #below AltMin - constant
            newod[i]=npod[0]
        elif newalt[i]>npalt[-2]: #above R0 -1 bin - follow model
            if deltaod==0:
                deltaod=newod[i]-opt_depth_wl[i]
                #print('deltaod %s %s %s'%(deltaod, i, newalt[i]))
            newod[i]=opt_depth_wl[i]+deltaod
    return newod

def getODRatio(newod, opt_depth_wl):
    od_ratio=newod/opt_depth_wl
    return od_ratio

def getScaledOD(opt_depth, od_ratio, corr=1):
    ''' Scale all wavelength using 532 nm measurement
    '''
    scaled_od={}
    for wl,od in opt_depth.items():
        scaled_od[wl]=opt_depth[wl]*od_ratio*corr #pow(wl/532.,1.3)
        for i in range(len(scaled_od[wl])):
            if scaled_od[wl][i]>10:
                scaled_od[wl][i]=99999.00        
    return scaled_od
    
def getIndex(myarray,val):
    return len(myarray)-sum(myarray>val)

def fillTGraph(wl, tg, opt_depth, altitudes, n):
    [tg.SetPoint(i, opt_depth[wl][i], altitudes[i]) for i in range(0,n)]

def fillTGraphSW(wl, tg, opt_depth, altitudes, n):
    [tg.SetPoint(i, altitudes[i], opt_depth[wl][i]) for i in range(0,n)]    

def plotTrans(opt_depth, altitudes, extinction):
    ''' plot stuff
    '''
    # Show blue and green wavelengths
    N25=getIndex(altitudes, 25000)
        
    can=ROOT.TCanvas("TransCan", "Atmospheric Transmission", 30,50, 650,700)
    can.Divide(1,3)
    can.cd(1)
    g355=ROOT.TGraph()
    g355.SetTitle("Kaskade Atmospheric Transmision table")
    fillTGraph(355, g355, opt_depth, altitudes, N25)
    g355.SetMarkerColor(ROOT.kBlue)
    g355.GetXaxis().SetTitle('Optical\,Depth\,:\,\\tau(h)=\int_{h_{HESS}}^h{\\alpha}(h)dh')
    g355.GetYaxis().SetTitle('Altitude (km, above site)')
    g355.Draw("AP*")
    
    g532=ROOT.TGraph()
    fillTGraph(532, g532, opt_depth, altitudes, N25)
    g532.SetMarkerColor(ROOT.kGreen)
    g532.Draw("SP*")
    
    can.cd(2)
    g355Ext=ROOT.TGraph()
    g355Ext.SetTitle("Atmospheric Extinction (alpha)")
    [g355Ext.SetPoint(i, extinction[355][i], altitudes[i]) for i in range(0,N25)]
    g355Ext.SetMarkerColor(ROOT.kBlue)
    g355Ext.GetXaxis().SetTitle('Extinction\, \\alpha (km^{-1})')
    g355Ext.GetYaxis().SetTitle('Altitude (km,  above site)')
    g355ExtX5=ROOT.TGraph()
    [g355ExtX5.SetPoint(i, extinction[355][i]/4., altitudes[i]) for i in range(0,N25)]
    g355ExtX5.SetMarkerColor(ROOT.kBlue)
    g355Ext.Draw("AP*")
    
    can.cd(3)
    g532Ext=ROOT.TGraph()
    g532Ext.SetTitle("Atmospheric Extinction (alpha)")
    [g532Ext.SetPoint(i, extinction[532][i], altitudes[i]) for i in range(0,N25)]
    g532Ext.SetMarkerColor(ROOT.kGreen)
    g532Ext.GetXaxis().SetTitle('Extinction\, \\alpha (km^{-1})')
    g532Ext.GetYaxis().SetTitle('Altitude (km, above site)')
    g532Ext.Draw("AP*")
    g355ExtX5.Draw("PS*")
    
    
    # Get Alpha(10 km), that is alpha_0 for Klett initialization
    N11=getIndex(altitudes, 10)
    print 'From %d km to %d km:'%(altitudes[N11], altitudes[N11+1])
    print '  At 355 nm (Blue) : tau(11)-tau(10) = %0.5f-%0.5f = %0.5f = alpha(10)'%\
           (opt_depth[355][N11+1], opt_depth[355][N11+1], opt_depth[355][N11+1]-opt_depth[355][N11])
    print '  At 532 nm (Green): tau(11)-tau(10) = %0.5f-%0.5f = %0.5f = alpha(10)'%\
           (opt_depth[532][N11+1], opt_depth[532][N11+1], opt_depth[532][N11+1]-opt_depth[532][N11])
    
    # Extinction km^-1
    return can, g355, g355Ext, g355ExtX5, g532, g532Ext, g355ExtX5


def plotNewOD(run, opt_depth, altitudes, newod, npalt, npod, od_ratio):
    # Show blue and green wavelengths
    N25=getIndex(altitudes, 110000)
        
    can=ROOT.TCanvas("ODCan", "Atmospheric Transmission", 30,50, 650,700)
    can.Divide(2)
    can.cd(1)

    g532fake=ROOT.TGraph(2)
    g532fake.SetTitle('Optical depth profile')
    g532fake.SetPoint(0, 0., 0.)
    g532fake.SetPoint(1, 0.5, 100000.)
    g532fake.Draw("AP")
    g532fake.GetXaxis().SetTitle('Optical\,Depth\,:\,\\tau(h)=\int_{h_{HESS}}^h{\\alpha}(h)dh')
    g532fake.GetYaxis().SetTitle('Altitude (km, above site)')


    g532=ROOT.TGraph()
    fillTGraph(532, g532, opt_depth, altitudes, N25)
    g532.GetXaxis().SetTitle('Optical\,Depth\,:\,\\tau(h)=\int_{h_{HESS}}^h{\\alpha}(h)dh')
    g532.GetYaxis().SetTitle('Altitude (km, above site)')
    g532.SetMarkerColor(ROOT.kGreen)
    g532.Draw("PS*")
    
    g532new=ROOT.TGraph()
    [g532new.SetPoint(i, newod[i], altitudes[i]) for i in range(0,N25)]
    g532new.SetMarkerStyle(ROOT.kCircle)
    g532new.Draw("PS")

    g532meas=ROOT.TGraph()
    [g532meas.SetPoint(i, npod[i], npalt[i]) for i in range(0,len(npod))]
    g532meas.SetMarkerStyle(ROOT.kFullCircle)
    g532meas.Draw("LS")
    
    leg = ROOT.TLegend(0.6,0.75,0.9,0.9);
    header="Run %s"%run
    leg.AddEntry("",header,"")
    leg.AddEntry(g532,"Modtran","P")
    leg.AddEntry(g532new,"My Estimate","P")
    leg.AddEntry(g532meas,"Lidar Measurement","L")
    leg.SetTextSize(0.025)
    leg.Draw("same")
    
    can.cd(2)
    g532ratio=ROOT.TGraph()
    [g532ratio.SetPoint(i, od_ratio[i], altitudes[i]) for i in range(0,N25)]
    g532ratio.SetMarkerStyle(ROOT.kCircle)
    g532ratio.Draw("AP*")

    can.Update()
    
    return can, g532fake, g532, g532new, g532meas, leg, g532ratio


def plotScaledOD(run, opt_depth, altitudes, scaled_od, newod355):
    ''' Plot original ODs and resulting scaled ODs
    '''
        # Show blue and green wavelengths
    altmax=21000    
    N25=getIndex(altitudes, altmax)
        
    can=ROOT.TCanvas("ScaledODCan", "Atmospheric Transmission", 30,50, 850,700)
    g532fake=ROOT.TGraph(2)
    g532fake.SetTitle('Optical depth profile')
    g532fake.SetPoint(0, 0., 0.)
    g532fake.SetPoint(1, altmax, 1.)
    g532fake.Draw("AP")
    g532fake.GetXaxis().SetTitle('Altitude (km, above site)')
    g532fake.GetYaxis().SetTitle('Optical\,Depth\,:\,\\tau(h)=\int_{h_{HESS}}^h{\\alpha}(h)dh')
    
    g532=ROOT.TGraph()
    g532.SetName('Opt_Depth_Model_532')
    fillTGraphSW(532, g532, opt_depth, altitudes, N25)
    g532.SetMarkerColor(ROOT.kGreen)
    g532.Draw("PS*")
    
    g532new=ROOT.TGraph()
    fillTGraphSW(532, g532new, scaled_od, altitudes, N25)  
    g532new.SetName('Opt_Depth_Scaled_532')
    g532new.SetMarkerStyle(ROOT.kCircle)
    g532new.SetMarkerColor(ROOT.kGreen)
    g532new.Draw("PS")

    g355new=ROOT.TGraph()
    g355new.SetName('Opt_Depth_Lidar_355')
    [g355new.SetPoint(i, altitudes[i], newod355[i]) for i in range(0,N25)]
    g355new.SetMarkerStyle(ROOT.kCircle)
    g355new.SetMarkerColor(ROOT.kBlue)
    g355new.Draw("PS")
    
    g355scaled=ROOT.TGraph()
    g355scaled.SetName('Opt_Depth_Scaled_355')
    fillTGraphSW(355, g355scaled, scaled_od, altitudes, N25)    
    g355scaled.SetLineColor(ROOT.kBlue)
    g355scaled.SetLineWidth(2)    
    g355scaled.Draw("PSL")


    leg = ROOT.TLegend(0.6,0.75,0.9,0.9);
    header="Run %s"%run
    leg.AddEntry("",header,"")
    leg.AddEntry(g532,"Modtran","P")
    leg.AddEntry(g532new,"Lidar 532","P")
    leg.AddEntry(g355new,"Lidar 355","P")
    leg.AddEntry(g355scaled,"Scaled 355","L")
    leg.SetTextSize(0.025)
    leg.Draw("same")

    gModel={}
    gScaled={}
    for wl in opt_depth.keys():
        if not wl%50:
            gModel[wl]=ROOT.TGraph()
            gModel[wl].SetName('Opt_Depth_Model_%s'%wl)
            fillTGraphSW(wl, gModel[wl], opt_depth, altitudes, N25)
            gModel[wl].SetLineStyle(2)
            gModel[wl].Draw("PSL")  
            gScaled[wl]=ROOT.TGraph()
            gScaled[wl].SetName('Opt_Depth_Lidar_%s'%wl)
            fillTGraphSW(wl, gScaled[wl], scaled_od, altitudes, N25)    
            gScaled[wl].SetLineColor(ROOT.kRed)
            gScaled[wl].Draw("PSL")
            
    return can, g532fake, g532, g532new, leg, gModel, gScaled, g355new, g355scaled

def processOne(run):
    ''' Process one run
    Read Atmo Trans model, get 532 nm lidar profile
    scale optical depth per measurement at 532 nm
    dump new atmo trans table for Kaskade/Corsika
    '''
    header, obs_alt, altitudes, opt_depth,extinction=readAtmoTrans()
    
    # od model
    plots=plotTrans(opt_depth, altitudes, extinction)
    
    # Estimate OD from ground to 100km from Lidar data and Modtran model
    #run=67220
    # 532nm
    wl532=532    
    npod,npalt=getLidarODData(fname='HESS_Lidar_Profiles_Sp5050_17022017.root', run=run, wl=wl532)
    newod=getLidarODValues(npalt, npod, altitudes, opt_depth[wl532])
#    for i in range(len(altitudes)):
#        print altitudes[i], opt_depth[wl][i], newod[i], npalt[i], npod[i]
    od_ratio_532=getODRatio(newod, opt_depth[wl532])
    odplots=plotNewOD(run, opt_depth, altitudes, newod, npalt, npod, od_ratio_532)

    # scaled results with 532nm ratio
    scaled_od=getScaledOD(opt_depth, od_ratio_532)

    # get 355 nm LIdar data for comparison
    wl355=355
    npod355,npalt355=getLidarODData(fname='HESS_Lidar_Profiles_Sp5050_17022017.root', run=run, wl=wl355)
    newod355=getLidarODValues(npalt355, npod355, altitudes, opt_depth[wl355])
    #print('Altitude\tModel\tInterp\tAltitude\tLidar')    
    #for i in range(len(altitudes)):
    #    print altitudes[i], opt_depth[wl355][i], newod355[i], npalt355[i], npod355[i]
    
    # plot results    
    scaled_plots=plotScaledOD(run, opt_depth, altitudes, scaled_od, newod355)

    # dump scaled Atmo Trans file for Kaskade/Corsika
    writeAtmoTrans(run, header, scaled_od)
    
    # Dump plots to ROOT file
    outF=ROOT.TFile.Open('Scaled_Trans_%s.root'%run,'RECREATE')
    for one in plots:
        one.Write()
    for one in odplots:
        one.Write()
    for one in scaled_plots:
        if type(one) is not dict:
            one.Write()
        else:
            for g in one.values():
                g.Write()
    outF.Write()
    outF.Close()
    
if __name__=="__main__":
    crab_bad   = [78815, 78922, 79862, 80081, 80086, 80124, 80129]
    crab_medium= [79556, 79858, 79859, 79860, 79884, 79885, 80026]
    crab_good  = [79881, 79882, 80154]
    crabruns=crab_bad+crab_medium+crab_good

    for run in crabruns:
        processOne(run)
        