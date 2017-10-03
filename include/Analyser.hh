/** @file Analyser.hh
 *
 * @brief Analyser class definition.
 *
 * Class to drive the Lidar data analysis
 *
 * @author Johan Bregeon
 *  
*/

#ifndef LIDARTOOLS_ANALYSER
#define LIDARTOOLS_ANALYSER

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#include <TArrayF.h>
#include <TArrayD.h>
#endif


#include <map>
#include <ctime>

#include "Overlap.hh"
#include "ConfigHandler.hh"
#include "AtmoProfile.hh"
#include "AtmoAbsorption.hh"
#include "GlidingAveFilter.hh"
#include "SavGolFilter.hh"

/** @namespace LidarTools
 *
 * @brief LidarTools package namespace
 * 
*/
namespace LidarTools {
	
/** @class Analyser
 * 
 * @brief Drives the Lidar data reduction analysis
 * 
*/
  class Analyser
  {
  
  public:
     
    /** @brief class constructor.
     *
     * The Analyser constructor takes an array of ranges, a map of signal and a bool for
     * verbosity.
     * 
     * @param range a TArrayF of altitudes
     * @param signalmap a map of Lidar data <wavelength, signal>
     * @param verbose a bool for verbosity
     */
    Analyser(TArrayF, std::map<Int_t, TArrayF>, Bool_t verbose=false);
   
    /** @brief class destructor
     * 
     * Deletes pointer members
     * @see fAbsorp fConfig fOverlap
     */
    virtual ~Analyser(); 
   
    /** @brief Get indices, and initialize variables
     *
     */
    int InitIndices();


    /** @brief Correct range for laser inclination
     *
     */
    void CorrectRange();

    /** @brief Set default configuration
     *
     */
    int SetConfig();

    /** @brief Pass a configuration file
     *
     *  @param infilename configuration file name, @see ConfigHandler
     */
    int SetConfig(char[]);

    /** @brief Write configuration to members
     *
     */
    int StoreConfigLocally();

    /** @brief Update ConfigHandler config from members
     *
     */
    int StoreConfigToHandler();

    /** @brief Overwrite a configuration parameter on the fly
     *
     * @param key key string
     * @param value key value
     */
    int OverwriteConfigParam(std::string, std::string);

    /** @brief Process data for all wavelengths
     *
     */
    int ProcessData();

    /** @brief Process data for the given wavelength
     *
     * @param wl the wavelength as an integer 
     * 
     */
    int ProcessData(Int_t);

    /** @brief Prepare data for the given wavelength
     *
     * @param wl the wavelength as an integer 
     * 
     */
    int PrepareData(Int_t);

    /** @brief Check data quality for the given wavelength
     *
     *  Look for -5 V spike in raw signal 
     *
     * @param wl the wavelength as an integer 
     * 
     */
    Bool_t CheckQuality(Int_t);

    /** @brief Get the Signal to Noise Ratio at R0 for the given wavelength
     *
     *  If SNR is too low, consider a lower R0
     *
     * @param wl the wavelength as an integer
     * @todo not implemented ! 
     * 
     */
    //Float_t VerifySNRatio(Int_t);


    /** @brief Subtract background for the given wavelength
     *
     * @param wl the wavelength as an integer 
     * 
     */
    void SubtractBackground(Int_t);

    /** @brief Compute the signal power "P=V*R**2" for the given wavelength
     *  Also compute Ln(Power) for plotting
     *
     * @param wl the wavelength as an integer 
     * 
     */
    void ComputePower(Int_t);
    
    /** @brief Filter the signal power for the given wavelength
     * 
     * The SavGolFilter class that implements a Savitsky-Golay filtering is
     * used.
     *
     *  Also computes Ln(filtered(Power)) for plotting
     *
     * @param wl the wavelength as an integer 
     * 
     */
    void FilterPower(Int_t);

    /** @brief Rebin data proxy, calls either RebinDataLog or
     *  RebinDataLinear
     *
     * @param wl the wavelength as an integer 
     * 
     */
    void RebinData(Int_t);    

    /** @brief Rebin data in logarithmic bins of altitude for the given wavelength
     *
     * @param wl the wavelength as an integer 
     * 
     */
    void RebinDataLog(Int_t);    

    /** @brief Rebin data linearly in altitude for the given wavelength
     *  using a gliding average filter
     * 
     * @param wl the wavelength as an integer      
     * 
     */
    void RebinDataGAF(Int_t);

