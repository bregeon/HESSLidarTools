/** @file test_Processor.C
 *
 * @brief Test the LidarProcessor class
 *
 * Runs a full analysis of the Lidar data and show how to overwrite
 * on the fly data analysis parameters.
 * 
 * @author Johan Bregeon
*/

/* Crab Runs for ref
bad   = [78815, 78922, 79862, 80081, 80086, 80124, 80129]
medium= [79556, 79858, 79859, 79860, 79884, 79885, 80026]
good  = [79881, 79882, 80154]
*/

//void test_Processor(Int_t run=67220, bool verbose=false)
void test_Processor(std::string run, std::string Sp355="20",
                                     std::string Sp532="20",  bool verbose=false)
{
LidarTools::LidarProcessor *p = new LidarTools::LidarProcessor(run, verbose);
if(!p->Init())
 { 

//   p->OverwriteConfigParam("LidarTheta","45");
//   p->OverwriteConfigParam("BkgMin","15000");
//   p->OverwriteConfigParam("BkgMax","20000");

  p->OverwriteConfigParam("LidarTheta","15");
  // 300 bins, min width = 7 m, for 100 bins min width =22 m, original binning is 2.3 m
  p->OverwriteConfigParam("NBins","100");
  p->OverwriteConfigParam("LogBins","0");
  p->OverwriteConfigParam("SGFilter","1");
  p->OverwriteConfigParam("AltMin","400");
  p->OverwriteConfigParam("AltMax","12000");
  p->OverwriteConfigParam("TauAltMin","400");
  p->OverwriteConfigParam("TauAltMax","8200");
  p->OverwriteConfigParam("R0_355","8200");
  p->OverwriteConfigParam("R0_532","8200");
//  p->OverwriteConfigParam("OverlapFunction","");

//  p->OverwriteConfigParam("AlgName","Klett");
//  p->OverwriteConfigParam("Klett_k","1");
  p->OverwriteConfigParam("AlgName","Fernald84");
//  p->OverwriteConfigParam("AlgName","Aeronet");
  p->OverwriteConfigParam("Fernald_Sp355",Sp355);
  p->OverwriteConfigParam("Fernald_Sp532",Sp532);
  

  // Optimize inversion
  p->OverwriteConfigParam("BkgFudgeFactor","1.01"); // drift is around 2% max most of the time
  p->OverwriteConfigParam("SNRatioThreshold","3");
  p->OverwriteConfigParam("OptimizeR0","1");
  p->OverwriteConfigParam("Fernald_sratio","1.01");  
  p->OverwriteConfigParam("OptimizeAC","1");
  p->OverwriteConfigParam("OptimizeAC_Hmin","6000");
  p->OverwriteConfigParam("AlignCorr_355","0.0");
  p->OverwriteConfigParam("AlignCorr_532","0.0");

//  p->OverwriteConfigParam("LidarAltitude","1835");
//  p->OverwriteConfigParam("AtmoAbsorption","/data/Hess/work/fedora/LidarTools/data/atm_trans_1835_1_0_0_0_1835.dat");


  int rc=p->Process(true, true);
  if(rc==0)
      p->SaveAs("test.root");
  }
else
  cout<<"File corrupted."<<endl;

}
