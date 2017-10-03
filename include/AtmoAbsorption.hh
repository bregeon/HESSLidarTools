/** @file AtmoAbsorption.hh
 *
 * @brief AtmoAbsorption class definition.
 *
 * Class to handle atmospheric transmission table
 *
 * @author Johan Bregeon
 *  
*/

#ifndef LidarTools_ABSORB
#define LidarTools_ABSORB

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#endif

#include <vector>
#include <mathutils/Table.hh>
#include <mathutils/LinearTable2.hh>
#include <mathutils/LinearTableTable.hh>

/** @namespace LidarTools
 *
 * @brief LidarTools package namespace
 * 
*/
namespace LidarTools {
	
/** @class AtmoAbsorption
 * 
 * Amount of direct atmospheric absorption and scattering (i.e. excluding 
 * multiple scattering) for Cherenkov light from any given height.
 * A plane-parallel atmosphere is assumed.
 *
 * @author Johan Bregeon, code stolen from Konrad Bernloehr (1997)
 *
 * @ingroup LidarTools
 * 
*/
  class AtmoAbsorption
  {
  
  public:

    /** @brief class constructor.
     *
     * The AtmoAbsorption constructor needs a file name and the observatory altitude.
     * 
     * @param setup_trans_fname transmission table file name
     * @param tel_altitude telescope altitude in meters above sea level
     * @param dump dump table into std::out if true
     */   
    AtmoAbsorption(const char *setup_trans_fname, float tel_altitude, bool dump = false);

    /** @brief class destructor
     *
     * @todo delete pointers
    */
    virtual ~AtmoAbsorption() {}

    /** @brief Initialize object from file name
     * 
     * Read file and parse table
     *
     * @param setup_trans_fname transmission table file name
     * @param tel_altitude telescope altitude in meters above sea level
     * @param dump dump table into std::out if true
    */
    void Init(const char *setup_trans_fname, float tel_altitude, bool dump = false);

   /** @brief  Find out direct atmospheric transmission probability between
    *    point of emission and point of detection.
    *
    * The result is interpolated over altitudes (not over wavelengthes)
    *
    * @param wl a given wave length
    * @param zem Altitude of emission (from sea level)
    * @param coszen cosine of the zenith angle
    */
    float Transmission (float wl, float zem, float coszen) const;

   /**  @brief Estimate out direct atmospheric extinction at a given altitude 
    *    and for a given wavelength.
    *
    * The result is interpolated over altitudes (not over wavelengths)
    *
    * @param wl Wavelength (nm)
    * @param zem Altitude (in meters above sea level)
    * @param coszen Cosine of the Zenith Angle
    */
    float Extinction (float wl, float zem, float coszen) const;
       
    /** @brief  Simple get method to retrieve the opacity profile
     *    for a given wavelength.
     *
     * @param wl Wavelength (nm)
     *
     */
    MathUtils::TableF GetTauProfWL(float wl) const;

    /** @brief dump table content to tstream  */
    void Dump(std::ostream &) const;
    
  protected:
    
    /** @brief Observatory altitude [m] */
    float fObsAltitude; 
    /** @brief Log of altitudes, in meters */
    std::vector<float>   fLogH1m;
    /** @brief Wavelengthes, in naonmeters */
    std::vector<int>     fWl;
    /** @brief Transmission Coefficients */
    MathUtils::LinearTableTable<MathUtils::TableF> fTrans;
    /** @brief number of wave lengths */
    int nwl;
    /** @brief dump table content if true */
    bool ok;
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::AtmoAbsorption,1);
#endif
  };  
};


#endif
