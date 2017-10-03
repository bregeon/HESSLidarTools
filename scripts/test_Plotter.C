/** @file test_Plotter.C
 *
 * @brief Test the Plotter class
 *
 * Runs a full analysis of the Lidar data and show how to plot results
 * and save data to disk.
 * 
 * 
 * @author Johan Bregeon
*/


void test_Plotter(std::string fname="../data/run_067220_Lidar_001.root")
{

// Open file
LidarTools::LidarFile *lidar = new LidarTools::LidarFile(fname, true);
lidar->Read();

// Analyse data
LidarTools::Analyser *analyser
                    = new LidarTools::Analyser(lidar->GetRange(),
                                               lidar->GetSignalMap(),
                                               true);
analyser->SetRunNumber(lidar->GetRunNumber());
analyser->SetTime(lidar->GetTime());
analyser->SetConfig();
analyser->OverwriteConfigParam("AltMax","15000");
analyser->OverwriteConfigParam("R0","9000");
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
plotter->SaveAs("test.root");

}

