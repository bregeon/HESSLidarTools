/*
 Simple macro to read back Lidar meteo information from
 a RunQuality ROOT file
 Copy file locally from hpss:
 rfcp $HPSS/Calibration/RunQuality/paris-0-8-14-1/qual068000/run_068884_qual.root .

 @TODO
 * should use Sash::DataSet runquality instead of runquality_tree TTree
*/


#include "kaskade_cpp/AtmoPlotter.hh"
#include "sash/DataSet.hh"
#include "parisrunquality/LidarMeteoInformation.hh"

#include "LidarTools/RayleighScattering.hh"

#include "TTree.h"
#include "TText.h"
#include "TLegend.h"
#include <fstream>
/* Convert from km to meters and use sea level
   as reference instead of observatory altitude
*/
TGraph* RenormLidarProf(TGraph* tg, float altitude=1800)
{
int N=tg->GetN();
TGraph* outg = new TGraph(N);
for(Int_t i=0; i<N; i++)
  {
    Double_t x=0., y=0.;
    tg->GetPoint(i,x,y);
    outg->SetPoint(i, x/1000.,y*1000. + altitude);
  }
return outg;
}

/* Dump Data in a txt file
*/
void Dump(TGraph* tg, TString fname="dump_data.txt")
{
std::fstream fs;
fs.open (fname, std::fstream::in | std::fstream::out | std::fstream::app);
int N=tg->GetN();
TGraph* outg = new TGraph(N);
for(Int_t i=0; i<N; i++)
  {
    Double_t x=0., y=0.;
    tg->GetPoint(i,x,y);
    fs<< y << x << std::endl;    
  }
fs.close();
}

