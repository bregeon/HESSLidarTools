/** @file AtmoProfile.hh
 *
 * @brief AtmoProfile class definition
 *
 * Class to store/read/write an atmospheric density profile,
 * in particular P and T profiles
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_AtmoProfile
#define LIDARTOOLS_AtmoProfile

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#endif

#include  "RayleighScattering.hh"

#include <map>
#include <cstdlib>      // atof
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream
#include <cstring>      // strcmp
#include <cmath>

namespace LidarTools {
	
/** @class AtmoProfile
 * 
 * @brief  Store/read/write an atmospheric density profile,
 * in particular P and T profiles
 * 
 * @see Analyser
 * 
*/
  class AtmoProfile
  {
  
  public:
    /** @brief class constructor
     *
     * The AtmoProfile constructor takes a bool for verbosity
     * and that's it.
     * 
     * The initial configuration is the default one and
     * can be reinitialized via the Reset method. 
     * 
     * Configuration files are handle through the Read
     * and Write methods
     * 
     * @param verbose a boolean for verbosity
     */
    AtmoProfile(Bool_t verbose=false);

    /** @brief class destructor
     *
    */
    virtual ~AtmoProfile() {}

    /** @brief Reset the configuration to default values
     *
    */
    void Reset();

    /** @brief Read a configuration from an ascii file
     *  and store it in the configuration map
     *
     *  @param filename the atmosphere file name
     *  @param longprof a bool to tell the reader whether a full profile
     *  description is available
    */
    void Read(std::string, bool);
    
    /** @brief Write the configuration map to an ascii file
     *  
     * @param filename the text file name
    */
    void Write(std::string);

    /** @brief Dump current configuration to std out */
    void Dump();

    /** @brief Set verbosity on or off
     *
     * @param verbose a bool, true or false
     */
    void SetVerbose(Bool_t verbose) {fVerbose=verbose;}

    // Interpolation -- Kaskade::TabulatedAtmosphere
    void interp ( double x, const double *v, int n, int &ipl, double &rpl ) const;
    double rpol ( const double *x, const double *y, int n, double xp ) const;

    // Internal -- Kaskade::TabulatedAtmosphere
    double pfx   (double height) const;
    double tfx   (double height) const;

    // API -- Kaskade::TabulatedAtmosphere
    /** @brief Returns the pressure for the given altitude in meters */    
    double GetPressure(double z /*in meters*/) const
    {return pfx(z); }
    /** @brief Returns the temperature for the given altitude in meters */        
    double GetTemperature(double z /* in meters*/) const
    {return tfx(z);}

    /** @brief Get Rayleigh scattering extinction at height for a given wl
     * 
     * @param wl the wavelenght
     * @param height the altitude in meters
     *  */
    double Extinction(int, double) const;
    
  private:
    /** @brief boolean to print some results if true */
    Bool_t fVerbose;
    
    /** @brief Altitude in meters */
    std::vector<double> fAltitude;
    /** @brief Density [g/cm^3]*/
    std::vector<double> fDensity;
    /** @brief Thickness [g/cm^2]  */
    std::vector<double> fThickness;
    /** @brief n-1 */
    std::vector<double> fIndex;
    /** @brief Temperature in K*/
    std::vector<double> fTemperature;
    /** @brief Pressure in mbar*/
    std::vector<double> fPressure;
    /** @brief pw/p */
    std::vector<double> fPw_P;
    /** @brief Number of profiles */
    int fNProfiles;
           
    /** @brief Log Altitude in meters */
    std::vector<double> fLogAltitude;
    /** @brief Log Temperature in K*/
    std::vector<double> fLogTemperature;
    /** @brief Log Pressure in mbar*/
    std::vector<double> fLogPressure;
    
    /** @brief Rayleigh scattering calculator */
    RayleighScattering* fRayleigh;
    
  protected:
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::AtmoProfile,1);
#endif

  }; // class

}; // namespace

#endif
