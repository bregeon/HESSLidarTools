/** @file test_AtmoProfile.C
 *
 * @brief Test the AtmoProfile class
 *
 * Creates a AtmoProfile obect and reads in a config file
 * and save data to disk.
 * 
 * @author Johan Bregeon
*/

//#include "LidarTools/AtmoProfile.hh"

void test_AtmoProfile()
{
LidarTools::AtmoProfile *atmo = new LidarTools::AtmoProfile(true);
atmo->Read("../data/atmprof10.dat", true);
atmo->Dump();

std::cout<<"Pressure profile interpolation at 3500 m"<<std::endl;
std::cout<<atmo->GetPressure(3000)<<" "
         <<atmo->GetPressure(3500)<<" "
         <<atmo->GetPressure(4000)<<std::endl;

std::cout<<"Temperature profile interpolation at 5500 m"<<std::endl;
std::cout<<atmo->GetTemperature(5000)<<" "
         <<atmo->GetTemperature(5500)<<" "
         <<atmo->GetTemperature(6000)<<std::endl;
}