void readQualityTree(int run=68500)
{
TString fname="/data/Hess/data/runqual/run_0";
fname+=run;
fname+="_qual.root";

TFile *f = new TFile(fname);

Sash::DataSet *qual = (Sash::DataSet*)f->Get("runquality");
qual->Print();

TTree *qtree=(TTree*)f->Get("runquality_tree");
qtree->SetBranchStatus("LidarMeteoInformation", 1);

ParisRunQuality::LidarMeteoInformation *lidar;
lidar=0;
qtree->SetBranchAddress("LidarMeteoInformation", &lidar);
qtree->GetEntry(0);
lidar->Print();
//lidar->Display();

// Call AtmoPlotter to get expected profile
Kaskade::AtmoPlotter *plotter = new Kaskade::AtmoPlotter();
plotter->InitAbsorption("/data/Hess/HESS_Soft_0-8-26/kaskade_cpp/dat/atm_trans_1800_1_10_0_0_1800.dat");
int blue=355, green=532;
TGraph *blueAlpha=plotter->GetAlphaGraph(blue);
TGraph *greenAlpha=plotter->GetAlphaGraph(green);
//Dump(blueAlpha, "Blue355.txt");
//Dump(greenAlpha, "Green532.txt");

// Use AtmoPlotter to get P and T profile
plotter->InitTabAtmo(10);
Kaskade::TabulatedAtmosphere *atmo = plotter->GetTabAtmo();
LidarTools::RayleighScattering *rayleigh= new LidarTools::RayleighScattering();
TGraph *blueRayleigh = new TGraph();
TGraph *greenRayleigh= new TGraph();
blueRayleigh->GetXaxis()->SetTitle("Rayleigh Extinction");
blueRayleigh->GetYaxis()->SetTitle("Altitude (m)");
blueRayleigh->SetMarkerColor(kBlue);
blueRayleigh->SetMarkerStyle(4);
greenRayleigh->GetXaxis()->SetTitle("Rayleigh Extinction");
greenRayleigh->GetYaxis()->SetTitle("Altitude (m)");
greenRayleigh->SetMarkerColor(kGreen);
greenRayleigh->SetMarkerStyle(4);
  double h=1000;
  int i=0;
  while(h<20000){
    double p=atmo->GetPressure(h);
    double t=atmo->GetTemperature(h);
    double beta355=rayleigh->Beta(0.355, p, t);
    double beta532=rayleigh->Beta(0.532, p, t);
    std::cout<<h<<" "<<p<<" "<<t<<" beta355 "<<beta355<<" beta532 "<<beta532<<std::endl;
    blueRayleigh->SetPoint(i,  beta355, h); 
    greenRayleigh->SetPoint(i, beta532, h); 
    h+=1000.;
    i++;
    }


// Lidar data TGraphs
TGraph *hLidarRawProfileWL1;
hLidarRawProfileWL1=lidar->FastGethLidarRawProfile532();
hLidarRawProfileWL1->GetXaxis()->SetRangeUser(-0.6,0.1);
hLidarRawProfileWL1->GetYaxis()->SetRangeUser(0,10);
hLidarRawProfileWL1->GetXaxis()->SetTitle("Raw signal (V)");
hLidarRawProfileWL1->GetYaxis()->SetTitle("Altitude (km)");

TGraph *hLidarRawProfileWL2;
hLidarRawProfileWL2=lidar->FastGethLidarRawProfile355();
hLidarRawProfileWL2->GetXaxis()->SetRangeUser(-0.6,0.1);
hLidarRawProfileWL2->GetYaxis()->SetRangeUser(0,10);
hLidarRawProfileWL2->GetXaxis()->SetTitle("Raw signal (V)");
hLidarRawProfileWL2->GetYaxis()->SetTitle("Altitude (km)");

TGraph *hLidarLnPW532;
hLidarLnPW532=lidar->FastGethLidarLnPowerProfile1();
hLidarLnPW532->SetMarkerStyle(1);
hLidarLnPW532->SetMarkerColor(kGreen);
hLidarLnPW532->SetMarkerSize(0.5);
hLidarLnPW532->GetYaxis()->SetTitle("Altitude (km)");
hLidarLnPW532->GetXaxis()->SetTitle("Ln(Power)");

TGraph *hLidarLnPW355;
hLidarLnPW355=lidar->FastGethLidarLnPowerProfile2();
hLidarLnPW355->SetMarkerStyle(1);
hLidarLnPW355->SetMarkerColor(kBlue);
hLidarLnPW355->SetMarkerSize(0.5);
hLidarLnPW355->GetYaxis()->SetTitle("Altitude (km)");
hLidarLnPW355->GetXaxis()->SetTitle("Ln(Power)");

TGraph* hLidarAtmosphereOpacity532;
hLidarAtmosphereOpacity532=lidar->FastGethLidarAtmosphereOpacity532();
TString title532="Atmosphere Opacity 532 nm (run ";
title532+=run;
title532+= ")";
hLidarAtmosphereOpacity532->SetTitle(title532);
hLidarAtmosphereOpacity532->GetXaxis()->SetTitle("Klett Alpha 532 nm");
hLidarAtmosphereOpacity532->GetYaxis()->SetTitle("Altitude (km)");

TGraph *hLidarAtmosphereOpacity355;
hLidarAtmosphereOpacity355=lidar->FastGethLidarAtmosphereOpacity355();
TString title355="Atmosphere Opacity 355 nm (run ";
title355+=run;
title355+= ")";
hLidarAtmosphereOpacity355->SetTitle(title355);
hLidarAtmosphereOpacity355->GetXaxis()->SetTitle("Klett Alpha 355 nm");
hLidarAtmosphereOpacity355->GetYaxis()->SetTitle("Altitude (km)");

// Test renormalization
TGraph *gLidarGreen532;
gLidarGreen532=RenormLidarProf(hLidarAtmosphereOpacity532);
gLidarGreen532->SetName("LidarGreen532");
TGraph *gLidarBlue355;
gLidarBlue355=RenormLidarProf(hLidarAtmosphereOpacity355);
gLidarBlue355->SetName("LidarBlue355");


// Plot on Canvas
TCanvas *c532=new TCanvas("Opacity532Can","Opacity532",30,50,1250,650);
c532->Divide(3);
c532->cd(1);
  hLidarRawProfileWL1->Draw("AP");
c532->cd(2);
  hLidarAtmosphereOpacity532->Draw("AP");
  TText *txt532 = new TText();
  char op532[50];
  sprintf(op532,"Opacity(4km)=%0.3f",lidar->GetLidarTau4WL1());
  txt532->DrawTextNDC(0.4,0.7,op532);
c532->cd(3);
  hLidarLnPW532->Draw("AP");

TString outName="run_0";
outName+=run;
outName+="_lidarprof_532.png";
c532->SaveAs(outName);

TCanvas *c355=new TCanvas("Opacity355Can","Opacity355",30,80,1250,650);
c355->Divide(3);
c355->cd(1);
  hLidarRawProfileWL2->Draw("AP");
c355->cd(2);
  hLidarAtmosphereOpacity355->Draw("AP");
  TText *txt355 = new TText();
  char op355[50];
  sprintf(op355,"Opacity(4km)=%0.3f",lidar->GetLidarTau4WL2());
  txt355->DrawTextNDC(0.4,0.7,op355);
c355->cd(3);
  hLidarLnPW355->Draw("AP");

outName="run_0";
outName+=run;
outName+="_lidarprof_355.png";
c355->SaveAs(outName);

TCanvas *cPWComp=new TCanvas("PowerComp","Power Comparison",30,80,750,850);
  hLidarLnPW355->Draw("AP");
  hLidarLnPW532->Draw("PS");

TCanvas *cComp=new TCanvas("TransModel","TransModel",30,50,750,850);
cComp->Divide(2,1);
cComp->cd(1);
  blueAlpha->Draw("AP*");
  blueAlpha->GetXaxis()->SetTitle("Klett Alpha 355 nm (m^{-1})");
  blueAlpha->GetYaxis()->SetTitle("Altitude above sea level (m)");
  blueAlpha->GetYaxis()->SetTitleOffset(1.2);
  gLidarBlue355->Draw("PS*");
  blueRayleigh->Draw("PSL");  
  TLegend *leg355 = new TLegend(0.7,0.7,0.9,0.9);
    leg355->AddEntry(blueAlpha,"MC Model","p");
    leg355->AddEntry(blueRayleigh,"Rayleigh 355 nm","pl");
    TString tname="Lidar Data (";
    tname+=run;
    tname+=")";
    leg355->AddEntry(gLidarBlue355,tname,"p");
    leg355->Draw();

cComp->cd(2);
  greenAlpha->Draw("AP*");
  greenAlpha->GetXaxis()->SetTitle("Klett Alpha 532 nm (m^{-1})");
  greenAlpha->GetYaxis()->SetTitle("Altitude above sea level (m)");
  greenAlpha->GetYaxis()->SetTitleOffset(1.2);
  gLidarGreen532->Draw("PS*");
  greenRayleigh->Draw("PSL");
  TLegend *leg532 = new TLegend(0.7,0.7,0.9,0.9);
    leg532->AddEntry(greenAlpha,"MC Model","p");
    leg532->AddEntry(greenRayleigh,"Rayleigh 532 nm","pl");
    tname="Lidar Data (";
    tname+=run;
    tname+=")";
    leg532->AddEntry(gLidarGreen532,tname,"p");
    leg532->Draw();

outName="run_0";
outName+=run;
outName+="_CompToModel.png";
cComp->SaveAs(outName);


// TFile *outputFile = new TFile("CompareLidarToModel.root","recreate");
// cComp->Write();
// gLidarBlue355->Write();
// blueAlpha->Write();
// greenAlpha->Write();
// gLidarGreen532->Write();
// outputFile->Write();  

}
