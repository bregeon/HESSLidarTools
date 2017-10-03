/** @file AtmoPlotter.hh
 *
 * @brief AtmoPlotter class definition.
 *
 * Class to plot atmosphere profiles
 *
 * @author Johan Bregeon
 *  
*/

#ifndef LidarTools_ATMOPLOT
#define LidarTools_ATMOPLOT

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#endif

#include "AtmoAbsorption.hh"
#include "AtmoProfile.hh"

/** @todo Plotter for P,T profiles
*/
/** @namespace LidarTools
 *
 * @brief LidarTools package namespace
 * 
*/
	
namespace LidarTools {
/** @class AtmoPlotter
 * 
 * @brief Plot atmosphere composition and transparency profiles
 * 
*/
  class AtmoPlotter
  {
  
  public:
    /** @brief class constructor.
     *
     * The AtmoPlotter constructor takes only the observatory altitude.
     * 
     * @param tel_altitude telescope altitude in meters above sea level
     * @param verbose a bool for verbosity
     */ 
    AtmoPlotter(float tel_altitude=1800., Bool_t verbose=false);

    /** @brief class destructor
     * 
     * @todo delete pointer members
     * @see fAbsorp fAtmoProf
     */
    virtual ~AtmoPlotter() {}

    /** @brief Initialize absorption
     * 
     * Creates a pointer to an AtmoAbsorption object
     * and assign it to member fAbsorp
     *      
     * @param filename the atmospherice absorption file name
     */
    void InitAbsorption(const char *filename = "../dat/atm_trans_1800_1_10_0_0_1800.dat");

    /** @brief Initialize atmosphere density profile
     * 
     * Creates a pointer to an AtmoProfile object
     * and assign it to member fAtmoProf
     *      
     * @param filename the atmospheric density profile file name
     * @param longprof set to true if reading a detailed profile
     */
    void InitAtmoProf(std::string, bool);
    
    /** @brief Fill integrated opacity (Tau) TGraph
     * 
     * @param wl the wave length
     */    
    void FillTauGraph(int);

    /** @brief Fill the exctinction (alpha) TGraph
     * 
     * @param wl the wave length
     */    
    void FillAlphaGraph(int);

    /** @brief Simple getter for member fAtmoProf */    
    AtmoProfile* GetAtmoProf(){return fAtmoProf;}

    /** @brief Get the opacity TGraph for the given wave length
     * 
     * @param wl the wave length
     */    
    TGraph* GetTauGraph(int wl)          {return fMapWLTau[wl];}

    /** @brief Get the extinction TGraph for the given wave length
     * 
     * @param wl the wave length
     */    
    TGraph* GetAlphaGraph(int wl)        {return fMapWLAlpha[wl];}
    
  protected:
    /** @brief boolean to print some results if true */
    Bool_t fVerbose;

    /** @brief the observatory altitude in meters above sea level */
    float fObsAltitude;         // Observatory altitude [m]

    /** @brief Pointer to the atmospheric absorption object */
    AtmoAbsorption* fAbsorp;
    /** @brief Pointer to the atmospheric density profile object */
    AtmoProfile* fAtmoProf;
    
    /** @brief Map of opacity TGraph for each wave length */    
    std::map<int, TGraph*> fMapWLTau;
    /** @brief Map of extinction TGraph for each wave length */    
    std::map<int, TGraph*> fMapWLAlpha;
    
    
    

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::AtmoPlotter,1);
#endif

  };
}; // end of namespace

#endif
