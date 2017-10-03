/** @file AtmoProfile.C
 *
 * @brief AtmoProfile class implementation
 *
 * @author Johan Bregeon
*/

#include "AtmoProfile.hh"


// Constructor
LidarTools::AtmoProfile::AtmoProfile(Bool_t verbose)
: fVerbose(verbose)
{
 if(fVerbose) std::cout<<"[LidarTools::AtmoProfile] Constructor"<<std::endl;
  
  fRayleigh = new RayleighScattering();
  
}

// Reset config
void LidarTools::AtmoProfile::Reset()
{

  if(fVerbose) std::cout << "[LidarTools::AtmoProfile] Reset atmosphere" << std::endl;

  fAltitude.clear();
  fDensity.clear();
  fThickness.clear();
  fIndex.clear();
  fTemperature.clear();
  fPressure.clear();
  fPw_P.clear();
  
}

// Read config from ASCII file
void LidarTools::AtmoProfile::Read(std::string filename, bool longprof=true)
{
  if(fVerbose) std::cout << "[LidarTools::AtmoProfile] Read atmosphere from file" << std::endl;

  std::ifstream is_file(filename.c_str(), std::ifstream::in); 
  std::string line;
  
  while( std::getline(is_file, line) )
  {
    if (line.compare(0,1,"#") !=0)
      {
      std::istringstream sstream(line);
      if(longprof){
        double alt=0., rho=0., thick=0., index=0., t=0., p=0., pw_p=0.;
        sstream>>alt>>rho>>thick>>index>>t>>p>>pw_p;
        // Fill in vectors
        fAltitude.push_back(alt*1000.);
        fDensity.push_back(rho);
        fThickness.push_back(thick);
        fIndex.push_back(index);
        fTemperature.push_back(t);
        fPressure.push_back(p);
        fPw_P.push_back(pw_p);
        // and logs for convenience
        fLogAltitude.push_back(log(alt*1000.));
        fLogTemperature.push_back(log(t));
        fLogPressure.push_back(log(p));

        }
      }
    else
    {
      if(fVerbose) std::cout<<"Header or commented line: "<<line<<std::endl;
    }
  } // end of while
  // close file
  is_file.close();
 
  // for convenience -- @todo add a check on this value 
  fNProfiles=fAltitude.size();
}

// Write atmospher to an ascii file
void LidarTools::AtmoProfile::Write(std::string filename)
{
if(fVerbose) std::cout << "[LidarTools::AtmoProfile] Write profile to file" << std::endl;
std::cout << "[LidarTools::AtmoProfile] Write profile to file "
          << "--- not implemented yet" << std::endl;

}

// Dump to std::out
void LidarTools::AtmoProfile::Dump()
{
  std::cout << "[LidarTools::AtmoProfile] Dump atmosphere content"<<std::endl;
  int N=fAltitude.size();
  for (int i=0; i<N; ++i)
      std::cout<<fAltitude[i]<<" "<<fPressure[i]<<" "<<fTemperature[i]<<std::endl;

}

/* ---------------------------- pfx ----------------------------- */
/**
 *  @brief Pressure of the atmosphere as a function of altitude
 * 
 * @param height altitude in meters
 *
*/

double LidarTools::AtmoProfile::pfx(double height) const
{
  const double *p_alt   = fAltitude.data();
  const double *p_log_p = fLogPressure.data();
  int num_prof    = fNProfiles;
  return exp(rpol(p_alt,p_log_p,num_prof,height));
}
/* ---------------------------- tfx ----------------------------- */
/**
 * @brief Temperature of the atmosphere as a function of altitude
 *
 * @param height altitude in meters
*/

double LidarTools::AtmoProfile::tfx(double height) const
{
  const double *p_alt  = fAltitude.data();
  const double *p_log_t= fLogTemperature.data();
  int num_prof   = fNProfiles;
  return exp(rpol(p_alt,p_log_t,num_prof,height));
}


