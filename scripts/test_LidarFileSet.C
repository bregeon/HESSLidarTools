/** @file test_LidarFile.C
 *
 * @brief Test the LidarFileSet class
 *
 * Needs to be compiled to run: 'root test_LidarFileSet.C+'
 * 
 * @author Johan Bregeon
*/

#include "LidarTools/LidarFileSet.hh"
#include "LidarTools/Analyser.hh"
#include "LidarTools/Plotter.hh"


void test_LidarFileSet()
{

// Create LidarSet from a list of run path
std::string crab="crabListSet.txt";
LidarTools::LidarFileSet *lidarSet = new LidarTools::LidarFileSet(crab, true);
lidarSet->Init();
lidarSet->MergeDataSet();

// Get data for the Analyser
TArrayF range = lidarSet->GetRange();
std::map<Int_t, TArrayF> signal=lidarSet->GetSignalMap();

// Debug
// TArrayF s532=signal[532];
// for(int i=0; i<100; i+=10){
//   std::cout<<range[i]<<std::endl;
//   std::cout<<s532[i]<<std::endl;
//   }
    
// Analyse data
LidarTools::Analyser *red = new LidarTools::Analyser(range, signal, true);
red->SetRunNumber(lidarSet->GetRunNumber());
//red->SetTime(lidar->GetTime());
red->SetConfig();
// Override config params
red->OverwriteConfigParam("LidarTheta","15");
red->OverwriteConfigParam("NBins","200");
red->OverwriteConfigParam("AltMin","800");
red->OverwriteConfigParam("AltMax","12000");
red->OverwriteConfigParam("R0","12000");

red->OverwriteConfigParam("AlgName","Fernald84");
red->OverwriteConfigParam("Fernald_Sp355","20");
red->OverwriteConfigParam("Fernald_Sp532","20");
red->OverwriteConfigParam("Fernald_sratio","1.01");
red->OverwriteConfigParam("BkgFudgeFactor","1.0");

// Process data
red->ProcessData();

// Integrated opacity
float tau4_532=red->GetTau4(532);
float tau4_355=red->GetTau4(355);
std::cout<<"Tau4 (green = 532 nm): " <<tau4_532<< std::endl;
std::cout<<"Tau4 (green = 355 nm): " <<tau4_355<< std::endl;

// Plot results
LidarTools::Plotter* fPlotter = new LidarTools::Plotter(red, true, true);
fPlotter->InitAll();
fPlotter->FillAll();
fPlotter->DisplayAll();
fPlotter->SaveAs("merged.root");

}

