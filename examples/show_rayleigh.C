/** @file show_rayleigh.C
 *
 * @brief Show the Rayleigh scattering calibration and contribution 
 * on real data
 *
 * @author Johan Bregeon
*/

#include "TSystem.h"
#include "LidarTools/LidarProcessor.hh"
#include "LidarTools/Analyser.hh"

#include "kaskade_cpp/AtmoPlotter.hh"

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

void show_rayleigh(Int_t run=65202)
{
LidarTools::LidarProcessor *p = new LidarTools::LidarProcessor(run, true);
p->OverwriteConfigParam("AltMin","0.1");
p->OverwriteConfigParam("AltMax","18");
p->OverwriteConfigParam("R0","15");
p->Process(true);
LidarTools::Plotter* lidarplotter=p->GetPlotter();


// Rayleigh
Kaskade::AtmoPlotter *plotter = new Kaskade::AtmoPlotter();

TString absName=gSystem->ExpandPathName("${HESSROOT}/kaskade_cpp/dat/atm_trans_1800_1_10_0_0_1800.dat");
plotter->InitAbsorption(absName);
  
int blue=355, green=532;

TGraph *blueTau=plotter->FillTauGraph(blue);
TGraph *blueAlpha=plotter->GetAlphaGraph(blue);

TGraph *greenTau=plotter->FillTauGraph(green);
TGraph *greenAlpha=plotter->GetAlphaGraph(green);

// plot
TCanvas *ac=new TCanvas("DataSimCan","Data vs Simulation",30,50,950,650);
ac->Divide(2);
ac->cd(1);
TGraph* opg=RenormLidarProf(lidarplotter->GetOpacityProfile(green));
opg->SetMarkerColor(kGreen);
greenAlpha->Draw("APL");
opg->Draw("PS*");

ac->cd(2);
TGraph* opb=RenormLidarProf(lidarplotter->GetOpacityProfile(blue));
opb->SetMarkerColor(kBlue);
blueAlpha->Draw("APL");
opb->Draw("PS*");
}