    /** @brief Rebin data linearly in altitude for the given wavelength
     *
     * @param wl the wavelength as an integer 
     * 
     */
    void RebinDataLinear(Int_t);

    /** @brief run the Klett inversion algorithm for the given wavelength
     *
     * uses parameters as initialized by the ConfigHandler
     *
     * @param wl the wavelength as an integer 
     * 
     * P(r)=V(r)*r^2
     *
     * alpha(r)=P(r)/( P(r0)/alpha0 - 2*sum{r0,r}{P(h).dh} )
     */
    void KlettInversion(Int_t);    

    /** @brief run the Fernald inversion algorithm for the given wavelength
     *
     *  uses parameters as initialized by the ConfigHandler
     *
     * @param wl the wavelength as an integer 
     * 
     * See Fernald 84 paper
     */
    void Fernald84Inversion(Int_t);    

    /** @brief run the official Aeronet inversion algorithm for the given wavelength
     *
     *  uses parameters as initialized by the ConfigHandler
     *
     * @param wl the wavelength as an integer 
     * 
     * See George fortran code
     */
    void AeronetInversion(Int_t);    

    /** @brief Compute atmosphere opacity profile
     *
     * and Optical depths for atmosphere opacity from TauMin to TauMax
     * integral of alpha
     *
     * @param wl the wavelength as an integer 
     */
    void ComputeAtmosphereOpacity(Int_t);

    /** @brief Compute atmosphere transmission profile
     *
     * @param wl the wavelength as an integer 
     */
    void ComputeAtmosphereTransmission(Int_t);

    /** @brief Set verbosity on or off
     *
     * @param verbose a bool, true or false
     */
    void SetVerbose(Bool_t verbose) {fVerbose=verbose;};

    /** @brief Set Run Number
     *
     * @param run the run number
     */
    void SetRunNumber(Int_t run) {fRunNumber=run;};

    /** @brief Set Sequence Number
     *
     * @param seq the sequence number
     */
    void SetSeqNumber(Int_t seq) {fSeqNumber=seq;};
    
    /** @brief Set Time stamp
     *
     * @param time the time stamp as a time_t
     */
    void SetTime(time_t time) {fTimeStamp=time;};

    // Getters

   /** @brief Returns the calibration altitude for the inversion in the current configuration
    *  for a given wave length
    *  
    * @return Float_t
    */
    Float_t GetParamR0(Int_t wl) const          {
		if (wl==355)        return fParamR0_355;
		else if (wl==532)  return fParamR0_532;
		else               return 0;
		 }

   /** @brief Sets the calibration altitude for the inversion in the current configuration
    *  for a given wave length
    *  
    * @return Float_t
    */
    void SetParamR0(Int_t wl, float r0)           {
		if (wl==355)         fParamR0_355=r0;
		else if (wl==532)   fParamR0_532=r0;
		else exit(1);
		 }

   /** @brief Returns the Fernald_Sp for the inversion in the current configuration
    *  for a given wave length
    *  
    * @return Float_t
    */
    Float_t GetParamFSp(Int_t wl) const          {
		if (wl==355)        return fFernald84_Sp355;
		else if (wl==532)  return fFernald84_Sp532;
		else               return 50;
		 }

   /** @brief Sets the Fernald_Sp for the inversion in the current configuration
    *  for a given wave length
    *  
    * @return void
    */
    void SetParamFSp(Int_t wl, Float_t Sp)          {
		if (wl==355)        fFernald84_Sp355=Sp;
		else if (wl==532)  fFernald84_Sp532=Sp;
		else exit(1);
		 }

   /** @brief Returns the Mis-Alignment correction factor in the current configuration
    *  for a given wave length
    *  
    * @return Float_t
    */
    Float_t GetParamFAC(Int_t wl) const          {
		if (wl==355)        return fParamAlignCorr_355;
		else if (wl==532)  return fParamAlignCorr_532;
		else               return 50;
		 }

   /** @brief Sets the Mis-Alignement corection factor in the current configuration
    *  for a given wave length
    *  
    * @return void
    */
    void SetParamFAC(Int_t wl, Float_t ACcor)     {
		if (wl==355)        fParamAlignCorr_355=ACcor;
		else if (wl==532)  fParamAlignCorr_532=ACcor;
		else exit(1);
		 }
    
    /** @brief Get Run Number
     */
     Int_t GetRunNumber() const              {return fRunNumber;}

    /** @brief Get Sequance Number
     */
     Int_t GetSeqNumber() const              {return fSeqNumber;}

