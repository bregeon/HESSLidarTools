/** @file RayleighScattering.hh
 *
 * @brief RayleighScattering class definition
 *
 * Class to calculate the Rayleigh scattering coefficients from Preesure and Temperature
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_RAYLEIGH
#define LIDARTOOLS_RAYLEIGH

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#endif

namespace LidarTools {

 /** @class RayleighScattering
  * 
  * @brief Estimate the Rayleigh scattering
  *
  */
  class RayleighScattering
  {
  
  public:

    /** @brief Constructor
     * 
     */
    RayleighScattering(Bool_t verbose=false);
    
    /** @brief Destructor
     * 
     */
    virtual ~RayleighScattering() {}

    // Simple getters and setters
    /** @brief Return the reference pressure 1013.25 mbar*/
    float GetStandardPressure() const    {return fPs;} 
    /** @brief Return the reference temperature 288.15 K */    
    float GetStandardTemperature() const {return fPs;}
    
    // Get pressure and temperature from the atmosphere profile
    //std::pair<float, float> GetAtmoPT(float h);
    
    // Extinction Profile
    /** @brief Volume Scattering for standard air condition
     * 
     * A.Bucholtz Applied Optics vol. 34 n. 15 p. 2769 - 20/05/1995 - Table 3
     * 
     * BetaS refers to Beta for fPs=1013.25 mbar and fTs=288.15 K
     * 
     * Rayleigh Volume-Scattering Coefficient \\beta_s in km^-1:
     * \li Verification BetaS(0.35) = 7.4473e-02 ~ 7.450e-02 in Table 2.
     * \li Verification BetaS(0.53) = 1.3352e-02 ~ 1.336e-02 in Table 2.\n
     * -->Converted to m^-1
     */    
    double BetaS(float wl);

    /** @brief Extinction value at a given wavelength, pressure and temperature */
    double Beta(float wl, float P, float T);
    //double Beta(float wl, float h);
    
  protected:
    /** @brief boolean to print some results if true */
    Bool_t fVerbose; //!

    /** @brief 1013.25 = standard pressure in mbar */
    float fPs = 1013.25;
    /** @brief 288.15  = standard temerature in K (15°C) */
    float fTs = 288.15;
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::RayleighScattering,1);
#endif

  }; // class

}; // namespace

#endif
