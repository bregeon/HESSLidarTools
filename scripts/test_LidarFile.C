/** @file test_LidarFile.C
 *
 * @brief Test the LidarFile class
 *
 * Open Lidar data files in different ways (text file, from run number,
 *  ROOT file) and run a standard data analysis with the Analyser.
 * and save data to disk.
 * 
 * Needs to be compiled to run: 'root test_LidarFile.C+'
 * 
 * @author Johan Bregeon
*/

#include "LidarTools/Analyser.hh"
#include "LidarTools/LidarFile.hh"

int run_test(LidarTools::LidarFile *lidar)
{
TArrayF range = lidar->GetRange();
std::map<Int_t, TArrayF> signal=lidar->GetSignalMap();  
// Analyse data
LidarTools::Analyser *red = new LidarTools::Analyser(range, signal, true);
red->SetRunNumber(lidar->GetRunNumber());
red->SetTime(lidar->GetTime());
red->SetConfig();

red->ProcessData();

// Integrated opacity
float tau4_532=red->GetTau4(532);
float tau4_355=red->GetTau4(355);
std::cout<<"Tau4 (green = 532 nm): " <<tau4_532<< std::endl;
std::cout<<"Tau4 (green = 355 nm): " <<tau4_355<< std::endl;
return 0;
}

void test_LidarFile()
{

std::cout<<"First test with text file"<<std::endl;
LidarTools::LidarFile *lidar1 = new LidarTools::LidarFile("../data/run_065202_Lidar_001.root.txt", true);
lidar1->Read();
std::cout<<"Analysing run " <<lidar1->GetRunNumber()<< std::endl;
run_test(lidar1);
std::cout<<"------End of test------- \n\n\n"<<std::endl;

std::cout<<"Second test with run number"<<std::endl;
LidarTools::LidarFile *lidar2 = new LidarTools::LidarFile(67217, true);
lidar2->Read();
std::cout<<"Analysing run " <<lidar2->GetRunNumber()<< std::endl;
run_test(lidar2);
std::cout<<"------End of test------- \n\n\n"<<std::endl;

std::cout<<"Third test with ROOT file (Read test)"<<std::endl;
LidarTools::LidarFile *lidar3 = new LidarTools::LidarFile("../data/run_067220_Lidar_001.root", true);
lidar3->Read();
std::cout<<"Analysing run " <<lidar1->GetRunNumber()<< std::endl;
run_test(lidar3);
std::cout<<"------End of test------- \n\n\n"<<std::endl;
}