    /** @brief Get Time String
     *
     */
    char* GetTimeString()                    {return ctime(&fTimeStamp);}

    /** @brief Get Theta
     *
     */
     Float_t GetLidarTheta() const           {return fLidarTheta;}

    /** @brief Get RangeToAltitude
     *
     */
     Float_t GetRangeToAltitude() const      {return cos(fLidarTheta*3.14159/180.);}

    /** @brief Get Lidar altitude
     *
     */
    Float_t GetLidarAltitude() const         {return fLidarAltitude;}
    
    /** @brief Get the TArrayF of raw range
     *
     */
    TArrayF GetRawRange() const             {return fRawRange;}

    /** @brief Get the TArrayF of altitudes
     *
     */
    TArrayF GetAltitudes() const             {return fAltitude;}
    
    /** @brief Get a TArrayF of the center of the altitude bins
     *  @see RebinData
     */
    TArrayF GetBinsCenterAltitude() const    {return fBinsCenterAltitude;}

    /** @brief Get Bkg Min range
     */
     Float_t GetParamBkgMin() const              {return fParamBkgMin;}

    /** @brief Get Bkg Max Range
     */
     Float_t GetParamBkgMax() const              {return fParamBkgMax;}

    /** @brief Get the data quality flag for the given wavelength
     *
     * @see CheckQuality
     * 
     * @param wl the wavelength as an integer 
    */
    const Bool_t  GetQuality(Int_t wl)       {return fQualityMap[wl];}    

    /** @brief Get the TArrayF of raw signal for the given wavelength
     *
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetRawSignal(Int_t wl)     {return fSignalMap[wl];}

    /** @brief Get the background value for a given wavelength
     *
     * @see SubtractBackground
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetBkg(Int_t wl)           {return fBkgMap[wl];}

    /** @brief Get a TArrayF of the raw data used to estimate the background
     *
     * @see SubtractBackground
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetFullBkg(Int_t wl)       {return fFullBkgMap[wl];}

    /** @brief Get a TArrayF of reduced signal for a given wavelength
     *
     * @see SubtractBackground 
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetReducedSignal(Int_t wl) {return fReducedSignalMap[wl];}

    /** @brief Get a TArrayF of the signal power
     *
     * @see ComputePower
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetPower(Int_t wl)         {return fPowMap[wl];}

    /** @brief Get a TArrayF of the filtered signal power
     *
     * @see FilterPower
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetFilteredPower(Int_t wl) {return fFilteredPowMap[wl];}
 
    /** @brief Get a TArrayF of the binned signal power
     *
     * @see ComputePower RebinData
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetBinnedPower(Int_t wl)   {return fBinnedPowMap[wl];}

    /** @brief Get a TArrayF of the binned signal power standard deviation
     *
     * @see ComputePower RebinDataGAF
     * @param wl the wavelength as an integer 
    */
    const TArrayF GetBinnedPowerDev(Int_t wl)   {return fBinnedPowDevMap[wl];}

    /** @brief Get a TArrayF of the extinction profile
     *
     * @see KlettInversion, Fernald84Inversion
     * @param wl the wavelength as an integer 
     * @param scattering kind of profile (total "T", Moelcules "M", Mie "P")
    */
    TArrayF GetAlphaProfile(Int_t wl, std::string scattering);

   /** @brief Get a TArrayF of the total extinction profile
     *
     * @see KlettInversion, Fernald84Inversion
     * @param wl the wavelength as an integer 
    */
    TArrayF GetAlphaProfile(Int_t wl) {return GetAlphaProfile(wl, "T");}

    /** @brief Get a TArrayF of the backscatter profile
     *
     * @see KlettInversion, Fernald84Inversion
     * @param wl the wavelength as an integer 
     * @param scattering kind of profile (total "T", Molecules "M", Mie "P")
    */
    TArrayF GetBetaProfile(Int_t wl, std::string scattering);

    /** @brief Get a TArrayF of the total backscatter profile
     *
     * @see KlettInversion, Fernald84Inversion
     * @param wl the wavelength as an integer 
    */
    TArrayF GetBetaProfile(Int_t wl) {return GetBetaProfile(wl,"T");}

   /** @brief Get a TArrayF of the atmosphere model total extinction profile
     *
     * @see KlettInversion, Fernald84Inversion
     * @param wl the wavelength as an integer 
    */
    TArrayF GetAlphaModelProfile(Int_t wl) {if(fAlphaModelMap.count(wl)>0) return fAlphaModelMap[wl];
		                            else return false;}

    
    /** @brief Return true if detailed particle and molecule information
     * is available.
     *
     * @see Fernald84Inversion
     * @param wl the wavelength as an integer 
    */
    bool hasDetails(Int_t wl)  {if(fAlphaMap_P.count(wl)>0) return true;
		                        else return false;}