//Kaskade::TabulatedAtmosphere
/* --------------------------- interp ------------------------------- */
/**
 *  @brief Linear interpolation with binary search algorithm.
 *
 *  Linear interpolation between data point in sorted (i.e. monotonic
 *  ascending or descending) order. This function determines between
 *  which two data points the requested coordinate is and where between
 *  them. If the given coordinate is outside the covered range, the
 *  value for the corresponding edge is returned.
 *
 *  A binary search algorithm is used for fast interpolation.
 *
 *  @param  x Input: the requested coordinate
 *  @param  v Input: tabulated coordinates at data points
 *  @param  n Input: number of data points
 *  @param  ipl Output: the number of the data point following the requested
 *	    coordinate in the given sorting (1 <= ipl <= n-1)
 *  @param  rpl Output: the fraction (x-v[ipl-1])/(v[ipl]-v[ipl-1])
 *	    with 0 <= rpl <= 1
*/      

void LidarTools::AtmoProfile::interp ( double x, const double *v, int n, int &ipl, double &rpl ) const
{
   int i, l, m, j, lm;

#ifdef DEBUG_TEST_ALL
   if ( v == NULL || n <= 2 )
   {
      fprintf(stderr,"Invalid parameters for interpolation.\n");
      ipl = 1;
      rpl = 0.;
      return;
   }
#endif

   if ( v[0] < v[n-1] )
   {
      if (x <= v[0])
      {
	ipl = 1;
        rpl = 0.;
	return;
      }
      else if (x >= v[n-1])
	{
	  ipl = n-1;
	  rpl = 1.;
         return;
      }
      lm = 0;
   }
   else
   {
      if (x >= v[0])
      {
         ipl = 1;
         rpl = 0.;
         return;
      }
      else if (x <= v[n-1])
      {
         ipl = n-1;
         rpl = 1.;
         return;
      }
      lm = 1;
   }

   l = (n+1)/2-1;
   m = (n+1)/2;
   for (i=1; i<=30; i++ )
   {
      j = l;
      if (j < 1) j=1;
      if (j > n-1) j=n-1;
      if (x >= v[j+lm-1] && x <= v[j-lm])
      {
         ipl = j;
         if ( v[j] != v[j-1] )
            rpl = (x-v[j-1])/(v[j]-v[j-1]);
         else
            rpl = 0.5;
         return;
      }
      m = (m+1)/2;
      if (x > v[j-1])
         l = l + (1-2*lm)*m;
      else
         l = l - (1-2*lm)*m;
   }
   fprintf(stderr,"Interpolation error.\n");
}

// Kaskade::TabulatedAtmosphere
/* ----------------------------- rpol ------------------------------- */
/**
 *  @brief Linear interpolation with binary search algorithm.
 *
 *  Linear interpolation between data point in sorted (i.e. monotonic
 *  ascending or descending) order. The resulting interpolated value
 *  is returned as a return value.
 *
 *  This function calls interp() the find out where to interpolate.
 *  
 *  @param   x  Input: Coordinates for data table
 *  @param   y  Input: Corresponding values for data table
 *  @param   n  Input: Number of data points
 *  @param   xp Input: Coordinate of requested value
 *
 *  @return  Interpolated value
 *
*/

double LidarTools::AtmoProfile::rpol ( const double *x, const double *y, int n, double xp ) const
{
   int ipl;
   double rpl;

   interp ( xp, x, n, ipl, rpl );
   return y[ipl-1]*(1.-rpl) + y[ipl]*rpl;
}

/**  @brief Temperature of the atmosphere as a function of altitude
 *
*/
double LidarTools::AtmoProfile::Extinction(int wl, double height) const
{
    double p=GetPressure(height);
    double t=GetTemperature(height);
    double ray=fRayleigh->Beta(wl/1000., p, t);
  return ray;
}

ClassImp(LidarTools::AtmoProfile)
