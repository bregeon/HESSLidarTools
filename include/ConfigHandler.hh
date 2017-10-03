/** @file ConfigHandler.hh
 *
 * @brief ConfigHandler class definition
 *
 * Class to store/read/write a Lidar data analysis configuration
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_CONFIGHANDLER
#define LIDARTOOLS_CONFIGHANDLER

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#include <TFile.h>
#endif

#include <map>
#include <cstdlib>      // atof

namespace LidarTools {
	
/** @class ConfigHandler
 * 
 * @brief Manage a configuration for the Analyser
 * 
 * @see Analyser
 * 
*/
  class ConfigHandler
  {
  
  public:
    /** @brief class constructor
     *
     * The ConfigHandler constructor takes a bool for verbosity
     * and that's it.
     * 
     * The initial configuration is the default one and
     * can be reinitialized via the Reset method. 
     * 
     * Configuration files are handle through the Read
     * and Write methods
     */
    ConfigHandler(Bool_t verbose=false);

    /** @brief class destructor
     *
    */
    virtual ~ConfigHandler() {}

    /** @brief Reset the configuration to default values
     *
    */
    void Reset();

    /** @brief Read a configuration from an ascii file
     *  and store it in the configuration map
    */
    void Read(std::string);
    
    /** @brief Write the configuration map to an ascii file
     *  
     * @param filename the text file name
    */
    void Write(std::string);

    /** @brief Dump current configuration to std out
     * @todo to be implemented
    */
    void Dump();

    /** @brief Set verbosity on or off
     *
     * @param verbose a bool, true or false
     */
    void SetVerbose(Bool_t verbose) {fVerbose=verbose;}

   // Public methods for config map
    /** @brief Get a string parameter from the current configuration
     *  
     * @param key the parameter name as a string
     * @return std::string
    */
    std::string GetParam(std::string key) {return fConfig[key];}

    /** @brief Get a float parameter from the current configuration
     *  
     * @param key the parameter name as a string
     * @return Float_t
    */
    Float_t GetParamF(std::string key)    {return atof(GetParam(key).c_str());}
 
     /** @brief Get an integer parameter from the current configuration
     *  
     * @param key the parameter name as a string
     * @return Int_t
    */
    Int_t GetParamI(std::string key)    {return atoi(GetParam(key).c_str());}
    
    /** @brief Set a parameter of the current configuration
     *  
     * @param key the parameter name as a string
     * @param value the parameter value as a string
    */
    void SetParam(std::string key, std::string value) {fConfig[key]=value;}

    // Getters all altitudes in meters and above the LidarAltitude, except itself
   /** @brief Returns the Lidar altitude in meters
    *  
    * @return Float_t
    */
    Float_t GetLidarAltitude()         {return GetParamF("LidarAltitude");}


   /** @brief Returns the Lidar inclination in degrees
    *  
    * @return Float_t
    */
    Float_t GetLidarTheta()         {return GetParamF("LidarTheta");}

   /** @brief Returns the quality threshold of the current configuration
     *  
     * @return Float_t
    */
    Float_t GetQualityThr()          {return GetParamF("QualityThr");}

   /** @brief Returns the minimum altitude of the current configuration
    *  
    * @return Float_t
    */
    Float_t GetParamAltMin()         {return GetParamF("AltMin");}

   /** @brief Returns the maximum altitude of the current configuration
    * 
    *  
    * @return Float_t
    */
    Float_t GetParamAltMax()         {return GetParamF("AltMax");}

   /** @brief Returns the minimum altitude for the background estimation
    * of the current configuration
    * 
    *  
    * @return Float_t
    */
    Float_t GetParamBkgMin()         {return GetParamF("BkgMin");}

   /** @brief Returns the maximum altitude for the background estimation
    * of the current configuration
    * 
    *  
    * @return Float_t
    */
    Float_t GetParamBkgMax()         {return GetParamF("BkgMax");}

   /** @brief Returns the background fudge factor
    * of the current configuration
    * 
    *  
    * @return Float_t
    */
    Float_t GetParamBkgFFactor()         {return GetParamF("BkgFudgeFactor");}

   /** @brief Returns the number of bins in the current configuration
    * 
    *  
    * @return Int_t
    */
    UInt_t  GetParamNBins()          {return GetParamI("NBins");}

   /** @brief Returns true if logartihmic binning is chosen in the current configuration
    * 
    *  
    * @return Int_t
    */
    Bool_t  GetParamLogBins()          {if (GetParamI("LogBins")>0) return true;
		                                else return false;}

   /** @brief Returns true if logartihmic binning is chosen in the current configuration
    *  
    * @return Int_t
    */
    Bool_t  GetParamSGFilter()         {if (GetParamI("SGFilter")>0) return true;
		                                else return false;}

   /** @brief Returns true if R0 optimization is requested in the current configuration
    *  
    * @return Int_t
    */
    Bool_t  GetParamOptimizeR0()         {if (GetParamI("OptimizeR0")>0) return true;
		                                else return false;}

   /** @brief Returns the calibration altitude for the inversion in the current configuration
    *  for a given wave length
    *  
    * @return Float_t
    */
    Float_t GetParamR0(Int_t wl)             {
		if (wl==355)        return GetParamF("R0_355");
		else if (wl==532)  return GetParamF("R0_532");
		else               return 0;
		 }

   /** @brief Returns the Fernald Sp factor in the current configuration
    *  for a given wave length
    *  
    * @return Float_t
    */
    Float_t GetParamFernaldSp(Int_t wl)             {
		if (wl==355)        return GetParamF("Fernald_Sp355");
		else if (wl==532)   return GetParamF("Fernald_Sp532");
		else               return 0;
		 }

   /** @brief Returns true if alignment correction factor optimization
    * is requested in the current configuration
    *  
    * @return Int_t
    */
    Bool_t  GetParamOptimizeAC()         {if (GetParamI("OptimizeAC")>0) return true;
		                                else return false;}

   /** @brief Returns the altitude down to which the alignment correction factor optimization
    * is done
    *  
    * @return Int_t
    */
    Float_t  GetParamOptimizeAC_Hmin()         {return GetParamF("OptimizeAC_Hmin");}

   /** @brief Returns the mis-alignement correction factor for the inversion
    *  in the current configuration for a given wave length
    *  
    * @return Float_t
    */
    Float_t GetParamAC(Int_t wl)             {
		if (wl==355)        return GetParamF("AlignCorr_355");
		else if (wl==532)  return GetParamF("AlignCorr_532");
		else               return 0;
		 }

   /** @brief Returns the Klett inverstion k parameter
    *  as in beta=l*alpha^k
    * @see Analyser::KlettInversion
    *  
    * @return Float_t
    */
    Float_t GetParamKlett_k()        {return GetParamF("Klett_k");}

   /** @brief Returns the Klett inverstion l parameter
    *  as in beta=l*alpha^k
    * @see Analyser::KlettInversion
    *  
    * @return Float_t
    */
    Float_t GetParamKlett_l()        {return GetParamF("Klett_l");}
 
   /** @brief Returns the altitude for the lower bound of the
    *  integration of the atmosphere opacity
    * 
    * @see Analyser::ComputeAtmosphereOpacity
    *  
    * @return Float_t
    */
    Float_t GetTauAltMin()           {return GetParamF("TauAltMin");}

   /** @brief Returns the altitude for the upper bound of the
    *  integration of the atmosphere opacity
    * 
    * @see Analyser::ComputeAtmosphereOpacity
    *  
    * @return Float_t
    */
    Float_t GetTauAltMax()           {return GetParamF("TauAltMax");}

   /** @brief Returns the name of the file used for the atmospheric
    *  profile
    * 
    * @return std::string
    */  
    std::string GetAtmoProfile()  {return GetParam("AtmoProfile");}

   /** @brief Returns the name of the file used for the atmospheric
    *  absorption
    * 
    * @return std::string
    */  
    std::string GetAtmoAbsorption()  {return GetParam("AtmoAbsorption");}

   /** @brief Returns the name of the file used for the overlap function
    * 
    * @return std::string
    */  
    std::string GetOverlap()         {return GetParam("OverlapFunction");}

   /** @brief Return the name of the chosen inversion algorithm
    * @see Analyser::ProcessData
    *  
    * @return std::string
    */
    std::string GetAlgName()        {return GetParam("AlgName");}

   /** @brief Returns Fernald Sp(wl nm) parameter
    * also known as the Lidar ratio
    * @see Analyser::Fernald84
    * @param wl the wave lenght 
    * @return Float_t
    */
    Float_t GetFernald_Sp(Int_t wl)             {
		if (wl==355)        return GetParamF("Fernald_Sp355");
		else if (wl==532)  return GetParamF("Fernald_Sp532");
		else               return 0;
		 }

   /** @brief Returns Fernald Sratio parameter:
    * Sratio=1+Beta_p/Beta_m
    *
    * Fraction of aerosols at reference altitude R0
    * @see Analyser::Fernald84
    *  
    * @return Float_t
    */
    Float_t GetFernald_sratio()           {return GetParamF("Fernald_sratio");}

   /** @brief Returns Signal To Noise ratio threshold:
    * noise is estimated from the measurement scattering
    * signal is total signal
    *
    * If requested, reference altitude R0 will be lowered to meet the
    * SN ratio threshold.
    * @see Analyser::ProcessData
    *  
    * @return Float_t
    */
    Float_t GetSNRatioThreshold()           {return GetParamF("SNRatioThreshold");}

   /** @brief Returns the config map
    * 
    * @see Plotter::SaveAs
    *  
    * @return Float_t
    */
    std::map <std::string, std::string> GetMap()   const        {return fConfig;}


  private:
    /** @brief boolean to print some results if true */
    Bool_t fVerbose; //!

   /** @brief the <key,value> map used to store the current configuration
    * 
    */  
    std::map <std::string, std::string> fConfig;
           
  protected:
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::ConfigHandler,1);
#endif

  }; // class

}; // namespace

#endif