    /** @brief Return true if data were filtered
     *
     * @see FilterPower
    */
    bool hasFiltered()  {if(fParamSGFilter) return true;
		                        else return false;}
                                        

    /** @brief Get the opacity profile value for the given wavelength
     *
     * @see ComputeAtmosphereOpacity
     * @param wl the wavelength as an integer 
     * @param scattering kind of profile (total "T", Molecules "M", Mie "P")
    */
    TArrayF GetOpacityProfile(Int_t wl, std::string scattering);

    /** @brief Get the total opacity profile value for the given wavelength
     *
     * @see ComputeAtmosphereOpacity
     * @param wl the wavelength as an integer 
    */
    TArrayF GetOpacityProfile(Int_t wl)          {return GetOpacityProfile(wl,"T");}

    /** @brief Get the opacity model profile value for the given wavelength
     *
     * @see ComputeAtmosphereOpacity AtmoAbsorption
     * @param wl the wavelength as an integer 
    */
    TArrayF GetOpacityModelProfile(Int_t wl)          {return fOpacityModelMap[wl];}

    /** @brief Get the transmission profile value for the given wavelength
     *
     * @see ComputeAtmosphereTransmission
     * @param wl the wavelength as an integer 
    */
    TArrayF GetTransmissionProfile(Int_t wl)          {return fTransmissionMap[wl];}

    /** @brief Get the Transmission model profile value for the given wavelength
     *
     * @see ComputeAtmosphereTransmission AtmoAbsorption
     * @param wl the wavelength as an integer 
    */
    TArrayF GetTransmissionModelProfile(Int_t wl)          {return fTransmissionModelMap[wl];}

    /** @brief Get the integrated opacity value for the given wavelength
     *
     * @see ComputeAtmosphereOpacity KlettInversion
     * @param wl the wavelength as an integer 
    */
    Float_t GetOD(Int_t wl, std::string scattering);

    /** @brief Get the total optical depth for the given wavelength
     *
     * @see ComputeAtmosphereOpacity KlettInversion
     * @param wl the wavelength as an integer 
    */
    Float_t GetOD(Int_t wl) {return GetOD(wl,"T");}

    /** @brief Get the Rayleigh scattering optical depth for a given wavelength
     *
     * @see ComputeAtmosphereOpacity KlettInversion
     * @param wl the wavelength as an integer 
    */
    Float_t GetRayleighOD(Int_t wl) {return GetOD(wl,"M");}

    /** @brief Get the aerosol optical depth for the given wavelength
     * 
     * This is the Aerosol Optical Depth = AOD
     *
     * @see ComputeAtmosphereOpacity Fernald84
     * @param wl the wavelength as an integer 
    */
    Float_t GetAOD(Int_t wl)          {return GetOD(wl,"P");}
    
    /** @brief Get the Model total optical depth for the given wavelength
     *
     * @see ComputeAtmosphereOpacity KlettInversion
     * @param wl the wavelength as an integer 
    */
    Float_t GetModelOD(Int_t wl)           {return fODModelMap[wl];}

    /** @brief Get the model AOD for the given wavelength
     * 
     * This is the Aerosol Optical Depth = AOD
     *
     * @see ComputeAtmosphereOpacity Fernald84
     * @param wl the wavelength as an integer 
    */
    Float_t GetModelAOD(Int_t wl)          {return fODModelMap_P[wl];}

    /** @brief Get a pointer to the ConfigHandler
     *
     * @see ConfigHandler SetConfig StoreConfigLocally     
    */
    ConfigHandler* GetConfig()         {return fConfig;}
 
     /** @brief Get a vector of available wavelength
     *
     * Now only 355 nm and 532 nm for HESS elastic lidar
     * 
     * @see Analyser LidarFile
    */
   std::vector<Int_t> GetWavelengths(){return fWaveLengthVec;}

    /** @brief Get a pointer to the AtmoAbsorption
     *
    */
    AtmoAbsorption* GetAtmoAbsorp() {return fAbsorp;}
  
  private:

    /** @brief Init  LidarTools::AtmoProfile that describes the atmosphere
     * profile
     *
     */
     void InitAtmoProfile();
  
    /** @brief Init  AtmoAbsorption for atmospheric transmission 
     *
     */
     void InitAtmoAbsorption();
     
