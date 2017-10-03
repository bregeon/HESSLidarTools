/** @file AtmoAbsorption.C
 *
 * @brief AtmoAbsorption class implementation
 *
 * @author Johan Bregeon
*/

#include <fstream>
#include <iostream> 
#include <iomanip>
#include <sstream> 
#include <stdexcept>
#include "AtmoAbsorption.hh"



// Constructor
LidarTools::AtmoAbsorption::AtmoAbsorption(const char *setup_trans_fname, 
							      float tel_altitude, bool dump)
  : fWl(0),
    nwl(0),
    ok(false)
{
  Init(setup_trans_fname,tel_altitude);
}

// Reads the  table of atmospheric transmission data.
void LidarTools::AtmoAbsorption::Init(const char *setup_trans_fname, float altitude , bool dump)
{
  fLogH1m.clear();
  fWl.clear();
  //fTrans.clear();

  std::string line;
  std::istringstream s;
  int iwl; 
     
  std::ifstream file(setup_trans_fname);
  if(! file) 
    throw std::runtime_error((std::string("Could not open ") + setup_trans_fname).c_str());
   
  if(dump)std::cout << "Read atmospheric transmission data from file " << setup_trans_fname << std::endl;
  do {
    std::getline(file,line);
  } while(line.substr(0,5) != "# H2=" && file.good());

  if ( line.substr(0,5) != "# H2=")
    throw std::runtime_error((std::string("Invalid first line in ") + setup_trans_fname).c_str());
  line = line.substr(5);
  s.clear();
  s.str(line);
  s >> fObsAltitude; 
  fObsAltitude *= 1000.; // Convert into [m]
  if ( fabs(fObsAltitude - altitude) > 25.f )
    {
      std::ostringstream e;
      e << setup_trans_fname << ": altitude requested " << altitude
	<< " does not match that available " << fObsAltitude << std::endl;
      throw std::runtime_error(e.str().c_str());
    }
  std::string::size_type pos = line.find("H1=");
  if(pos == std::string::npos)
    throw std::runtime_error((std::string("Invalid first line in ") + setup_trans_fname).c_str());
  line = line.substr(pos+4);
  fLogH1m.push_back(log10(fObsAltitude));

  s.clear();
  s.str(line);
  while(s) {
    float f;
    s >> f;
    if(s) fLogH1m.push_back(log10(f*1e3f));
  }

  // 500 bins from 0 to 500
  fTrans.Init(500,0.,500.);
  for (iwl=0; iwl<500 && std::getline(file,line) ; ) 
    {
      int i;
      float f;
      if(line[0] == '#') continue;
      s.clear();
      s.str(line);
      s >> i;
      fWl.push_back(i);
      //fTrans.resize(fTrans.size()+1);
      for(std::vector<float>::const_iterator it = fLogH1m.begin() + 1;
	  it != fLogH1m.end() && s; ++it) {
	s >> f;
	fTrans[iwl].Insert(*it,f);
      }
      if(!s) std::runtime_error("NOT ENOUGH DATA");
      iwl++;
    }  
  if ( iwl == 0 ) 
    throw std::runtime_error((std::string("No transmission data available in ") + setup_trans_fname).c_str());
  if ( fWl[iwl-1]-fWl[0] != iwl-1 || fWl[iwl-1] > 1000 )
    throw std::runtime_error((std::string("Transmission data not at nanometer intervals in ") + setup_trans_fname).c_str());
  
  nwl = iwl; 
  if(dump)std::cout << "Got " << nwl << " wavelength intervals for " << fLogH1m.size() << " heights starting at " << fObsAltitude/1000. << " km" << std::endl;
  ok = true;
  
  //close file ?
  file.close();
  
  if(dump) {
    std::ofstream lfile("absor.cpp");
    Dump(lfile);
  }
}


// Return the transmission
float LidarTools::AtmoAbsorption::Transmission (float wl, float zem /*[m]*/, float coszen) const
{
   float optical_depth;
   if ( !ok )
     throw std::runtime_error("Atmsopheric absorption not initialized");
   int iwl = (int)rint(wl - fWl[0]);
   if ( zem < fObsAltitude || zem <= 0. )
     return 0.;
   if(iwl < 0) iwl = 0;
   if(iwl >= nwl) iwl = nwl-1;
   
   optical_depth = fTrans[iwl](log10(zem))/coszen;
   
   if ( optical_depth > 100. )
     return 0.;
   else
     return exp(-1.*optical_depth);
}

void LidarTools::AtmoAbsorption::Dump(std::ostream &os) const
{
  float z = 100;
  do {
    os <<  z + fObsAltitude 
       << " " << Transmission(300,z + fObsAltitude,1) 
       << " " << Transmission(500,z + fObsAltitude,1) 
       << " " << Transmission(700,z + fObsAltitude,1) 
       << std::endl;
    z += 100;
  } while(z < 50000);
}

// Simple get method to retrieve the opacity profile for a given wavelength.
MathUtils::TableF LidarTools::AtmoAbsorption::GetTauProfWL (float wl) const
{
   // no interpolation, just take the nearest wavelength
   if ( !ok )
     throw std::runtime_error("Atmsopheric absorption not initialized");
   int iwl = (int)rint(wl - fWl[0]);
   if(iwl < 0) iwl = 0;
   if(iwl >= nwl) iwl = nwl-1;

   return fTrans[iwl];
}


// Extinction
float LidarTools::AtmoAbsorption::Extinction (float wl, float zem /*[m]*/, float coszen) const
{
   /* Alpha is the extinction in the Klett inversion algorithm (m^-1)  */
   float alpha=0.;
   if ( !ok )
     throw std::runtime_error("Atmsopheric absorption not initialized");
   int iwl = (int)rint(wl - fWl[0]);
   if ( zem < fObsAltitude || zem <= 0. )
     return 0.;
   if(iwl < 0) iwl = 0;
   if(iwl >= nwl) iwl = nwl-1;
   
   float zem_p=0., zem_m=0.;
   /* @TODO think better how to estimate the derivative : feature at 5 km */   
   zem_p=zem/coszen*1.1;
   zem_m=zem/coszen*0.9;
   alpha = ( fTrans[iwl](log10(zem_p))-fTrans[iwl](log10(zem_m)) ) / (zem_p-zem_m);

   return alpha;
}
