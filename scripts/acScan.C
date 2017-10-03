/** @file acScan.C
 *
 * @brief Scan AC param to show effect
 * 
 * @author Johan Bregeon
*/


//void test_Processor(Int_t run=67220, bool verbose=false)
 void acScan(std::string run, int ac=0)
{

LidarTools::LidarProcessor *p = new LidarTools::LidarProcessor(run, true);
if(!p->Init())
 { 
 gROOT->SetBatch(1);
  p->OverwriteConfigParam("LidarTheta","15");
  // 300 bins, min width = 7 m, for 100 bins min width =22 m, original binning is 2.3 m
  p->OverwriteConfigParam("NBins","100");
  p->OverwriteConfigParam("LogBins","0");
  p->OverwriteConfigParam("SGFilter","0");
  p->OverwriteConfigParam("AltMin","800");
  p->OverwriteConfigParam("AltMax","14000");
  p->OverwriteConfigParam("R0_355","12000");
  p->OverwriteConfigParam("R0_532","12000");
  p->OverwriteConfigParam("AlgName","Fernald84");
  p->OverwriteConfigParam("Fernald_Sp355","20");
  p->OverwriteConfigParam("Fernald_Sp532","20");
  

  // Optimize inversion
  p->OverwriteConfigParam("BkgFudgeFactor","1.01"); // drift is around 2% max most of the time
  p->OverwriteConfigParam("SNRatioThreshold","5");
  p->OverwriteConfigParam("OptimizeR0","1");
  p->OverwriteConfigParam("Fernald_sratio","1.01");  
  p->OverwriteConfigParam("OptimizeAC","0");
  p->OverwriteConfigParam("OptimizeAC_Hmin","7000");
  char acs[12];
  sprintf(acs, "%.2f", ac/100.);
  p->OverwriteConfigParam("AlignCorr_355",acs);
  p->OverwriteConfigParam("AlignCorr_532",acs);

  int rc=p->Process(true, true);
  TString outname="scan_ac_";
  outname+=int(ac*100);
  outname+=".root";
  if(rc==0)
      p->SaveAs(outname.Data());
  delete p;       
  }
else
  cout<<"File corrupted."<<endl;  

}