    /** @brief Init LidarTools::Overlap for the overlap function correction */
    void InitOverlap();

    /** @brief LidarTools::doOptimizeRO for a given wavelength
     *  so that S/N ratio be above threshold - changes fParamR0_wl
     *
     * @param wl the wavelength as an integer 
     */
    int doOptimizeR0(Int_t);

    /** @brief LidarTools::doOptimizeAC for a given wavelength
     *  to set fParamAC_wl for mis-alignment correction
     *
     * @param wl the wavelength as an integer 
     */
    int doOptimizeAC(Int_t);

    /** @brief LidarTools::PureRayleighInversion for a given wavelength
     *  to test a mis-alignment correction factor
     *
     * @param wl the wavelength as an integer 
     * @param AcCorr the correction factor
     */
    Float_t PureRayleighInversion(Int_t, Float_t);

    /** @brief boolean to print some results if true */
    Bool_t fVerbose;
    
    /** @brief the run number */
    Int_t fRunNumber;
    /** @brief the sequence number */
    Int_t fSeqNumber;
    
    /** @brief the time */
    time_t fTimeStamp;

    /** @brief Vector of available wave length */
    std::vector<Int_t> fWaveLengthVec;
    
    /** @brief Raw altitude in km above Lidar */
    TArrayF fRawRange;
    
    /** @brief Altitude in km above Lidar corrected for zenith angle inclination */
    TArrayF fRange;

    /** @brief Configuration handler */
    ConfigHandler *fConfig;

    /** @brief overlap function*/
    LidarTools::Overlap *fOverlap;
    /** @brief overlap function filename */
    std::string fOverlapFileName;
    /** @brief apply overlap */
    bool fApplyOverlap;

    /** @brief Raw data map */
    std::map<Int_t, TArrayF> fSignalMap;
    /** @brief Data Quality map */
    std::map<Int_t, Bool_t> fQualityMap;    
    /** @brief Background map */
    std::map<Int_t, Float_t> fBkgMap;
    /** @brief Background map */
    std::map<Int_t, TArrayF> fFullBkgMap;
    /** @brief Reduced signal map */
    std::map<Int_t, TArrayF> fReducedSignalMap;
    /** @brief power signal map */
    std::map<Int_t, TArrayF> fPowMap;
    /** @brief ln power signal map */
    std::map<Int_t, TArrayF> fLnPowMap;
    /** @brief filtered power signal map */
    std::map<Int_t, TArrayF> fFilteredPowMap;
    /** @brief ln(filtered(power)) signal map */
    std::map<Int_t, TArrayF> fLnFilteredPowMap;
    /** @brief Binned signal map */
    std::map<Int_t, TArrayF> fBinnedPowMap;
    /** @brief Binned signal standard deviation map */
    std::map<Int_t, TArrayF> fBinnedPowDevMap;
    /** @brief Alpha0 map */
    std::map<Int_t, Float_t> fParamAlpha0Map;
    /** @brief Extinction map */
    std::map<Int_t, TArrayF> fAlphaMap;
    /** @brief Extinction map for molecules - Rayleigh scattering*/
    std::map<Int_t, TArrayF> fAlphaMap_M;
    /** @brief Extinction map for particules - Mie scattering */
    std::map<Int_t, TArrayF> fAlphaMap_P;
    /** @brief Backscatter map */
    std::map<Int_t, TArrayF> fBetaMap;
    /** @brief Backscatter map for molecules - Rayleigh scattering*/
    std::map<Int_t, TArrayF> fBetaMap_M;
    /** @brief Backscatter map for particules - Mie scattering */
    std::map<Int_t, TArrayF> fBetaMap_P;
    /** @brief Opacity profile map */
    std::map<Int_t, TArrayF> fOpacityMap;
    /** @brief Opacity profile map  - Rayleigh scattering */
    std::map<Int_t, TArrayF> fOpacityMap_M;
    /** @brief Opacity profile map  - Mie scattering */
    std::map<Int_t, TArrayF> fOpacityMap_P;
    /** @brief Transmission profile map */
    std::map<Int_t, TArrayF> fTransmissionMap;
    /** @brief OD map */
    std::map<Int_t, Float_t> fODMap;
    /** @brief OD map for Rayleigh scattering */
    std::map<Int_t, Float_t> fODMap_M;
    /** @brief OD map for Mie scattering */
    std::map<Int_t, Float_t> fODMap_P;    
    /** @brief Extinction map for atmosphere transmission model */
    std::map<Int_t, TArrayF> fAlphaModelMap;
    /** @brief Opacity model profile map */
    std::map<Int_t, TArrayF> fOpacityModelMap;
    /** @brief Transmission model profile map */
    std::map<Int_t, TArrayF> fTransmissionModelMap;
    /** @brief OD map for Model */
    std::map<Int_t, Float_t> fODModelMap;
    /** @brief OD map for Model - Mie scattering */
    std::map<Int_t, Float_t> fODModelMap_P;

