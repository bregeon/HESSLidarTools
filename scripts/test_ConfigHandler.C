/** @file test_ConfigHandler.C
 *
 * @brief Test the ConfigHandler class
 *
 * Creates a ConfigHandler obect and reads in a config file
 * and save data to disk.
 * 
 * @author Johan Bregeon
*/

void test_ConfigHandler()
{
LidarTools::ConfigHandler *cfg = new LidarTools::ConfigHandler(verbose=true);
cfg->Read("../data/config.cfg");
cfg->Dump();
cfg->Write("test.cfg");

std::cout<<"Get Parameter AltMin "<<std::endl;
std::cout<<cfg->GetParam("AltMin")<<std::endl;

std::cout<<"Get Parameter HMin "<<std::endl;
std::cout<<cfg->GetParamOptimizeAC_Hmin()<<std::endl;

cfg->SetParam("AtmoAbsorption","/data/Hess/work/fedora/LidarTools/data/atm_trans_1835_1_0_0_0_1835.dat");
std::cout<<cfg->GetParam("AtmoAbsorption")<<std::endl;

}
