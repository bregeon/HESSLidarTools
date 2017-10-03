/** @file AtmoPlotter.C
 *
 * @brief AtmoPlotter class implementation
 *
 * @author Johan Bregeon
*/

#include <fstream>
#include <iostream> 
#include <iomanip>
#include <sstream> 
#include <stdexcept>

#include "AtmoPlotter.hh"


LidarTools::AtmoPlotter::AtmoPlotter(float tel_altitude, Bool_t verbose)
: fVerbose(verbose), fObsAltitude(tel_altitude), fAbsorp(0), fAtmoProf(0)
{
std::cout<<"Initialize AtmoPlotter"<<std::endl;
}

// Atmosphere Transmission
void LidarTools::AtmoPlotter::InitAbsorption(const char *filename)
{
  // Read input file - Altitude has to match file content - 1800 m
  fAbsorp = new LidarTools::AtmoAbsorption(filename, fObsAltitude, true);
}

// Atmospheric Density Profile
void LidarTools::AtmoPlotter::InitAtmoProf(std::string filename, bool longprof)
{
  fAtmoProf = new LidarTools::AtmoProfile();
  fAtmoProf->Read(filename, longprof);
}


void LidarTools::AtmoPlotter::FillTauGraph(int wl)
{

  // Get Tau profiles
  std::cout<<"Read Profile for wl "<<wl<<std::endl;
  MathUtils::TableF tauProf = fAbsorp->GetTauProfWL(wl);

  // Define Optical Depth Tau TGraph
  TGraph *gTau = new TGraph();
  TString name="gTau_";
  name+=wl;
  gTau->SetName(name);
  name="Optical Depth Profile (";
  name+=wl;
  name+=" nm)";
  gTau->SetTitle(name);
  gTau->GetXaxis()->SetTitle("Optical depth from 0 to h");
  gTau->GetYaxis()->SetTitle("Altitude (sea level in m)");
  if(wl==355)
    gTau->SetMarkerColor(kBlue);
  else if (wl==532)
    gTau->SetMarkerColor(kGreen);
  
  // Fill TGraph
  int i=0;
  std::map<float, float>::const_iterator it;
  for (it = tauProf.begin(); it != tauProf.end(); ++it) {
    //std::cout<<i<<" "<< it->second<<" "<<pow(10,it->first)<<std::endl;
    gTau->SetPoint(i, it->second, pow(10,it->first));
    i++;
    }
  
  fMapWLTau.insert( std::pair<int, TGraph*>(wl,gTau) );
}

void LidarTools::AtmoPlotter::FillAlphaGraph(int wl)
{
  // Define Optical Depth Tau TGraph
  TGraph *gAlpha = new TGraph();
  TString name="gAlpha_";
  name+=wl;
  gAlpha->SetName(name);
  name="Extinction Profile (";
  name+=wl;
  name+=" nm)";
  gAlpha->SetTitle(name);
  gAlpha->GetXaxis()->SetTitle("Extinction (/1m)");
  gAlpha->GetYaxis()->SetTitle("Altitude (sea level in m)");
  if(wl==355)
    gAlpha->SetMarkerColor(kBlue);
  else if (wl==532)
    gAlpha->SetMarkerColor(kGreen);
  
  //  Fill Alpha using interpolation in HESSAtmosphericAbsorption::Extinction
  //  Feature at 5 km can be smoothed by taking a larger distance in the
  //  derivative
  
  int i=0;
  
  for(float logalt=3.3; logalt<=4.4; logalt+=0.01)
    {
      float alt=pow(10, logalt);
      float alpha=fAbsorp->Extinction(wl, alt, 1.);
      gAlpha->SetPoint(i, alpha, alt);
      i++;
    }

  fMapWLAlpha.insert( std::pair<int, TGraph*>(wl,gAlpha) );
}

