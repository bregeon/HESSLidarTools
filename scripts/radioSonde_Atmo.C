/* Test KB atmosphere against radio sonde data
  @author JB@LUPM October 2014 
*/


#include "LidarTools/AtmoPlotter.hh"
#include "LidarTools/RayleighScattering.hh"

#include "TSystem.h"
#include "TLegend.h"
#include "TKey.h"
#include "TF1.h"
#include "TH1F.h"

void radioSonde_Atmo()
{
// Call AtmoPlotter to get expected profile
LidarTools::AtmoPlotter *plotter = new LidarTools::AtmoPlotter();
plotter->InitAbsorption(gSystem->ExpandPathName("$HESSUSER/LidarTools/data/atm_trans_1800_1_10_0_0_1800.dat"));
//plotter->InitAbsorption("/data/Hess/work/fedora/LidarTools/data/atm_trans_1835_1_10_0_0_1835.dat");
  int blue=355, green=532;
  plotter->FillTauGraph(blue);
  plotter->FillAlphaGraph(blue);
  plotter->FillTauGraph(green);
  plotter->FillAlphaGraph(green);


// plotter->InitTabAtmo(10); // does not exist
plotter->InitAtmoProf("../data/atmprof10.dat", true);

//Kaskade::TabulatedAtmosphere *atmo = plotter->GetTabAtmo();
LidarTools::AtmoProfile *atmo=plotter->GetAtmoProf();

// Another AtmoPlotter to test for KB atmosphere with no aerosols
LidarTools::AtmoPlotter *plotter1835 = new LidarTools::AtmoPlotter(1835);
plotter1835->InitAbsorption(gSystem->ExpandPathName("$HESSUSER/LidarTools/data/atm_trans_1835_1_0_0_0_1835.dat"));
  plotter1835->FillTauGraph(blue);
  plotter1835->FillAlphaGraph(blue);
  plotter1835->FillTauGraph(green);
  plotter1835->FillAlphaGraph(green);

// Rayleigh scattering calculator
LidarTools::RayleighScattering *rayleigh= new LidarTools::RayleighScattering();


TGraph *blueRayleigh = new TGraph();
TGraph *greenRayleigh= new TGraph();
blueRayleigh->SetNameTitle("Rayleigh_355nm","Theoretical Rayleight 355 nm (atmo1)");
blueRayleigh->GetXaxis()->SetTitle("Rayleigh Extinction");
blueRayleigh->GetYaxis()->SetTitle("Altitude (m)");
blueRayleigh->SetMarkerColor(kBlue);
blueRayleigh->SetMarkerStyle(4);
greenRayleigh->SetNameTitle("Rayleigh_532nm","Theoretical Rayleight 532 nm (atmo1)");
greenRayleigh->GetXaxis()->SetTitle("Rayleigh Extinction");
greenRayleigh->GetYaxis()->SetTitle("Altitude (m)");
greenRayleigh->SetMarkerColor(kGreen);
greenRayleigh->SetMarkerStyle(4);
TGraph *blueReverse = new TGraph();
TGraph *greenReverse= new TGraph();

TGraph *kbPressure = new TGraph();
kbPressure->SetMarkerColor(kRed);
kbPressure->SetMarkerStyle(kFullCircle);
kbPressure->SetMarkerSize(0.4);
TGraph *kbTemperature = new TGraph();
kbTemperature->SetMarkerColor(kRed);
kbTemperature->SetMarkerStyle(kFullCircle);
kbTemperature->SetMarkerSize(0.4);

// Loop on layers
  double h=1000;
  int i=0;
  while(h<35000){
    double p=atmo->GetPressure(h);
    double t=atmo->GetTemperature(h);
    double beta355=rayleigh->Beta(0.355, p, t);
    double beta532=rayleigh->Beta(0.532, p, t);
    //std::cout<<h<<" "<<p<<" "<<t<<" beta355 "<<beta355<<" beta532 "<<beta532<<std::endl;
    blueRayleigh->SetPoint(i,  beta355, h); 
    greenRayleigh->SetPoint(i, beta532, h);
    blueReverse->SetPoint(i, h, beta355); 
    greenReverse->SetPoint(i, h, beta532);
    kbPressure-> SetPoint(i, p, h);
    kbTemperature-> SetPoint(i, t, h);
    h+=1000.;
    i++;
    }


// Get the main radio sonde canvas
// Plot radiosonde P,T profiles and official atmosphere P,T profiles
TFile *sondesFile=new TFile("/data/Hess/work/pro/LidarTools/data/RadioSondeData_2009-2014.root");
TCanvas *MainCanvas = (TCanvas*)sondesFile->Get("AllSondesCanvas");
MainCanvas->Draw();

MainCanvas->cd(1);
kbPressure->Draw("PS");
TLegend *lp = new TLegend(0.4,0.8,0.9,0.9);
lp->AddEntry(kbPressure,"atmprof10.dat pressure", "p");
lp->AddEntry("All Radiosondes Pressures","radiosondes pressures 2009-2014");
lp->Draw("same");

MainCanvas->cd(2);
kbTemperature->Draw("PS");
TLegend *lt = new TLegend(0.4,0.8,0.9,0.9);
lt->AddEntry(kbTemperature,"atmprof10.dat temperature", "p");
lt->AddEntry("All Radiosondes Temperatures","radiosondes temperatures 2009-2014");
lt->Draw("same");
MainCanvas->cd();
MainCanvas->Update();

// Get all the radiosonde P,T profiles
TGraph *allR355=new TGraph();
allR355->SetNameTitle("RadioSondeRayleigh355","Radiosondes Rayleigh 355 nm");
TGraph *allR532=new TGraph();
allR532->SetNameTitle("RadioSondeRayleigh532","Radiosondes Rayleigh 532 nm");

TIter nextkey(sondesFile->GetListOfKeys());
TKey *key;
int k=0;
while (key = (TKey*)nextkey()) {
    TGraph *graphP=(TGraph*)key->ReadObj();
    TString nameP=graphP->GetName();
    // Pressure profile
    if(nameP.First("P")>0 && nameP.First("_")>0){
      //std::cout<<nameP<<std::endl;
      // Get also temperature
      TString nameT=nameP.ReplaceAll("Pressure","Temperature");
      TGraph *graphT=(TGraph*)sondesFile->Get(nameT);
      if(graphT->GetN()>10 && graphP->GetN()>10)
        {
        for(int i=0; i<graphP->GetN(); i++){
          Double_t hp=0., p=0., ht=0., t=0.;
          graphP->GetPoint(i,p,hp);
          graphT->GetPoint(i,t,ht);
//           std::cout<<hp<<" "<<p<<" "<<ht<<" "<<t
//                        <<" "<<rayleigh->Beta(0.355, p, t)
//                        <<" "<<rayleigh->Beta(0.355, p, t)<<std::endl;
          double ray355 = rayleigh->Beta(0.355, p, t); 
          double ray532 = rayleigh->Beta(0.532, p, t);
          if(hp<35000 && hp>1800 && p>10 && p<1100 && t>100 && t<350){
            allR355->SetPoint(k, ray355, hp);
            allR532->SetPoint(k, ray532, hp);          
            k++;
            }
          }
        }
      }    
   }

// Canvas to test Rayleigh derived from radiosonde data P,T profiles
// against Rayleigh derived from official atmosphere P,T profile   
TCanvas *rayCan= new TCanvas("RayleighCanvas","Rayleigh Canvas",30, 50,650,850);
rayCan->Divide(1,2);
rayCan->cd(1);
allR355->Draw("AP*");
allR355->GetXaxis()->SetTitle("extinction (m^{-1})");
allR355->GetYaxis()->SetTitle("Altitude (m)");
allR355->GetYaxis()->SetTitleOffset(1.25);

blueRayleigh->Draw("PS");
rayCan->cd(2);
allR532->Draw("AP*");
allR532->GetXaxis()->SetTitle("extinction (m^{-1})");
allR532->GetYaxis()->SetTitle("Altitude (m)");
allR532->GetYaxis()->SetTitleOffset(1.25);
greenRayleigh->Draw("PS");
rayCan->Update();


// Canvas To test Rayleigh from P,T profile against Extinction from MODTRAN
TCanvas *atmoCan= new TCanvas("extinctionCanvas","Extinction Canvas",30, 50,650,850);
atmoCan->Divide(1,2);

atmoCan->cd(1);
TGraph* blueAlpha=plotter->GetAlphaGraph(355);
TGraph* blueAlpha1835=plotter1835->GetAlphaGraph(355);
blueAlpha->Draw("AP*");
blueAlpha->GetXaxis()->SetTitle("extinction 355 nm (m^{-1})");
blueAlpha->GetYaxis()->SetTitle("Altitude (m)");
blueAlpha->GetYaxis()->SetTitleOffset(1.25);
blueAlpha->SetMarkerStyle(25);
blueAlpha->SetMarkerSize(0.4);
blueRayleigh->Draw("PSL");
blueAlpha1835->Draw("PS");
blueAlpha1835->SetMarkerStyle(26);
blueAlpha1835->SetMarkerSize(0.4);
TLegend *leg355 = new TLegend(0.35,0.7,0.9,0.9);
  leg355->AddEntry(blueAlpha,"atm_trans_1800_1_10_0_0_1800.dat","p");
  leg355->AddEntry(blueAlpha1835,"atm_trans_1835_1_0_0_0_1835.dat (no aerosols)","p");
  leg355->AddEntry(blueRayleigh,"Rayleigh atmoprof10.dat","pl");
  leg355->Draw();

atmoCan->cd(2);
TGraph* greenAlpha=plotter->GetAlphaGraph(532);
TGraph* greenAlpha1835=plotter1835->GetAlphaGraph(532);
greenAlpha->Draw("AP*");
greenAlpha->GetXaxis()->SetTitle("extinction 355 nm (m^{-1})");
greenAlpha->GetYaxis()->SetTitle("Altitude (m)");
greenAlpha->GetYaxis()->SetTitleOffset(1.25);
greenAlpha->SetMarkerStyle(25);
greenAlpha->SetMarkerSize(0.4);
greenRayleigh->Draw("PSL");
greenAlpha1835->Draw("PS");
greenAlpha1835->SetMarkerStyle(26);
greenAlpha1835->SetMarkerSize(0.4);
  TLegend *leg532 = new TLegend(0.35,0.7,0.9,0.9);
    leg532->AddEntry(greenAlpha,"atm_trans_1800_1_10_0_0_1800.dat","p");
    leg532->AddEntry(greenAlpha1835,"atm_trans_1835_1_0_0_0_1835.dat (no aerosols)","p");
    leg532->AddEntry(greenRayleigh,"Rayleigh atmoprof10.dat","pl");
    leg532->Draw();

// Canvas to look at residuals from radio sonde data to analytical Rayleigh
// extinction
TCanvas *fitCan= new TCanvas("FitCanvas","Fit Canvas",30, 50,550,550);
TH1F *h532 = new TH1F("SondesToRefResiduals532","Rayleigh Sondes To Ref Residuals 532 nm",500,-30,30);
for(int i=0; i<allR532->GetN(); i++){
  Double_t h=0., r=0.;
  allR532->GetPoint(i,r,h);
  if(h>10000 && h<20000){
    Double_t rayg=greenReverse->Eval(h);
    Double_t d=(r-rayg)/rayg*100;
    h532->Fill(d);
    }
  }
h532->Draw();
h532->GetXaxis()->SetTitle("(sonde-ref)/ref*100");

// fitCan->Divide(2,2);
// fitCan->cd(1);
// blueReverse->Draw("AP*");
// TF1 *fb=new TF1("fb","[0]*exp([1]*x+[2])+[3]*exp([4]*x+[5])",1e-8,1e-2);
// fb->SetParameters(1.3e+03,-8.2e+05 ,2.4, 1.0e+04,-3.8e+04 ,0.85);
// blueReverse->Fit("fb","");

//fitCan->cd(2);
//greenReverse->Draw("AP*");
// TF1 *fg=new TF1("fg","[0]*exp([1]*x+[2])+[3]*exp([4]*x+[5])",1e-8,1e-2);
// fg->SetParameters(5e+03,   -4.5e+06 ,1.1, 2.30e+03,-2.2e+05 ,2.3);
// greenReverse->Fit("fg","");

// fitCan->cd(3);
// TH1F *h355=new TH1F("h355","h355",500,-30,30);
// for(int i=0; i<allR355->GetN(); i++){
//   Double_t h=0., r=0.;
//   allR355->GetPoint(i,r,h);
//   if(h>10000 && h<20000){
//     Double_t rayb=blueReverse->Eval(h);
//     Double_t d=(r-rayb)/rayb*100;
//     //std::cout<<d<<std::endl;
//     h355->Fill(d);
//     }
//   }
// h355->Draw();
  
//fitCan->cd(4);

}
