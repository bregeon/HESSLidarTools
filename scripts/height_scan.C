/** @file heigh_scan.C
 *
 * @brief Use LidarProcessor to scan the Klett R0 parameter
 *
 * @author Johan Bregeon
*/

#include <iostream>
#include "LidarTools/LidarProcessor.hh"

#include "TCanvas.h"
#include "TGraph.h"

float process(std::string filename, float h, int wl)
{
  LidarTools::LidarProcessor *p = new LidarTools::LidarProcessor(filename);
  p->OverwriteConfigParam("AltMax","15");
  std::stringstream param;
  param<<h;
  std::cout<<param.str()<<std::endl;
  p->OverwriteConfigParam("R0",param.str());
  std::stringstream nbins;
  nbins<<200; 
  p->OverwriteConfigParam("NBins",nbins.str());
  p->Process(false);
  std::stringstream fname;
  fname<<"htest_"<<wl<<"_"<<h<<".root";
  p->SaveAs(fname.str());
  return p->GetAnalyser()->GetTau4(wl);
  delete p;
}

void height_scan()
{
//std::string filename="/data/Hess/data/alldata/run_065202_Lidar_001.root.txt";
std::string filename="/sps/hess/users/lpta/bregeon/data/run065202/run_065202_Lidar_001.root";

int wl=532;
float h=0., hmin=7, hmax=14;
const int N=20;
float alpha[N], alt[N];
float step=(hmax-hmin)/N;
h=hmin;
for(int i=0; i<N; i++)
  {
  alpha[i]=process(filename, h, wl);
  alt[i]=h;
  h+=step;
  }
  
// results
TGraph *gScan = new TGraph(N);
for(int i=0; i<N; i++)
  {  
  std::cout<<alpha[i]<<std::endl;
  gScan->SetPoint(i, alt[i], alpha[i]);  
 }

TCanvas *cScan = new TCanvas("ParamScanCan", "Scanning Parameters", 30,50,850,650);
cScan->cd();
gScan->Draw("AP*");
gScan->GetXaxis()->SetTitle("R0");
gScan->GetYaxis()->SetTitle("Tau4");

}