    /** @brief N points in altitude range */
    Int_t fN;
    /** @brief Laser inclination with respect to zenith */
    Int_t fLidarTheta;
    /** @brief Index of min altitude */
    Int_t fAltMinIndex;
    /** @brief Index of max altitude */
    Int_t fAltMaxIndex;
   
    /** @brief Altitude array in meters above sea level */
    TArrayF fAltitude;
    /** @brief Binned Altitude array */
    TArrayF fBinsAltitude;
    /** @brief Center Altitude bins array */
    TArrayF fBinsCenterAltitude;

    /** @brief Choose reconstruction algorithm */
    std::string fAlgName;
    
    /** @brief boolean to optimize automatically the choice of R0 */
    Bool_t fParamOptimizeR0;
    /** @brief boolean to optimize automatically the mis-alignment correction factor */
    Bool_t fParamOptimizeAC;
    /** @brief altitude down to which we assume to be pure Rayleigh when optimizing mis-alignement corrections factors */
    Float_t fParamOptimizeAC_Hmin;

    /* Reconstruction parameters
     * Altitudes in meters and above the LidarAltitude, except itself
     */
    /** @brief Lidar Altitude */
    Float_t fLidarAltitude;      
    /** @brief Threshold in V expected to be reached for good data */
    Float_t fQualityThr;
    /** @brief Minimum altitude for the signal */
    Float_t fParamAltMin;      
    /** @brief Maximum altitude for the signal */
    Float_t fParamAltMax;  
    /** @brief Minimum altitude for the background estimation */
    Float_t fParamBkgMin;  
    /** @brief Maximum altitude for the background estimation */
    Float_t fParamBkgMax;  
    /** @brief Background fudge factor */
    Float_t fParamBkgFFactor;  
    /** @brief Nb of bins in altitude     */
    UInt_t fParamNBins;
    /** @brief Use Logarithmic binning in altitude ? */
    Bool_t fParamLogBins;
    /** @brief Apply Savitsky-Golay filter ? */
    Bool_t fParamSGFilter;

    /* See Klett inversion method */    
    /** @brief Reference Altitude for 355 nm */
    Float_t fParamR0_355;      
    /** @brief Reference Altitude for 532 nm */
    Float_t fParamR0_532;      
    /** @brief as in beta=l*alpha^k */
    Float_t fParamKlett_k;
    /** @brief as in beta=l*alpha^k */
    Float_t fParamKlett_l;
    /** @brief Minimum altitude from which to integrate the atmosphere opacity */
    Float_t fTauAltMin;
    /** @brief Maximum altitude from which to integrate the atmosphere opacity */
    Float_t fTauAltMax;
    
    /* See Fernald84 inversion method */
    /** @brief Sp=alpha_p/beta_p extinction-to-backscatter ratio at 355 nm*/
    Float_t fFernald84_Sp355;
    /** @brief Sp=alpha_p/beta_p extinction-to-backscatter ratio at 532 nm*/
    Float_t fFernald84_Sp532;
    /** @brief sratio=1+Sp/Sr */
    Float_t fFernald84_sratio;
    
    /** @brief Signal To Noise Ratio threshold to start intergration */
    Float_t fSNRatioThreshold;

    /** @brief Mis-Alignment correction factor for 355 nm */
    Float_t fParamAlignCorr_355;
    /** @brief Mis-Alignment correction factor for 532 nm */
    Float_t fParamAlignCorr_532;
    
    /** @brief Atmospheric Absorption.
      *
    */
    AtmoAbsorption *fAbsorp;

    /** @brief Atmospheric profile
     * 
     * The atmospheric profile: density, pressure, temperature etc...
     */
    AtmoProfile* fAtmoProfile;
    
    /** @brief Atmospheric absorption file name
     * 
     */
    std::string fAtmoAbsorption;

    /** @brief Atmospheric P,T profile file name
     *  atmprof10.dat
     */
    std::string fAtmoFileName;
    
  protected:
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::Analyser,1);
#endif

  }; // class

}; // namespace

#endif
