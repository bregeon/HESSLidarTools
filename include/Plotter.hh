/** @file Plotter.hh
 *
 * @brief Plotter class definition
 *
 * Class to display the Lidar data analysis results with ROOT
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_PLOTTER
#define LIDARTOOLS_PLOTTER

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#include <TArrayF.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TLegend.h>
#include <TText.h>
#endif

#include <iostream> 
#include <iomanip> // setprecision
#include <sstream> // stringstream

#include "LidarTools/Analyser.hh"

namespace LidarTools {

 /** @class Plotter
  * 
  * @brief Display Analyser analysis results with ROOT
  *
  */
  class Plotter
  {
  
  public:
  
    /** @brief class constructor, takes a pointer to an Analyser
     * 
     * @param analyser a pointer to an Analyser
     * @param applyOffset choose or not to offset altitude by Analyser->fLidarAltitude
     * @param verbose a bool to turn verbosity on or off
     *
     */
    Plotter(LidarTools::Analyser*, Bool_t applyOffset=true, Bool_t verbose=false);
    
    /** @brief class destructor
     * Delete the pointer to the analyser, the pointer to the main TCanvas
     * and clear TGraphs.
     */
    virtual ~Plotter();

    /** @brief Set verbosity on or off
     *
     * @param verbose a bool, true or false
     */
    void SetVerbose(Bool_t verbose) {fVerbose=verbose;}
    
    /** @brief Get color for a given wave length
     * 
     * 355 nm is blue, 532 nm is green
     * 
     * @param wl the wavelength as an integer
     * @return an ROOT color as an integer
     */
    Int_t GetColor(Int_t);
    
    // Handle graphs per wavelength
    // Initialize graphs
    /** @brief Initialize raw profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */
    void InitRawProfile(Int_t);

    /** @brief Initialize the background profile for the given wave length
     * 
     * @param wl the wavelength
     */
    void InitBkgProf(Int_t);

    /** @brief Initialize the background histogram for the given wave length
     * 
     * @param wl the wavelength
     */
    void InitBkgHist(Int_t);

    /** @brief Initialize the Ln(Raw Power) profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void InitLnRawPowerProfile(Int_t);
    
    /** @brief Initialize the Ln(FilteredPower) profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void InitLnFilteredPowerProfile(Int_t);
    
    /** @brief Initialize the Ln(Binned Power) profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void InitLnBinnedPowerProfile(Int_t);

    /** @brief Initialize the Binned Power Deviatio profiles TGraph for the given wave length
     * 
     * Power, Power StdDev, Power/StdDev
     * 
     * @param wl the wavelength
     */    
    void InitBinnedPowerDevProfiles(Int_t);
    
    /** @brief Initialize the atmosphere extinction profiles TGraph
     *  for the given wave length and all kinds: total, particles, molecules
     * 
     * @param wl the wavelength
     */    
    void InitExtinctionProfiles(Int_t);

    /** @brief Initialize the atmosphere backscatter profiles TGraph
     *  for the given wave length and all kinds: total, particles, molecules
     * 
     * @param wl the wavelength
     */    
    void InitBackscatterProfiles(Int_t);
    
    /** @brief Initialize the Lidar ratio TGraph
     * 
     * Extinction-to-backscatter ratio
     * 
     * @param wl the wavelength
     */    
    void InitLidarRatio(Int_t);

    /** @brief Initialize the Opacity profile TGraph
     * 
     * Opacity
     * 
     * @param wl the wavelength
     */    
    void InitOpacityProfile(Int_t);

    /** @brief Initialize the transmission profile TGraph
     * 
     * Opacity
     * 
     * @param wl the wavelength
     */    
    void InitTransmissionProfile(Int_t);

    /** @brief Initialize the OD results TGraph
     * 
     * Opacity
     * 
     * @param wl the wavelength
     */    
    void InitODResults(Int_t);
    
    /** @brief Initialize the Angstroem Exponent profile TGraph
     * 
     * Anstroem Exponent
     * 
     * @param wl the wavelength
     */    
    void InitAngstExpProfile();
    
    
    // Fill graphs
    /** @brief Fill the raw profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillRawProfile(Int_t);

    /** @brief Fill the background profile for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillBkgProf(Int_t);

    /** @brief Fill the background histogram for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillBkgHist(Int_t);

    /** @brief Fill the Ln(Power) profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillLnRawPower(Int_t);

    /** @brief Fill the Ln(Power) profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillLnFilteredPower(Int_t);

    /** @brief Fill the Ln(Power) profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillLnBinnedPower(Int_t);
 
     /** @brief Fill the Power Deviation profile TGraph for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillBinnedPowerDev(Int_t);
   
    /** @brief Fill the atmospher extinction and backscatter profiles TGraph
     *  for the given wave length
     * 
     * Fills total Exctinction (alpha) and Backscatter (better)
     * and the exctinction-to-backscatter ratio (Lidar Ratio)
     * 
     * then if information available fills separate graph for
     * Mie (particle) and Rayleigh (molecule) scattering
     * 
     * @param wl the wavelength
     */    
    void FillExtinctionBackscatter(Int_t);
   

    /** @brief Fill the atmosphere opacity profile TGraph
     *  for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillOpacity(Int_t);
   
    /** @brief Fill the atmosphere transmission profile TGraph
     *  for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillTransmission(Int_t);
   

    /** @brief Fill the optical depth results TGraph
     *  for the given wave length
     * 
     * @param wl the wavelength
     */    
    void FillODResults(Int_t);
    
   
    /** @brief Fill the Angstroem Exponent profile TGraph
     *  for two given wave lengths:
     *  Angstroem Exponent = log(abs(alpha_p_wl1/alpha_p_wl2))/log(wl2/wl1)
     * 
     * @param wl1 the first wavelength
     * @param wl2 the second wavelength
     */    
    void FillAngstroemExp(Int_t, Int_t);
      

    // Loops on all available wavelength
    /** @brief Initialize all graphs and histograms for the given wavelength
     *
     * @param wl the wavelength
     */
    void InitAll(Int_t);

    /** @brief Loops on all available wavelength to initialize graphs
     *
     */
    void InitAll();

    /** @brief Fill all graphs and histograms for the given wavelength
     *
     * @param wl the wavelength
     */
    void FillAll(Int_t);

    /** @brief Loops on all available wavelength to fill graphs
     *
     */
    void FillAll();

    /** @brief Display a canvas with the analysis results for all wavelength
     *
     */
    void DisplayAll();

    /** @brief Display a canvas with the low level analysis results for all wavelength
     *
     */
    void DisplayRaw();

    /** @brief Display a canvas with the background information for all wavelength
     *
     */
    void DisplayBkg();

    /** @brief Display a canvas with the StdDev information for all wavelength
     *
     */
    void DisplayStdDev();

    /** @brief Display a canvas with the filtering results for all wavelength
     *
     */
    void DisplayFilter();

    /** @brief Display a canvas with the analysis results for all wavelength
     *
     */
    void DisplayMain();

    /** @brief Display an extra canvas with the analysis results for all wavelength
     * in particular with the Lidar ratio
     */
    void DisplayExtra();

    /** @brief Display a summary canvas with the analysis results for all wavelength
     * including the atmosphere model
     */
    void DisplaySummary();
    
    /** @brief Display  canvas with the Angstroem exponent profile
     */
    void DisplayAngstExpAOD();

    /** @brief Save everything to disk
     * 
     * @param filename the ourput ROOT file path
     */
    void SaveAs(std::string);
    
    // Getters
    /** @brief Returns a pointer to the raw profile TGraph of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetRawProfile(Int_t wl)     {if(fRawProfileMap.count(wl)) return fRawProfileMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the background profile of the given wavelength
     * 
     * @param wl the wavelength
     * @return TH1F *
     */
    TGraph *GetBkgProf(Int_t wl)          {if(fBkgProfMap.count(wl)) return fBkgProfMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the background histogram of the given wavelength
     * 
     * @param wl the wavelength
     * @return TH1F *
     */
    TH1F *GetBkgHist(Int_t wl)          {if(fBkgHistMap.count(wl)) return fBkgHistMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the Ln(Raw Power) profile TGraph of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetLnRawPowerProfile(Int_t wl) {if(fLnRawPowerMap.count(wl))return fLnRawPowerMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the Ln(Filtered Power) profile TGraph of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetLnFilteredPowerProfile(Int_t wl) {if(fLnFilteredPowerMap.count(wl))return fLnFilteredPowerMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the Ln(Binned Power) profile TGraph of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetLnBinnedPowerProfile(Int_t wl) {if(fLnBinnedPowerMap.count(wl))return fLnBinnedPowerMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the Binned Power profile TGraph of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetBinnedPowerProfile(Int_t wl) {if(fBinnedPowerMap.count(wl))return fBinnedPowerMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the Binned Power Deviation profile TGraph of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetBinnedPowerDevProfile(Int_t wl) {if(fBinnedPowerDevMap.count(wl))return fBinnedPowerDevMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the Binned Power Deviation Ratio profile TGraph of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetBinnedPowerDevRatioProfile(Int_t wl) {if(fBinnedPowerDevRatioMap.count(wl))return fBinnedPowerDevRatioMap[wl];
                                         else return 0;}

    /** @brief Returns a pointer to the extinction profile TGraph
     *  of the given wavelength for the given kind:
     * Total "T", Particles "P", Molecules "R"
     * 
     * @param wl the wavelength
     * @param scattering Total "T", Particles "P", Molecules "R"
     * @return TGraph *
     */
    TGraph *GetExtinctionProfile(Int_t wl, std::string scattering);

    /** @brief Returns a pointer to the total extinction profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetExtinctionProfile(Int_t wl) {return GetExtinctionProfile(wl,"T");}

    /** @brief Returns a pointer to the backscatter profile TGraph
     *  of the given wavelength for the given kind:
     * Total "T", Particles "P", Molecules "R"
     * 
     * @param wl the wavelength
     * @param scattering Total "T", Particles "P", Molecules "R"
     * @return TGraph *
     */
    TGraph *GetBackscatterProfile(Int_t wl, std::string scattering);

    /** @brief Returns a pointer to the total backscatter profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetBackscatterProfile(Int_t wl) {return GetBackscatterProfile(wl,"T");}


    /** @brief Returns a pointer to the model extinction profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetExtinctionModelProfile(Int_t wl) {if(fExtinctionModelMap.count(wl))return fExtinctionModelMap[wl];
                                                 else return 0;}
    /** @brief Returns a pointer to the Lidar ration profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetLidarRatioProfile(Int_t wl) {if(fLidarRatioMap.count(wl))return fLidarRatioMap[wl];
                                            else return 0;}

    /** @brief Returns a pointer to the opacity profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetOpacityProfile(Int_t wl) {if(fOpacityMap.count(wl))return fOpacityMap[wl];
                                            else return 0;}

    /** @brief Returns a pointer to the transmission profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetTransmissionProfile(Int_t wl) {if(fTransmissionMap.count(wl))return fTransmissionMap[wl];
                                            else return 0;}

    /** @brief Returns a pointer to the opacity model profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetOpacityModelProfile(Int_t wl) {if(fOpacityModelMap.count(wl))return fOpacityModelMap[wl];
                                            else return 0;}

    /** @brief Returns a pointer to the transmission model profile TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetTransmissionModelProfile(Int_t wl) {if(fTransmissionModelMap.count(wl))return fTransmissionModelMap[wl];
                                            else return 0;}


    /** @brief Returns a pointer to the Optical Depths values TGraph
     *  of the given wavelength
     * 
     * @param wl the wavelength
     * @return TGraph *
     */
    TGraph *GetODResults(Int_t wl) {if(fODResultsMap.count(wl))return fODResultsMap[wl];
                                            else return 0;}

    /** @brief Returns a pointer to the Angstroem Exponent profile TGraph
     * 
     * @return TGraph *
     */
    TGraph *GetAngstExpProfile() {return fAngstExp;}

    /** @brief Returns the vector of available wavelengths
     * 
     * @return std::vector<Int_t>
     */
    std::vector<Int_t> GetWavelengths() {return fWaveLengthVec;}

    /** @brief Returns a pointer to the low level canvas
     * 
     * @return TCanvas *
     */
    TCanvas* GetRawCanvas()            {return fRawCanvas;}

    /** @brief Returns a pointer to the background canvas
     * 
     * @return TCanvas *
     */
    TCanvas* GetBkgCanvas()            {return fBkgCanvas;}

    /** @brief Returns a pointer to the low level canvas
     * 
     * @return TCanvas *
     */
    TCanvas* GetFilterCanvas()            {return fFilterCanvas;}

    /** @brief Returns a pointer to the main canvas
     * 
     * @return TCanvas *
     */
    TCanvas* GetMainCanvas()            {return fMainCanvas;}

    /** @brief Returns a pointer to the extra canvas
     * 
     * @return TCanvas *
     */
    TCanvas* GetExtraCanvas()            {return fExtraCanvas;}

    /** @brief Returns a pointer to the summary canvas
     * 
     * @return TCanvas *
     */
    TCanvas* GetSummaryCanvas()            {return fSummaryCanvas;}
    
    /** @brief Return the value of the integrated opacity for the given wavelength
     * 
     * This actually is just a proxy to the Analyser.
     * 
     * @return Float_t
     */
    Float_t GetOD(Int_t wl)           {return fAnalyser->GetOD(wl);}

    /** @brief Return the value of the integrated opacity due to particles
     * for the given wavelength. 
     *  
     * Aeorosol Optical Depth
     * This actually is just a proxy to the Analyser.
     * 
     * @return Float_t
     */
    Float_t GetAOD(Int_t wl)           {return fAnalyser->GetAOD(wl);}

  private:
    /** @brief boolean to print some results on standard out if true */
    Bool_t fVerbose; //!
    
    /** @brief boolean save if altitudes have been offset */
    Bool_t fApplyOffset; //!

    /** @brief Altitude offset to be added to Analyser altitudes,
     *  this is convenient to compare to atmosphere transmission models.
     *  The offset is taken from Analyser->fLidarAltitude
     */
    Float_t fAltitudeOffset;

    /** @brief the run number */
    Int_t fRunNumber;

    /** @brief the sequence number */
    Int_t fSeqNumber;
        
    /** @brief Vector of available wave length */
    std::vector<Int_t> fWaveLengthVec;

    /** @brief Analyser contains data to be plotted */
    LidarTools::Analyser *fAnalyser;       

    /** @brief Map of TGraph raw data profiles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fRawProfileMap;

    /** @brief Map of TGraph bkg data profiles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fBkgProfMap;

    /** @brief Map of Histogram of the raw signal background
     * 
     * The key is the wavelength, the value is a pointer to the TH1F
     */ 
    std::map<Int_t, TH1F*> fBkgHistMap;

    /** @brief Map of TGraph of Ln(Raw Power)
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fLnRawPowerMap;

    /** @brief Map of TGraph of Ln(Filtered Power)
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fLnFilteredPowerMap;

    /** @brief Map of TGraph of Ln(BinnedPower)
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fLnBinnedPowerMap;

    /** @brief Map of TGraph of BinnedPower
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fBinnedPowerMap;

    /** @brief Map of TGraph of BinnedPowerDev
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fBinnedPowerDevMap;

    /** @brief Map of TGraph of BinnedPowerDevRatio
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fBinnedPowerDevRatioMap;

    /** @brief Map of TGraph of Atmosphere exctinction
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fExtinctionMap;

    /** @brief Map of TGraph of Atmosphere exctinction for Particles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fExtinctionMap_P;

    /** @brief Map of TGraph of Atmosphere exctinction for Molecules
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fExtinctionMap_M;

    /** @brief Map of TGraph of Atmosphere backscatter
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fBackscatterMap;

    /** @brief Map of TGraph of Atmosphere backscatter for Particles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fBackscatterMap_P;

    /** @brief Map of TGraph of Atmosphere backscatter for Molecules
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fBackscatterMap_M;

    /** @brief Map of TGraph of Exctinction-to-backscatter ratio
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fLidarRatioMap;

    /** @brief Map of TGraph of opacity profiles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fOpacityMap;

    /** @brief Map of TGraph of transmission profiles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fTransmissionMap;

    /** @brief Map of TGraph of Model atmosphere exctinction
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fExtinctionModelMap;

    /** @brief Map of TGraph of opacity model profiles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fOpacityModelMap;

    /** @brief Map of TGraph of transmission model profiles
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fTransmissionModelMap;

    /** @brief Map of TGraph of Optical Depth
     * 
     * The key is the wavelength, the value is a pointer to the TGraph
     */
    std::map<Int_t, TGraph*> fODResultsMap;
    
    /** @brief TGraph of Angstroem Exponent
     * 
     * a pointer to the TGraph
     */
    TGraph* fAngstExp;

    /** @brief Low level canvas to display basics distributions */
    TCanvas *fRawCanvas;

    /** @brief Low level Canvas to display background related distributions */
    TCanvas *fBkgCanvas;

    /** @brief Low level Canvas to display StdDev related distributions */
    TCanvas *fStdDevCanvas;

    /** @brief Low level canvas to display filter effect */
    TCanvas *fFilterCanvas;

    /** @brief Main canvas to display plots */
    TCanvas *fMainCanvas;

    /** @brief Extra canvas to display details */
    TCanvas *fExtraCanvas;

    /** @brief Summary canvas to display details */
    TCanvas *fSummaryCanvas;

    /** @brief Summary canvas to display details */
    TCanvas *fAngstExpCanvas;
    
    /** @brief Create a TGraph atmosphere profile */
    TGraph* AtmoGraph(TString, TString, Int_t);

  protected:
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::Plotter,1);
#endif

  }; // class

}; // namespace

#endif
