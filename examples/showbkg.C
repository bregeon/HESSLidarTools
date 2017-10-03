/** @file showbkg.C
 *
 * @brief Show what the background looks like for one run
 *
 * @author Johan Bregeon
*/

#include "LidarTools/LidarFile.hh"
#include "LidarTools/Analyser.hh"
#include "LidarTools/Plotter.hh"

void showbkg(std::string fname="../data/run_067220_Lidar_001.root")
{

// Open file
LidarTools::LidarFile *lidar = new LidarTools::LidarFile(fname, true);

// Analyse data
LidarTools::Analyser *analyser
                    = new LidarTools::Analyser(lidar->GetRange(),
                                               lidar->GetSignalMap(),
                                               true);
//analyser->SetConfig("new_config.cfg");
analyser->OverwriteConfigParam("AltMax","15");
analyser->OverwriteConfigParam("R0","10");
analyser->ProcessData();

// Plot them with the Plotter class
LidarTools::Plotter *plotter = new LidarTools::Plotter(analyser, true);
plotter->InitAll();
plotter->FillAll();
plotter->DisplayAll();

// Cross-check with integrated opacity
float tau4_532=analyser->GetTau4(532);
float tau4_355=analyser->GetTau4(355);
std::cout<<"Tau4 (green = 532 nm): " <<tau4_532<< std::endl;
std::cout<<"Tau4 (green = 355 nm): " <<tau4_355<< std::endl;

// Save to disk
plotter->SaveAs("bkg.root");

TCanvas *c=new TCanvas("ShowBkgCanvas","Show background",30,50,650,680);
c->Divide(2,2);
c->cd(1);
plotter->GetRawProfile(355)->Draw("AP*");
c->cd(2);
plotter->GetRawProfile(532)->Draw("AP*");
c->cd(3);
plotter->GetBkgHist(355)->Draw();
c->cd(4);
plotter->GetBkgHist(532)->Draw();

}

