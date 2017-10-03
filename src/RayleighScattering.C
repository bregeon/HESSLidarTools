/** @file RayleighScattering.C
 *
 * @brief RayleighScattering class implementation
 *
 * @author Johan Bregeon
*/

#include <iostream> 
#include <cmath>

#include "RayleighScattering.hh"

// Initialize standard pressure and temperature
LidarTools::RayleighScattering::RayleighScattering(bool verbose):
fVerbose(verbose)
{
if(verbose)std::cout<<"[LidarTools::RayleighScattering] Constructor"<<std::endl;
}


// Volume Scattering for standard air condition
// A.Bucholtz Applied Optics vol. 34 n. 15 p. 2769 - 20/05/1995 - Table 3
double LidarTools::RayleighScattering::BetaS(float wl)
{
  //std::cout<<"Calculating Reference Extinction for wl "<<wl<<std::endl;
  
  double A=0., B=0., C=0., D=0.;
  
  if (wl>0.2 && wl<=0.5)
    {
      A = 7.68246*pow(10,-4);
      B = 3.55212;
      C = 1.35579;
      D = 0.11563;
    }
  else if (wl>0.5 && wl<2.2)
    {
      A = 10.21675*pow(10,-4);
      B = 3.99668;
      C = 1.10298*pow(10,-3);
      D = 2.71393*pow(10,-2);
    }
  else
    {
      std::cout<<"Wave length not in range 0.2 - 2.2 um"<<std::endl;
      return -1;
    }
  double betaS = A * pow( wl, -(B + C*wl + D/wl) ) / 1000.;
  return betaS;
}

// Volume Scattering for a given Wavelength, Pressure and Temperature
//
double LidarTools::RayleighScattering::Beta(float wl, float P, float T)
{
  double betaS = BetaS(wl);
  double beta = betaS * (P/fPs) * (fTs/T);
  return beta;  
}

/* Volume Scattering for a fiven wavelength and height
// assumes an atmosphere has been given in input
//
double LidarTools::RayleighScattering::Beta(float wl, float h)
{  
  std::pair<float, float> PT = GetAtmoPT(h);
  
  return Beta(wl, PT.first, PT.second);
}

// Get Pressure and Temperature for a given height
std::pair<float, float> LidarTools::RayleighScattering::GetAtmoPT(float h)
{
  float P=1013.15;
  float T=288.15;
  
  return std::pair<float, float>(P,T);
}
*/

/** @brief Mark class implementation for ROOT */
ClassImp(LidarTools::RayleighScattering);
