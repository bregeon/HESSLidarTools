/** @file test_Rayleigh.C
 *
 * @brief Test the RayleighScattering class
 *
 * Reads in HESS standard atmosphere and print out a few Rayleigh
 * scattering values
 * 
 * @author Johan Bregeon
*/

//#include "RayleighScattering.hh"
//#include "kaskade_cpp/Kaskade.hh"
//#include "kaskade_cpp/TabulatedAtmosphere.hh"

void test_Rayleigh()
{
 
  LidarTools::RayleighScattering *r= new LidarTools::RayleighScattering();
  r->BetaS(0.35);
  
  Kaskade::Engine engine;
  //Kaskade::TabulatedAtmosphere *atmo = new 
  Kaskade::TabulatedAtmosphere atmo(engine);
  atmo.init_atmosphere(10);
  double h=1000;
  while(h<20000){
    double p=atmo.GetPressure(h);
    double t=atmo.GetTemperature(h);
    double beta355=r->Beta(0.355, p, t);
    double beta532=r->Beta(0.532, p, t);
    std::cout<<h<<" "<<p<<" "<<t
             <<" Beta355 "<<beta355<<" Beta532 "<<beta532<<std::endl;
    h+=1000.;
    }
  
}
