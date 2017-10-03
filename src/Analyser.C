/** @file Analyser.C
 *
 * @brief Analyser object implementation
 *
 * Class to drive the Lidar data analysis
 * 
 * @author Johan Bregeon
*/

#include <iostream> 
#include <cmath> 
#include <sstream> 


#include "Analyser.hh"

// Constructor
LidarTools::Analyser::Analyser(TArrayF range, std::map<Int_t, TArrayF> signalmap,
                               Bool_t verbose)
: fVerbose(verbose),
  fRunNumber(-99999),
  fSeqNumber(1),
  fTimeStamp(0),
  fConfig(0),
  fOverlap(0),
  fApplyOverlap(false),
  fAbsorp(0),
  fAtmoProfile(0)
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Constructor" << std::endl; 

  // Save raw data
  fRawRange=range;
  fSignalMap=signalmap;
}

// Destructor
LidarTools::Analyser::~Analyser()
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Destructor" << std::endl; 
delete fAbsorp;
delete fConfig;
delete fOverlap;
delete fAtmoProfile;

// @todo clear maps
fSignalMap.clear();
fQualityMap.clear();    
fBkgMap.clear();
fFullBkgMap.clear();
fReducedSignalMap.clear();
fPowMap.clear();
fLnPowMap.clear();
fFilteredPowMap.clear();
fLnFilteredPowMap.clear();
fBinnedPowMap.clear();
fBinnedPowDevMap.clear();
fParamAlpha0Map.clear();
fAlphaMap.clear();
fAlphaMap_M.clear();
fAlphaMap_P.clear();
fBetaMap.clear();
fBetaMap_M.clear();
fBetaMap_P.clear();
fOpacityMap.clear();
fOpacityMap_M.clear();
fOpacityMap_P.clear();
fTransmissionMap.clear();
fODMap.clear();
fODMap_M.clear();
fODMap_P.clear();
fODModelMap.clear();
fODModelMap_P.clear();
fAlphaModelMap.clear();
fOpacityModelMap.clear();
fTransmissionModelMap.clear();

}


// SetConfig
int LidarTools::Analyser::SetConfig()
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Set configuration"<< std::endl; 
  if(fConfig)
      fConfig->Reset();
  else
      fConfig = new ConfigHandler(fVerbose);
  int rc=StoreConfigLocally();
  return rc;
}

// SetConfig
int LidarTools::Analyser::SetConfig(char infilename[])
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Set configuration file: "
                         <<infilename<< std::endl; 
  SetConfig();
  fConfig->Read(infilename);
  int rc=StoreConfigLocally();
  return rc;
}

// Overwrite config parameters on the fly
int LidarTools::Analyser::OverwriteConfigParam(std::string key, std::string value)
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Overwrite configuration parameter: "
                         << key<<" = "<<value<< std::endl; 
  fConfig->SetParam(key,value);
  int rc=StoreConfigLocally();
  return rc;
}

int LidarTools::Analyser::StoreConfigLocally()
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Store configuration locally"<< std::endl; 
  fLidarAltitude  = fConfig->GetLidarAltitude();  //  1800 m
  fLidarTheta     = fConfig->GetLidarTheta();     //  1800 m
  fQualityThr     = fConfig->GetQualityThr();     //  -5.0 V 
  fParamAltMin    = fConfig->GetParamAltMin();    //   800 m   
  fParamAltMax    = fConfig->GetParamAltMax();    //  10000 m    
  fParamBkgMin    = fConfig->GetParamBkgMin();    //  20000 m   
  fParamBkgMax    = fConfig->GetParamBkgMax();    //  25000 m
  fParamBkgFFactor= fConfig->GetParamBkgFFactor();    //  1.0 (hopefully)
  fParamNBins     = fConfig->GetParamNBins();     // 100     
  fParamLogBins   = fConfig->GetParamLogBins();   // 1     
  fParamSGFilter  = fConfig->GetParamSGFilter();   // 0
  fParamR0_355    = fConfig->GetParamR0(355);        //  10000 m    
  fParamR0_532    = fConfig->GetParamR0(532);        //  10000 m    
  fParamKlett_k   = fConfig->GetParamKlett_k();   //   1     
  fParamKlett_l   = fConfig->GetParamKlett_l();   //   1     
  fTauAltMin      = fConfig->GetTauAltMin();      //   800   
  fTauAltMax      = fConfig->GetTauAltMax();      //   4000    
  fAlgName        = fConfig->GetAlgName();        // Inversion algorithm name
  fFernald84_Sp355  = fConfig->GetFernald_Sp(355);// Exctinction-to-backscatter ratio at 355 nm
  fFernald84_Sp532  = fConfig->GetFernald_Sp(532);// Exctinction-to-backscatter ratio at 532 nm
  fFernald84_sratio = fConfig->GetFernald_sratio();// sratio=1+Sp/Sr
  
  // Inversion optimization parameters
  fSNRatioThreshold = fConfig->GetSNRatioThreshold(); // 5
  fParamOptimizeR0  = fConfig->GetParamOptimizeR0();  // true
  fParamOptimizeAC  = fConfig->GetParamOptimizeAC();  // true
  fParamOptimizeAC_Hmin  = fConfig->GetParamOptimizeAC_Hmin();  // 6000 m or 4000 m
  fParamAlignCorr_355 = fConfig->GetParamAC(355); // 0.07;
  fParamAlignCorr_532 = fConfig->GetParamAC(532); // 0.00;

  // Correct range before processing indices
  CorrectRange();
  
  // Always re-Initialize inidices from new parameters
  int rc=InitIndices();
  
  if(rc>0){
      std::cout<<"[LidarTools::Analyser] Could not initialize indices... aborting."<<std::endl;
      return rc;
      }
  
  // To avoid destructing and reconstructing the same things
  // test if the data files have been changed
  // Absorption
  if(fAtmoAbsorption.compare(fConfig->GetAtmoAbsorption())!=0)
    {
    fAtmoAbsorption = fConfig->GetAtmoAbsorption(); // LidarTools/data/atm_trans_1800_1_10_0_0_1800.dat
    InitAtmoAbsorption();
    }
  // Atmospheric profile
  if(fAtmoFileName.compare(fConfig->GetAtmoProfile())!=0)
    {
    fAtmoFileName = fConfig->GetAtmoProfile();              // LidarTools/data/atmprof10.dat
    InitAtmoProfile();
    }
  // Overlap
  if(fOverlapFileName.compare(fConfig->GetOverlap())!=0)
    {    
    fOverlapFileName=fConfig->GetOverlap();        // OverlapFunction
    InitOverlap();
    }

  return 0;
}

// Update ConfigHandler config object from members
int LidarTools::Analyser::StoreConfigToHandler()
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Store configuration to hanlder"<< std::endl; 
  std::stringstream ss;
  ss<<fLidarAltitude;  
  fConfig->SetParam("LidarAltitude", ss.str()); 
  ss.str(std::string()); ss<<fLidarTheta;
  fConfig->SetParam("LidarTheta", ss.str());
  ss.str(std::string()); ss<<fQualityThr;
  fConfig->SetParam("QualityThr", ss.str());
  ss.str(std::string()); ss<<fParamAltMin;
  fConfig->SetParam("AltMin", ss.str());
  ss.str(std::string()); ss<<fParamAltMax;
  fConfig->SetParam("AltMax", ss.str());
  ss.str(std::string()); ss<<fParamBkgMin;
  fConfig->SetParam("BkgMin", ss.str());
  ss.str(std::string()); ss<<fParamBkgMax;
  fConfig->SetParam("BkgMax", ss.str());
  ss.str(std::string()); ss<<fParamBkgFFactor;
  fConfig->SetParam("BkgFudgeFactor", ss.str());
  ss.str(std::string()); ss<<fParamNBins;
  fConfig->SetParam("NBins", ss.str());
  ss.str(std::string()); ss<<fParamLogBins;
  fConfig->SetParam("LogBins", ss.str());
  ss.str(std::string()); ss<<fParamSGFilter;
  fConfig->SetParam("SGFilter", ss.str());
  ss.str(std::string()); ss<<fParamR0_355;
  fConfig->SetParam("R0_355", ss.str());
  ss.str(std::string()); ss<<fParamR0_532;
  fConfig->SetParam("R0_532", ss.str());
  ss.str(std::string()); ss<<fParamKlett_k;
  fConfig->SetParam("Klett_k", ss.str());
  ss.str(std::string()); ss<<fParamKlett_l;
  fConfig->SetParam("Klett_l", ss.str());
  ss.str(std::string()); ss<<fTauAltMin;
  fConfig->SetParam("TauAltMin", ss.str());
  ss.str(std::string()); ss<<fTauAltMax;
  fConfig->SetParam("TauAltMax", ss.str());
  ss.str(std::string()); ss<<fAlgName;
  fConfig->SetParam("AlgName", ss.str());
  ss.str(std::string()); ss<<fFernald84_Sp355;
  fConfig->SetParam("Fernald_Sp355", ss.str());
  ss.str(std::string()); ss<<fFernald84_Sp532;
  fConfig->SetParam("Fernald_Sp532", ss.str());
  ss.str(std::string()); ss<<fFernald84_sratio;
  fConfig->SetParam("Fernald_sratio", ss.str());

  // Inversion optimization parameters
  ss.str(std::string()); ss<<fSNRatioThreshold;
  fConfig->SetParam("SNRatioThreshold", ss.str());
  ss.str(std::string()); ss<<fParamOptimizeR0;
  fConfig->SetParam("OptimizeR0", ss.str());
  ss.str(std::string()); ss<<fParamOptimizeAC;
  fConfig->SetParam("OptimizeAC", ss.str());
  ss.str(std::string()); ss<<fParamOptimizeAC_Hmin;
  fConfig->SetParam("OptimizeAC_Hmin", ss.str());
  ss.str(std::string()); ss<<fParamAlignCorr_355;
  fConfig->SetParam("AlignCorr_355", ss.str());
  ss.str(std::string()); ss<<fParamAlignCorr_532;
  fConfig->SetParam("AlignCorr_532", ss.str());
    
  // Absorption
  ss.str(std::string()); ss<<fAtmoAbsorption;
  fConfig->SetParam("AtmoAbsorption", ss.str());
  ss.str(std::string()); ss<<fAtmoFileName;
  fConfig->SetParam("AtmoProfile", ss.str());
  ss.str(std::string()); ss<<fOverlapFileName;
  fConfig->SetParam("OverlapFunction", ss.str());
  
  return 0;
}

// InitAtmo - Create HESSAtmosphericAbsorption object
void LidarTools::Analyser::InitAtmoProfile()
{
  // Read input file - Altitude has to match file content - 1800 m
  // Do that to better intialized Klett to model
  if(fAtmoProfile)
    delete fAtmoProfile;
  if(!fAtmoFileName.empty()){
     if(fVerbose) std::cout << "[LidarTools::Analyser] Initializing atmosphere profile" << std::endl;
     fAtmoProfile = new AtmoProfile(fVerbose);
     fAtmoProfile->Read(fAtmoFileName, true);
     }
  else {
     if(fVerbose) std::cout << "[LidarTools::Analyser] No atmosphere profile given." << std::endl; 
  }
}

// InitAtmo - Create HESSAtmosphericAbsorption object
void LidarTools::Analyser::InitAtmoAbsorption()
{
  // Read input file - Altitude has to match file content - 1800 m
  // Do that to better intialized Klett to model
  if(fAbsorp)
    delete fAbsorp;
  if(!fAtmoAbsorption.empty()){
     if(fVerbose) std::cout << "[LidarTools::Analyser] Initializing atmospheric absorption" << std::endl;
  fAbsorp = new AtmoAbsorption(fAtmoAbsorption.c_str(), fLidarAltitude, true);
     }
  else {
     if(fVerbose) std::cout << "[LidarTools::Analyser] No atmosphere transmission given" << std::endl; 
  }
}

/** InitOverlap
 *
 * Create Overlap object
*/
void LidarTools::Analyser::InitOverlap()
{
  // Overlap function
  if(fOverlap)
    delete fOverlap;
    
  if(!fOverlapFileName.empty()){
     if(fVerbose) std::cout << "[LidarTools::Analyser] Initializing Overlap function" << std::endl;
     fOverlap = new LidarTools::Overlap(fOverlapFileName.c_str(), fVerbose);
     fApplyOverlap=true;     
     }
  else {
     if(fVerbose) std::cout << "[LidarTools::Analyser] No correction for Overlap function applied" << std::endl; 
     fApplyOverlap=false;     
  }

}

/** PrepareData
 *
 * Prepare data for one wave length
*/
int LidarTools::Analyser::PrepareData(Int_t wl)
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Preparing data for wavelength "<< wl << std::endl; 
  
  // Data reduction (subtract background)
  SubtractBackground(wl);
  // Compute power and Ln(power), and correct for the overlap function
  ComputePower(wl);
  // Filter noise if asked for
  if(fParamSGFilter)
     FilterPower(wl);
  // Rebin data
  RebinData(wl);
  
  return 0;
}



/** ProcessData
 *
 * Process data for one wave length
*/
int LidarTools::Analyser::ProcessData(Int_t wl)
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Processing wavelength "<< wl << std::endl; 

  int rc=0;
  // Basic data quality check
  if(CheckQuality(wl))
      {
      // Prepare data --- bkg, power, filtering, binning, SNRatio
      // First pass, use parameters from given configuration
      PrepareData(wl);
      // Optimize R0  - changes the value of fParamR0_wl
      if(fParamOptimizeR0)
        doOptimizeR0(wl);      
      
      // Optimize AC - changes the value of fParamFAC_wl
      if(fParamOptimizeAC)
        doOptimizeAC(wl);

      // Inversion
      if (fAlgName=="Klett")
              KlettInversion(wl);
      else if (fAlgName=="Fernald84")
              Fernald84Inversion(wl);
      else if (fAlgName=="Aeronet")
              AeronetInversion(wl);
      else{
           std::cout << "[LidarTools::Analyzer] unknown inversion required" << std::endl; 
           exit(2);
	      }
      // Atmosphere opacity profile, Tau4 and AOD
      ComputeAtmosphereOpacity(wl);
      // Atmosphere transmission profile
      ComputeAtmosphereTransmission(wl);
      
      // Dump real config
      StoreConfigToHandler();
      }
  else{
      std::cout << "[LidarTools::ProcessData] Data quality is not good for "
                << wl <<" nm ... aborting." << std::endl;
      rc=1; 
      }
  return rc;
}

/** ProcessData
 *
 * Process data for all wave lengths
*/
int LidarTools::Analyser::ProcessData()
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Start Lidar data processing" << std::endl; 
  int rc=0;
  std::map<Int_t, TArrayF>::iterator it;
  for (it=fSignalMap.begin(); it!=fSignalMap.end(); ++it)
    {
    Int_t wl=it->first;
    std::cout << "[LidarTools::Analyser] Processing run "<<fRunNumber
              <<"-"<<fSeqNumber<<" @ " << wl <<" nm"<< std::endl;
    rc+=ProcessData(wl);
    fWaveLengthVec.push_back(wl);
   }
   return rc;
}


// Correct range for zenith angle pointing
void LidarTools::Analyser::CorrectRange()
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Correct range"<< std::endl; 
  
  fRange.Reset();
  fRange.Set(fRawRange.GetSize());
  float correction=cos(fLidarTheta*3.14159/180.); // correction = 0.9659;
  for(int i=0; i<fRawRange.GetSize(); i++)
     fRange[i]=fRawRange[i]*correction;
}

// Init
int LidarTools::Analyser::InitIndices()
{

  if(fVerbose) std::cout << "[LidarTools::Analyser] Get indices, and initialize variables" << std::endl; 
  
  // First check vector length
  if (fRange.GetSize()<10)
    return 3;
  
  // Clear maps and arrays
  fBkgMap.clear();
  fAltitude.Reset();
  
  // Initialize bkg map
  std::map<Int_t, TArrayF>::iterator it;
  for (it=fSignalMap.begin(); it!=fSignalMap.end(); ++it)
    {
    Int_t wl=it->first;
    fBkgMap[wl]=0.;
    }

  // Get Size and Indices
  int k=0;
  int altminindex=0, altmaxindex=0, altsize=0;
  for (int i=0; i<fRange.GetSize(); i++){
//    if (fRange[i]>=fParamBkgMin/1000. && fRange[i]<=fParamBkgMax/1000.){
//          k++;
//          }
    if (fRange[i]>=fParamAltMin/1000. && altminindex==0) altminindex=i;
    if (fRange[i]>=fParamAltMax/1000. && altmaxindex==0) altmaxindex=i;      
  }
  
//  altmaxindex=fRange.GetSize()-k;
  altsize=altmaxindex-altminindex+1;
  
  // Store size and indices
  fN=altsize;
  fAltMinIndex=altminindex;
  fAltMaxIndex=altmaxindex;
    
  // Initialize Altitude Array in meters
  fAltitude.Set(fN);
  k=0;
  for (int i=fAltMinIndex; i<=fAltMaxIndex; i++){
      fAltitude.AddAt(fRange[i]*1000., k);
      k++;
    }  

  return 0;
}

// Check data quality
Bool_t LidarTools::Analyser::CheckQuality(Int_t wl)
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Check data quality: look for -5.0 V peak" << std::endl; 
  // Input is raw signal
  Float_t min=999.;
  TArrayF signal=fSignalMap[wl];
  for (int i=0; i<signal.GetSize(); i++){
      if(signal[i]<min){
          min=signal[i];
          }
      }

  if(min<fQualityThr){
      fQualityMap[wl]=true;
      return true;
      }
  else{
      fQualityMap[wl]=false;
      return false;
      }
}

// Background subtraction
void LidarTools::Analyser::SubtractBackground(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Subtract background" << std::endl; 
  // Input is raw signal
  TArrayF signal=fSignalMap[wl];
  
  // Estimate background
  // Watch out that fRange is in km and BkgMin/Max are in meters
  Float_t bkgvalue=0.;
  int k=0;
  for (int i=0; i<fRange.GetSize(); i++){
    if (fRange[i]>=fParamBkgMin/1000. && fRange[i]<=fParamBkgMax/1000.){
      bkgvalue+=signal[i];
      k++;
    }
  }
  bkgvalue/=k;
  // Apply background fudge factor
  bkgvalue*=fParamBkgFFactor;
  
  //Copy background (2nd loop!), no fudge factor
  TArrayF fullbkg(k);
  k=0;
  for (int i=0; i<fRange.GetSize(); i++){
    if (fRange[i]>=fParamBkgMin/1000. && fRange[i]<=fParamBkgMax/1000.){
      fullbkg.AddAt(signal[i], k);
      k++;
      }
  }
  
  // Store bkg value in map
  fFullBkgMap[wl]=fullbkg;
  fBkgMap[wl]=bkgvalue;
  
  // Subtract Background and create sub-array
  // Output is reduced signal
  k=0;
  TArrayF data(fN);
  for (int i=fAltMinIndex; i<=fAltMaxIndex; i++){
      data.AddAt(fabs(signal[i]-bkgvalue), k);
      k++;
    }
  
  //Store array in map
  fReducedSignalMap[wl]=data;
}


// Compute power and Ln(power)
void LidarTools::Analyser::ComputePower(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Compute power and Ln(power)" << std::endl;
  // Input is reduced signal
  TArrayF rsignal=fReducedSignalMap[wl];
  
  // Output arrays
  TArrayF pw(fN);
  
  // Initialize local variable for overlap function correction
  Float_t corrOver=1.;
  // Local variable to get the range from the altitude
  float RangeToAltitude=GetRangeToAltitude(); //cos(fLidarTheta*3.14159/180.); // correction = 0.9659;

  // Compute power and Ln(power)
  for (int i=0; i<fN; i++){
    // Overlap function with altitude in meters
    if (fApplyOverlap)
        corrOver=fOverlap->GetOverlap(fAltitude[i]);
    // std::cout<<corr<<std::endl;
    // Power - corrected for overlap
    //       - consider the effective range and not the altitude
    Float_t range=fAltitude[i]/RangeToAltitude;
    pw.AddAt(rsignal[i]* range * range / corrOver , i);
    }

 // Store arrays in maps
 fPowMap[wl]=pw;
}


// Filter power
void LidarTools::Analyser::FilterPower(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Filter signal power and compute Ln(filtered(power))" << std::endl;
  // Input is reduced signal
  TArrayF pw=fPowMap[wl];
  
  // Output arrays
  TArrayF filtered(fN);
  
  // Define parameters - hardcoded for now
  int nl=10, nr=10, m=3; // left points, right points, polynome degree
  
  // Create SavGolFilter object
  LidarTools::SavGolFilter *savgol = new LidarTools::SavGolFilter(fVerbose);
  // initialize
  savgol->Init(pw.GetArray(),pw.GetSize());
  // filter !
  savgol->MoveWindow(nl, nr, m);
  // save data locally
  std::vector<float> meanVec=savgol->GetMeanVec();
  std::vector<float>::iterator val;
  int i=0;
  for (val=meanVec.begin(); val!=meanVec.end(); ++val){
        //std::cout<<(*val)<<" ";
        filtered.AddAt(*val , i);
        i++;
        }  
  // clean up
  delete savgol;
  
  
 // Store arrays in maps
 fFilteredPowMap[wl]=filtered;
}


// Rebin data proxy
void LidarTools::Analyser::RebinData(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Rebin data" << std::endl;
  if(fParamLogBins)
    RebinDataLog(wl);
  else
    //RebinDataLinear(wl);
    RebinDataGAF(wl);
}

// Rebin data log
void LidarTools::Analyser::RebinDataLog(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Rebin data logarithmicly" << std::endl;
  // Rebin data
  // get bin width in Log scale
  float BinLnAltWidth=(log(fParamAltMax)-log(fParamAltMin))/fParamNBins;
  // bins edges and center array s
  fBinsAltitude.Set(fParamNBins+1);
  fBinsCenterAltitude.Set(fParamNBins);
  for(UInt_t i=0; i<fParamNBins+1; i++){
      fBinsAltitude.AddAt( exp(log(fParamAltMin)+i*BinLnAltWidth), i );
      if(i>0)
        fBinsCenterAltitude.AddAt( (fBinsAltitude[i-1]+fBinsAltitude[i])/2., i-1);
    }
  
  // Input is power signal or filtered power signal
  TArrayF pw;
  if(fParamSGFilter)
      pw=fFilteredPowMap[wl];
  else
      pw=fPowMap[wl];
  // Output is binned power arrays
  TArrayF binpw(fParamNBins);
  
  // locals
  Float_t suwpw=0.;
  Int_t nPoints=0;
  Float_t kAlt=0.;
  // Binning
  for(int i=0; i<fAltitude.GetSize(); i++){
    if(fAltitude[i]>fBinsAltitude[kAlt]){
      suwpw+=pw[i];
      nPoints+=1;
      }
    if( (fAltitude[i]>fBinsAltitude[kAlt+1]) ||
        (i==(fAltitude.GetSize()-1)) ){
      binpw.AddAt(suwpw/nPoints, kAlt);
      suwpw=pw[i];
      nPoints=1;
      kAlt+=1;
      if(kAlt==fParamNBins) break;
      }
    }

  // Store array in binned power map
  fBinnedPowMap[wl]=binpw;
  
}

// Rebin data linearly Gliding Average Filter
void LidarTools::Analyser::RebinDataGAF(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Rebin data linearly using a gliding average filter" << std::endl;
  // first need to truncate fPowMap and fAltitude to AltMin,AltMax - Done in InitIndices

  // Start the gliding filter
  GlidingAveFilter *gaf=new GlidingAveFilter(fVerbose);
  // Init gaf
  gaf->Init(fAltitude.GetArray(), fAltitude.GetSize());
  // Need to calculate the window width from fParamNBins
  // get required bin width
  float BinAltWidth=(fParamAltMax-fParamAltMin)/fParamNBins;
  // get current bin width
  float bw=fAltitude[1]-fAltitude[0]; //~ 2.5 meters
  // calculate window width, consider overlapping bins
  float nww=ceil(BinAltWidth/bw)*2;
  //fParamNBins+=nww;
  // Rebin the altitude here
  gaf->MoveWindow(nww);
  // Save results
  std::vector<float> meanVec=gaf->GetMeanVec();
  std::vector<float>::iterator val;
  int k=0;
  fParamNBins=meanVec.size();
  fBinsCenterAltitude.Set(fParamNBins);
  fBinsAltitude.Set(fParamNBins+1);
  for (val=meanVec.begin(); val!=meanVec.end(); ++val){
        //std::cout<<(*val)<<std::endl;
        fBinsCenterAltitude[k]=(*val);
        k++;              
        }
  // and calculate bins lower end
  float awidth=fBinsAltitude[1]-fBinsAltitude[0];
  for(k=0; k<fBinsCenterAltitude.GetSize(); k++)
      fBinsAltitude[k]=fBinsCenterAltitude[k]-awidth/2.;        
  fBinsAltitude[fParamNBins]=fBinsCenterAltitude[fParamNBins-1]+awidth/2.;
  delete gaf;
  
  
  // Rebin Power - Input is power signal or filtered power signal
  TArrayF pw;
  if(fParamSGFilter)
      pw=fFilteredPowMap[wl];
  else
      pw=fPowMap[wl];

  // Rebin now
  GlidingAveFilter *gaf2=new GlidingAveFilter(fVerbose);
  gaf2->Init(pw.GetArray(), pw.GetSize());
  gaf2->MoveWindow(nww);
  // store results - mean and standard deviation
  TArrayF binpw(fParamNBins);
  TArrayF binpwdev(fParamNBins);
  std::vector<float> meanVec2=gaf2->GetMeanVec();  
  k=0;
  for (val=meanVec2.begin(); val!=meanVec2.end(); ++val){
        //std::cout<<(*val)<<std::endl;
        binpw[k]=(*val);      
        k++;
        }
  std::vector<float> devVec2=gaf2->GetStdDevVec();  
  k=0;
  for (val=devVec2.begin(); val!=devVec2.end(); ++val){
        //std::cout<<(*val)<<std::endl;
        binpwdev[k]=(*val);      
        k++;
        }
  delete gaf2;


  // Store array in binned power map
  fBinnedPowMap[wl]=binpw;
  fBinnedPowDevMap[wl]=binpwdev;

}

// Rebin data
// Simple linear binning
void LidarTools::Analyser::RebinDataLinear(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Rebin data linearly" << std::endl;
  // first need to truncate fPowMap and fAltitude to AltMin,AltMax
  
  // Need to calculate the window width from fParamNBins
  // get required bin width
  float BinAltWidth=(fParamAltMax-fParamAltMin)/fParamNBins;
  
  // Rebin data
  // bins edges and center array s
  fBinsAltitude.Set(fParamNBins+1);
  fBinsCenterAltitude.Set(fParamNBins);
  for(UInt_t i=0; i<fParamNBins+1; i++){
      fBinsAltitude.AddAt(fParamAltMin+i*BinAltWidth, i);
      if(i>0)
        fBinsCenterAltitude.AddAt( (fBinsAltitude[i-1]+fBinsAltitude[i])/2., i-1);
    }
  
  // Rebin Power - Input is power signal or filtered power signal
  TArrayF pw;
  if(fParamSGFilter)
      pw=fFilteredPowMap[wl];
  else
      pw=fPowMap[wl];

  // Output is binned power arrays
  TArrayF binpw(fParamNBins);
  
  // locals
  Float_t suwpw=0.;
  Int_t nPoints=0;
  Float_t kAlt=0.;
  // Binning
  for(int i=0; i<fAltitude.GetSize(); i++){
    if(fAltitude[i]>fBinsAltitude[kAlt]){
      suwpw+=pw[i];
      nPoints+=1;
      }
    if( (fAltitude[i]>fBinsAltitude[kAlt+1]) ||
        (i==(fAltitude.GetSize()-1)) ){
      binpw.AddAt(suwpw/nPoints, kAlt);
      suwpw=pw[i];
      nPoints=1;
      kAlt+=1;
      if(kAlt==fParamNBins) break;
      }
    }

  // Store array in binned power map
  fBinnedPowMap[wl]=binpw;

}

/** doOptimizeR0
 *
 * Optmize R0 so that S/N ratio be above threshold for one wave length
*/
int LidarTools::Analyser::doOptimizeR0(Int_t wl)
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Optimizie R0 for wavelength "<< wl << std::endl; 
  // If SNR is not good because R0 is too high
  // adjust R0 to a lower value where SNR>5
  // Verify Signal/Noise ratio
  //float_t SNRAtR0 = VerifySNRatio(wl);
  int AlphaNBins=fParamNBins;
  // Local config
  float_t paramR0=GetParamR0(wl);
  while(fBinsAltitude[AlphaNBins]>paramR0)AlphaNBins--;
      float_t SNRAtR0 = GetBinnedPower(wl)[AlphaNBins]/
                        GetBinnedPowerDev(wl)[AlphaNBins];                      

  std::cout<<"[LidarTools::Analyser] S/N Ratio at R0 = "<<paramR0<<" m,  is "
           <<SNRAtR0<<" while "<<fSNRatioThreshold <<" is required."<<std::endl;                   

  if(SNRAtR0<fSNRatioThreshold) {
	std::cout<<"[LidarTools::Analyser] S/N Ratio is too low, adjusting"<<std::endl;
	while((GetBinnedPower(wl)[AlphaNBins]/
	       GetBinnedPowerDev(wl)[AlphaNBins])<fSNRatioThreshold)
		AlphaNBins--;
	// Need to set global value fParamR0_wl
	paramR0=fBinsAltitude[AlphaNBins];
	SNRAtR0 = GetBinnedPower(wl)[AlphaNBins]/GetBinnedPowerDev(wl)[AlphaNBins];
    std::cout<<"[LidarTools::Analyser] New R0 = "<<paramR0
	        <<" m, and S/N Ratio is " << SNRAtR0 <<std::endl;
    // Store parameter value in local member -- config not updated !	
    SetParamR0(wl, paramR0);
	}

  return 0;
}

/** doOptimizeAC
 *
 * Optmize Mis-Alignment correction factor for the given wave length
*/
int LidarTools::Analyser::doOptimizeAC(Int_t wl)
{
  if(fVerbose) std::cout << "[LidarTools::Analyser] Optimize AC for wavelength "<< wl
                         << " from R0 down to "<<fParamOptimizeAC_Hmin<<" m"<< std::endl; 
  Int_t N=21, iMin=0;
  TArrayD res(N);
  Double_t min=1.;
  for(Int_t i=0; i<N; i++){
     res[i]=PureRayleighInversion(wl, i*0.01);
     if(res[i]<min){
       min=res[i];
       iMin=i;
       }
     }
  
//  if(fVerbose)
    std::cout << "[LidarTools::Analyser] AC Correction factor is "<< iMin<<"%"<< std::endl;
  // Store parameter value in local member -- config not updated !
  SetParamFAC(wl, iMin*0.01);

  return 0;   
}

// Pure Rayleigh inversion to optimize mis-alignment correction factor
Float_t LidarTools::Analyser::PureRayleighInversion(Int_t wl, Float_t AlCorr)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Pure Rayleigh inversion with Corr = "
                       <<AlCorr << std::endl;
  // Params
  Float_t Sr = 8.*3.14159/3.;  // 8.37 = Lidar Ratio alpha/beta for molecules = Rayleigh
  Float_t Sp = Sr;             // --> pure Rayleigh hypothesis !
  Float_t sratio=1.;           //  1 means no aerosol at R0
    
  // Look for NBins for Alpha and closest bin to reference altitude r0
  Int_t AlphaNBins=fParamNBins;
  while(fBinsAltitude[AlphaNBins]>GetParamR0(wl))
	   AlphaNBins--;

  // Input is binned power - need 2 copies
  TArrayF binpwraw=fBinnedPowMap[wl];  
  TArrayF binpw   =fBinnedPowMap[wl];

  // Output: Total Extinction and Backscatter
  TArrayF alpha(AlphaNBins);
  TArrayF beta(AlphaNBins);
  // Extinction and Backscatter for molecules (Rayleigh)
  TArrayF alpha_m(AlphaNBins);
  TArrayF beta_m(AlphaNBins);
  // Extinction and Backscatter for particles (Mie)
  TArrayF alpha_p(AlphaNBins);
  TArrayF beta_p(AlphaNBins);
    
  // initialize at R
  // Here R0 is in meters above sea level, and wl are in nm
  // Take nearest altitude bin to R0 for initialization
  Float_t altitude_R0 = (fBinsCenterAltitude[AlphaNBins-1]+fBinsCenterAltitude[AlphaNBins-2])/2.;
  Float_t alpha0  = fAtmoProfile->Extinction(wl, altitude_R0+fLidarAltitude);
  Float_t beta0   = alpha0/Sr; // pure Rayleigh
    
  // Reference values
  alpha_m[AlphaNBins-1] = alpha0;
  beta_m[AlphaNBins-1]  = beta0;
  beta_p[AlphaNBins-1]  = beta_m[AlphaNBins-1] * (sratio-1);
  alpha_p[AlphaNBins-1] = beta_p[AlphaNBins-1]*Sp;
  alpha[AlphaNBins-1]   = alpha_m[AlphaNBins-1]+alpha_p[AlphaNBins-1];
  beta[AlphaNBins-1]    = beta_m[AlphaNBins-1]+beta_p[AlphaNBins-1];
   
  // Inversion
  for(int i=AlphaNBins-2; i>=0; i--){	  
     // altitude bin width
     Float_t atmoSlabThickness=(fBinsCenterAltitude[i+1]-fBinsCenterAltitude[i])/GetRangeToAltitude();
     // average altitude
     Float_t altitude=(fBinsCenterAltitude[i+1]+fBinsCenterAltitude[i])/2.;
     
     // Rayleigh
     alpha_m[i]= fAtmoProfile->Extinction(wl, altitude+fLidarAltitude);
     beta_m[i] = alpha_m[i]/Sr;

     // A an intermediate integral
     Float_t A = (Sp-Sr)*(beta_m[i]+beta_m[i+1])*atmoSlabThickness;
     
     // Tweak signal for mis-alignment     
//     binpw[i] = binpwraw[i] * (1 + AlCorr*(altitude_R0-altitude)/1000. );         
//     binpw[i] = binpwraw[i] * (1 + AlCorr*(10000-altitude)/1000.);
     binpw[i] = binpwraw[i] * (1 + AlCorr*sqrt(abs(10000.-fLidarAltitude-altitude)/1000.) );

     // final total backscatter formula
     Float_t num = binpw[i]*exp(+A);
     Float_t denom_1 = binpw[i+1]/beta[i+1];
     Float_t denom_2 = Sp*(binpw[i+1]+binpw[i]*exp(+A))*atmoSlabThickness;

     beta[i] = num/(denom_1+denom_2);
     
     // Derive extinction and backscatter for particles
     beta_p[i]  = beta[i]-beta_m[i];
     alpha_p[i] = Sp*beta_p[i];
     
     // Derive total extinction
     alpha[i] = alpha_m[i]+alpha_p[i];     
     }
  
  // now calculate residuals down to 6000 m or 4000 m
  Double_t residuals=0.;
  Int_t i=AlphaNBins-1;  
  while((fBinsCenterAltitude[i]+fLidarAltitude>fParamOptimizeAC_Hmin))
    {
     residuals += alpha_p[i]*alpha_p[i]; // to be seen mathematically as (alpha_alpha_m)^2
     i--;
    }
  if(fVerbose) std::cout << "[LidarTools::Analyser] Pure Rayleigh inversion residuals = "
                         <<residuals/(AlphaNBins-1-i)<< std::endl;
  
  // return the mean of residuals
  return residuals/(AlphaNBins-1-i);
}

// Klett inversion
void LidarTools::Analyser::KlettInversion(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Klett inversion" << std::endl;
  // Klett inversion
  // Look for NBins for Alpha and closest bin to reference altitude r0
  int AlphaNBins=fParamNBins;
  while(fBinsAltitude[AlphaNBins]>GetParamR0(wl))AlphaNBins--;
  
  // Input is binned power
  TArrayF binpw=fBinnedPowMap[wl];
  // Output is Extinction and Backscatter
  TArrayF alpha(AlphaNBins);
  TArrayF beta(AlphaNBins);
  // Total extinction from model
  TArrayF alpha_model(AlphaNBins);


  // locals
  Float_t pw_m=0., alpha_m=0., int_pw_m=0.;
    
  // initialize at R0
  // Here R0 is in meters above sea level, and wl are in nm
  // Take nearest altitude bin to R0 for initialization
  Float_t altitude=(fBinsCenterAltitude[AlphaNBins-1]+fBinsCenterAltitude[AlphaNBins-2])/2.;
  Float_t alpha0=fAbsorp->Extinction(wl, altitude+fLidarAltitude, 1.);
  // Store alpha0 in map
  fParamAlpha0Map[wl]=alpha0;
  
  if(fVerbose)
    {
     std::cout<<"[LidarTools::Analyser] Initialization R0="<<GetParamR0(wl)<<" m"<<std::endl;
     std::cout<<"                       nearest bin is at "<< altitude <<" m"<<std::endl;
     std::cout<<"                       a0_wl1="<< alpha0 <<" m^-1"<<std::endl;
    }

  alpha.AddAt(alpha0, AlphaNBins-1);

  // Old k=1 code
//   for(int i=AlphaNBins-2; i>=0; i--){
//     pw_m=binpw[i+1];
//     alpha_m=alpha[i+1];
//     int_pw_m=(binpw[i+1]+binpw[i])/2.*(fBinsCenterAltitude[i+1]-fBinsCenterAltitude[i]);
//     alpha.AddAt(binpw[i]/(pw_m/alpha_m-2*int_pw_m), i);
//     }

  // Integrate from top with Klett k variable
  for(int i=AlphaNBins-2; i>=0; i--){
    pw_m=binpw[i+1];
    alpha_m=alpha[i+1];
    Float_t atmoSlabThickness=(fBinsCenterAltitude[i+1]-fBinsCenterAltitude[i])/GetRangeToAltitude();
    int_pw_m=( pow(binpw[i+1],1/fParamKlett_k) + pow(binpw[i],1/fParamKlett_k) )/2.*atmoSlabThickness;
    
    // alpha formula
    alpha.AddAt( pow(binpw[i],1/fParamKlett_k) /( pow(pw_m,1/fParamKlett_k) /alpha_m-2*int_pw_m), i);
   
    // for this simple Klett: beta=l*alpha^k 
    beta.AddAt(fParamKlett_l*pow(alpha[i],fParamKlett_k), i);
    
    // Get expected model extinction -- not used here but good for plotting
    alpha_model[i] = fAbsorp->Extinction(wl, altitude+fLidarAltitude, 1.);
    }
    
  fAlphaMap[wl]= alpha;
  fBetaMap[wl] = beta;
  
  // Store atmosphere extinction model
  fAlphaModelMap[wl]  = alpha_model;

}

// Fernald inversion
void LidarTools::Analyser::Fernald84Inversion(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Fernald inversion" << std::endl;
  // Params
  Float_t Sr = 8.*3.14159/3.;       // 8.37 = Lidar Ratio alpha/beta for molecules = Rayleigh
  Float_t Sp = GetParamFSp(wl);     // Lidar Ratio alpha/beta for particles = Mie 
  Float_t sratio=fFernald84_sratio; // Scattering ratio = 1+ beta_p/beta_m
                                    // 1 means no aerosol at R0, then has to be greater than 1
                                    // 1.01 from ergastiria/datapros20080421/inputs/klett_inputs.txt
  Float_t AlCorr=GetParamFAC(wl);   // Mis-alignment correction factor in %/1000m
                                    // 0 means no correction

//   Estimate srtatio from Model assuming an aerosol type (Sp)
//   alphaMmodel = alpha0
//   alphaPmodel = alpha0model-alpha0
//   betaMmodel = beta0
//   betaPmodel = alphaPmodel/Sp
//   sratio = 1+ betaPmodel/betaMmodel
 
    
  // Look for NBins for Alpha and closest bin to reference altitude r0
  Int_t AlphaNBins=fParamNBins;
  while(fBinsAltitude[AlphaNBins]>GetParamR0(wl))
	   AlphaNBins--;

  // Input is binned power - need 2 copies
  TArrayF binpwraw=fBinnedPowMap[wl];  
  TArrayF binpw   =fBinnedPowMap[wl];

  // Output: Total Extinction and Backscatter
  TArrayF alpha(AlphaNBins);
  TArrayF beta(AlphaNBins);
  // Extinction and Backscatter for molecules (Rayleigh)
  TArrayF alpha_m(AlphaNBins);
  TArrayF beta_m(AlphaNBins);
  // Extinction and Backscatter for particles (Mie)
  TArrayF alpha_p(AlphaNBins);
  TArrayF beta_p(AlphaNBins);
  // Total extinction from model
  TArrayF alpha_model(AlphaNBins);

    
  // initialize at R
  // Here R0 is in meters above sea level, and wl are in nm
  // Take nearest altitude bin to R0 for initialization
  Float_t altitude_R0 = (fBinsCenterAltitude[AlphaNBins-1]+fBinsCenterAltitude[AlphaNBins-2])/2.;
  Float_t alpha0model  = fAbsorp->Extinction(wl, altitude_R0+fLidarAltitude, 1.);
  Float_t alpha0  = fAtmoProfile->Extinction(wl, altitude_R0+fLidarAltitude);
  Float_t beta0   = alpha0/Sr; // pure Rayleigh
    
  // Store alpha0 in map
  fParamAlpha0Map[wl]=alpha0;
  
  if(fVerbose)
    {
     std::cout<<"[LidarTools::Analyser] Initialization R0="<<GetParamR0(wl)<<" m"<<std::endl;
     std::cout<<"                       nearest bin is at "<< altitude_R0 <<" m"<<std::endl;
     std::cout<<"                       a0_wl1="<< alpha0 <<" m^-1"<<std::endl;
    }
  
  // Reference values
  alpha_m[AlphaNBins-1] = alpha0;
  beta_m[AlphaNBins-1]  = beta0;
  beta_p[AlphaNBins-1]  = beta_m[AlphaNBins-1] * (sratio-1);
  alpha_p[AlphaNBins-1] = beta_p[AlphaNBins-1]*Sp;
  alpha[AlphaNBins-1]   = alpha_m[AlphaNBins-1]+alpha_p[AlphaNBins-1];
  beta[AlphaNBins-1]    = beta_m[AlphaNBins-1]+beta_p[AlphaNBins-1];
  // Model
  alpha_model[AlphaNBins-1] = alpha0model;
   
  // Inversion
  for(int i=AlphaNBins-2; i>=0; i--){	  
     // altitude bin width
     Float_t atmoSlabThickness=(fBinsCenterAltitude[i+1]-fBinsCenterAltitude[i])/GetRangeToAltitude();
     // average altitude
     Float_t altitude=(fBinsCenterAltitude[i+1]+fBinsCenterAltitude[i])/2.;

     // Get expected model extinction -- not used here but good for plotting
     alpha_model[i]= fAbsorp->Extinction(wl, altitude+fLidarAltitude, 1.);
     
     // Rayleigh from analytical formula -- actually from Konrad atmosphere table
     //   alpha_m[i]= fAbsorp->Extinction(wl, altitude+fLidarAltitude, 1.);
     alpha_m[i]= fAtmoProfile->Extinction(wl, altitude+fLidarAltitude);
     beta_m[i] = alpha_m[i]/Sr;

     // A an intermediate integral
     Float_t A = (Sp-Sr)*(beta_m[i]+beta_m[i+1])*atmoSlabThickness;
     
     // Tweak signal for mis-alignment     
//     binpw[i] = binpwraw[i] * (1 + AlCorr*(altitude_R0-altitude)/1000. );         
//     binpw[i] = binpwraw[i] * (1 + AlCorr*(10000-altitude)/1000.);
     binpw[i] = binpwraw[i] * (1 + AlCorr*sqrt(abs(10000.-fLidarAltitude-altitude)/1000.) );

     // final total backscatter formula
     Float_t num = binpw[i]*exp(+A);
     Float_t denom_1 = binpw[i+1]/beta[i+1];
     Float_t denom_2 = Sp*(binpw[i+1]+binpw[i]*exp(+A))*atmoSlabThickness;
     //std::cout<<"[LidarTools::Analyser] binpw[i],num,d1,d2: "<<i<<" "<<binpw[i]<<" "<<num<<" "<<denom_1<<" "<<denom_2<<std::endl;

     beta[i] = num/(denom_1+denom_2);
     
     // Derive extinction and backscatter for particles
     beta_p[i]  = beta[i]-beta_m[i];
//      if (beta_p[i]<0){
// 	   beta_p[i]=beta_m[i]*(sratio-1);
// 	   beta[i]=beta_p[i]+beta_m[i];
// 	   if(fVerbose) std::cout<<"[LidarTools::Analyser] Fernald84: negative beta_p, assume pure Rayleigh for bin "<<i
// 	            <<std::endl;
// 	   }
     alpha_p[i] = Sp*beta_p[i];
     
     // Derive total extinction
     alpha[i] = alpha_m[i]+alpha_p[i];
     
     // Real Debug
     //std::cout<<"Fernald "<<i<<" "<<atmoSlabThickness<<" "<<alpha[i]<<" "<<alpha_m[i]<<" "<<alpha_p[i]<<" "
     //         <<beta[i]<< " "<<beta_m[i]<<" "<<beta_p[i]<<std::endl;     
     }
  
  // Store results in maps  
  fAlphaMap[wl]  = alpha;
  fBetaMap[wl]   = beta;
  fAlphaMap_P[wl]= alpha_p;
  fBetaMap_P[wl] = beta_p;
  fAlphaMap_M[wl]= alpha_m;
  fBetaMap_M[wl] = beta_m;
  
  // Store atmosphere extinction model
  fAlphaModelMap[wl]  = alpha_model;
  
}

// Aeronet inversion 
void LidarTools::Analyser::AeronetInversion(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Aeronet inversion" << std::endl;
  // Params
  Float_t Sr=8.*3.14159/3.; // 8.37 = Lidar Ratio alpha/beta for molecules = Rayleigh
  Float_t Sp=0.;            // Lidar Ratio alpha/beta for particles = Mie 
  if(wl==355)
    Sp=fFernald84_Sp355;
  else if (wl==532)
    Sp=fFernald84_Sp532;
  else
    Sp=50;
    
  // Look for NBins for Alpha and closest bin to reference altitude r0
  Int_t AlphaNBins=fParamNBins;
  while(fBinsAltitude[AlphaNBins]>GetParamR0(wl))AlphaNBins--;
  
  // Input is binned power
  TArrayF binpw=fBinnedPowMap[wl];
  // Output: Total Extinction and Backscatter
  TArrayF alpha(AlphaNBins);
  TArrayF beta(AlphaNBins);
  // Extinction and Backscatter for molecules (Rayleigh)
  TArrayF alpha_m(AlphaNBins);
  TArrayF beta_m(AlphaNBins);
  // Extinction and Backscatter for particles (Mie)
  TArrayF alpha_p(AlphaNBins);
  TArrayF beta_p(AlphaNBins);
  // Total extinction from model
  TArrayF alpha_model(AlphaNBins);

    
  // initialize at R
  // Here R0 is in meters above sea level, and wl are in nm
  // Take nearest altitude bin to R0 for initialization
  Float_t altitude= (fBinsCenterAltitude[AlphaNBins-1]+fBinsCenterAltitude[AlphaNBins-2])/2.;
  Float_t alpha0model  = fAbsorp->Extinction(wl, altitude+fLidarAltitude, 1.);
  Float_t alpha0  = fAtmoProfile->Extinction(wl, altitude+fLidarAltitude);
  Float_t beta0   = alpha0/Sr; // pure Rayleigh
  
  // Store alpha0 in map
  fParamAlpha0Map[wl]=alpha0;
  
  if(fVerbose)
    {
     std::cout<<"[LidarTools::Analyser] Initialization R0="<<GetParamR0(wl)<<" m"<<std::endl;
     std::cout<<"                       nearest bin is at "<< altitude <<" m"<<std::endl;
     std::cout<<"                       a0_wl1="<< alpha0 <<" m^-1"<<std::endl;
    }
  
  // initialize references
  // Scattering ratio = 1+ beta_p/beta_m
  // 1 means no aerosol at R0, then has to be greater than 1
  // 1.01 from ergastiria/datapros20080421/inputs/klett_inputs.txt
  Float_t sratio=fFernald84_sratio;
  
  // Reference values
  alpha_m[AlphaNBins-1] = alpha0;
  beta_m[AlphaNBins-1]  = beta0;
  beta_p[AlphaNBins-1]  = beta_m[AlphaNBins-1] * (sratio-1);
  alpha_p[AlphaNBins-1] = beta_p[AlphaNBins-1]*Sp;
  alpha[AlphaNBins-1]   = alpha_m[AlphaNBins-1]+alpha_p[AlphaNBins-1];
  beta[AlphaNBins-1]    = beta_m[AlphaNBins-1]+beta_p[AlphaNBins-1];
  // Model
  alpha_model[AlphaNBins-1] = alpha0model;

  // Inversion
  // --------- Q1 integral   
  TArrayF  Q1(AlphaNBins);
  TArrayF  Q1temp(AlphaNBins);
  Q1temp[AlphaNBins-1]=0.;
  for(int i=AlphaNBins-2; i>=0; i--){
    // altitude bin width -- note that delta_Z>0
    Float_t step=(fBinsCenterAltitude[i+1]-fBinsCenterAltitude[i])/GetRangeToAltitude();
    // average altitude > 0
    Float_t altitude=(fBinsCenterAltitude[i+1]+fBinsCenterAltitude[i])/2.;    
     // Get expected model extinction -- not used here but good for plotting
     alpha_model[i]= fAbsorp->Extinction(wl, altitude+fLidarAltitude, 1.);
    // Rayleigh from analytical formula -- actually from Konrad atmosphere table
    alpha_m[i]= fAtmoProfile->Extinction(wl, altitude+fLidarAltitude);
    // Q1
    Q1temp[i]=Q1temp[i+1]+(0.5*step*(alpha_m[i+1]+alpha_m[i]));
    }
  for(int i=0; i<AlphaNBins; i++)
      Q1temp[i]=-Q1temp[i];
  for(int i=0; i<AlphaNBins; i++)
    Q1[i]=exp(-2.0*((Sp/8.37758)-1.0)*Q1temp[i]);
  
  // --------- Q2 integral   
  TArrayF  temp(AlphaNBins);
  TArrayF  Q2temp(AlphaNBins);
  TArrayF  Q2(AlphaNBins);
  for(int i=0; i<AlphaNBins; i++)
    temp[i]=binpw[i]*Q1[i];
  Q2temp[AlphaNBins-1]=0.;
  for(int i=AlphaNBins-2; i>=0; i--){
    // altitude bin width -- note that delta_Z>0
    Float_t step=(fBinsCenterAltitude[i+1]-fBinsCenterAltitude[i])/GetRangeToAltitude();
    // Q2
    Q2temp[i]=Q2temp[i+1]+(0.5*step*(temp[i+1]+temp[i]));    
    }
  for(int i=0; i<AlphaNBins; i++)
      Q2temp[i]=-Q2temp[i];
  for(int i=0; i<AlphaNBins; i++)
    Q2[i]=2.*Sp*Q2temp[i];
   
  // --------- Numerateur et denominateur
  // FIRST, calculate quantities at the aerosol free region (normalization window)
  // Signal at the aerosol free region    
  // here I get a single value instead of the average over a window
  double sref = binpw[AlphaNBins-1];
  // Other parameters amfree and bmfree
  double amfree = alpha_m[AlphaNBins-1];
  //double bmfree = beta_m[AlphaNBins-1]; // not used
  //double bpfree = beta_p[AlphaNBins-1]; // not used
  double apfree = alpha_p[AlphaNBins-1];
  //double bref = beta[AlphaNBins-1];     // not used
  
  TArrayF  arith(AlphaNBins);  
  TArrayF  paron(AlphaNBins);  
  TArrayF  temp2(AlphaNBins);  
  for(int i=0; i<AlphaNBins; i++){
    arith[i]=Sp*binpw[i]*Q1[i];
    paron[i]=((Sp*sref)/(apfree+(Sp/8.37758)*amfree))-Q2[i];
    temp2[i]=arith[i]/paron[i];   
    }
    
  // final formula
  for(int i=0; i<AlphaNBins; i++){
    float test2=Sp/8.37758;
    alpha_p[i]= temp2[i]-test2*alpha_m[i];
    beta_p[i] = alpha_p[i]/Sp;
    }     
  // other pieces
  for(int i=0; i<AlphaNBins; i++){
    alpha[i]  = alpha_p[i]+alpha_m[i];
    beta_m[i] = alpha_m[i]/Sr;
    beta[i]   = beta_p[i]+beta_m[i];
    // Debug
    //std::cout<<"Aeronet "<<i<<" "<<alpha[i]<<" "<<alpha_m[i]<<" "<<alpha_p[i]<<" "
    //         <<beta[i]<< " "<<beta_m[i]<<" "<<beta_p[i]<<std::endl;     
    }    
    
  // Store results in maps  
  fAlphaMap[wl]  = alpha;
  fBetaMap[wl]   = beta;
  fAlphaMap_P[wl]= alpha_p;
  fBetaMap_P[wl] = beta_p;
  fAlphaMap_M[wl]= alpha_m;
  fBetaMap_M[wl] = beta_m;
  
  // Store atmosphere extinction model
  fAlphaModelMap[wl] = alpha_model;
  
}

// Compute integrated atmosphere opacity 
void LidarTools::Analyser::ComputeAtmosphereOpacity(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Compute atmosphere opacity, Tau4 and AOD" << std::endl;
  // Input is extinction profile
  TArrayF alpha_M=fAlphaMap_M[wl];
  TArrayF alpha=fAlphaMap[wl];
  TArrayF alpha_P=fAlphaMap_P[wl];  
  TArrayF alphamodel=fAlphaModelMap[wl];
  // Opacity from AltMin to AltMax
  TArrayF opacity(alpha.GetSize());
  TArrayF opacity_P(alpha.GetSize());
  TArrayF opacitymodel(alphamodel.GetSize());
  
  // Optical Depths from fTauAltMin to fTauAltMax
  Float_t od_m=0., od_t=0., od_p=0., od_model=0., od_model_p=0.;
  // Rayleigh, Total, AOD, Model Total, Model AOD
  
  // Integration
  for(int i=0; i<alpha.GetSize(); i++){
    Float_t area_M     =alpha_M[i]*(fBinsAltitude[i+1]-fBinsAltitude[i]);
    Float_t area       =alpha[i]*(fBinsAltitude[i+1]-fBinsAltitude[i]);
    Float_t area_P     =alpha_P[i]*(fBinsAltitude[i+1]-fBinsAltitude[i]);    
    Float_t areamodel  =alphamodel[i]*(fBinsAltitude[i+1]-fBinsAltitude[i]);
    Float_t areamodel_P=areamodel-area_M;
    if(i==0){
      opacity[i]=area;
      opacity_P[i]=area_P;
      opacitymodel[i]=areamodel;
      }
    else{
      opacity[i]=opacity[i-1]+area;
      opacity_P[i]=opacity_P[i-1]+area_P;
      opacitymodel[i]=opacitymodel[i-1]+areamodel;
      }
    if(fBinsAltitude[i+1]>=fTauAltMin && fBinsAltitude[i]<=fTauAltMax){
        od_m+=area_M;
        od_t+=area;
        od_p+=area_P;
        od_model+=areamodel;
        od_model_p+=areamodel_P;
        }
    }
  // Store Opacity and Tay4
  fOpacityMap[wl]=opacity;
  fOpacityMap_P[wl]=opacity_P;
  fOpacityModelMap[wl]=opacitymodel;
  fODMap_M[wl]=od_m;
  fODMap[wl]=od_t;
  fODMap_P[wl]=od_p;
  fODModelMap[wl]=od_model;
  fODModelMap_P[wl]=od_model_p;
  
  
if(fVerbose) std::cout << "[LidarTools::Analyser] OD("<<wl<<" nm) = "<<fODMap[wl]
                       << "\tAOD = "<<fODMap_P[wl]<<std::endl;
}

// Compute integrated atmosphere opacity 
void LidarTools::Analyser::ComputeAtmosphereTransmission(Int_t wl)
{
if(fVerbose) std::cout << "[LidarTools::Analyser] Compute atmosphere Transmission" << std::endl;
  // Input is opacity profile
  TArrayF opacity=fOpacityMap[wl];
  TArrayF opacitymodel=fOpacityModelMap[wl];
  // Transmission
  TArrayF trans(opacity.GetSize());
  TArrayF transmodel(opacitymodel.GetSize());

  for(int i=0; i<opacity.GetSize(); i++){
    trans[i]=exp(-1.*opacity[i]);
    transmodel[i]=exp(-1.*opacitymodel[i]);
    }

  //Store results
  fTransmissionMap[wl]=trans;
  fTransmissionModelMap[wl]=transmodel;
}

// Simple getter for the extinction profile
TArrayF LidarTools::Analyser::GetAlphaProfile(Int_t wl, std::string scattering)
{
	if (scattering=="T") // Total
	    return fAlphaMap[wl];
	else if (scattering=="P"){ // Particles
	    if(fAlphaMap_P.count(wl))
	        return fAlphaMap_P[wl];
	    else
	        return 0;
	    }
	else if (scattering=="M"){ // Molecules
  	    if(fAlphaMap_M.count(wl))
	        return fAlphaMap_M[wl];
	    else
	        return 0;
	    }
	else
	    return 0;
}

// Simple getter for the backscattering profile
TArrayF LidarTools::Analyser::GetBetaProfile(Int_t wl, std::string scattering)
{
	if (scattering=="T") // Total
	    return fBetaMap[wl];
	else if (scattering=="P"){ // Particles
	    if(fBetaMap_P.count(wl))
	        return fBetaMap_P[wl];
	    else
	        return 0;
	    }
	else if (scattering=="M"){ // Molecules
  	    if(fBetaMap_M.count(wl))
	        return fBetaMap_M[wl];
	    else
	        return 0;
	    }
	else
	    return fBetaMap[wl];
}

// Simple getter for the backscattering profile
TArrayF LidarTools::Analyser::GetOpacityProfile(Int_t wl, std::string scattering)
{
	if (scattering=="T") // Total
	    return fOpacityMap[wl];
	else if (scattering=="P"){ // Particles
	    if(fOpacityMap_P.count(wl))
	        return fOpacityMap_P[wl];
	    else
	        return 0;
	    }
	else if (scattering=="M"){ // Molecules
  	    if(fOpacityMap_M.count(wl))
	        return fOpacityMap_M[wl];
	    else
	        return 0;
	    }
	else
	    return fOpacityMap[wl];
}

// Simple getter for the extinction profile
Float_t LidarTools::Analyser::GetOD(Int_t wl, std::string scattering)
{
	if (scattering=="T") // Total
	    return fODMap[wl];
	else if (scattering=="P"){ // Particles
	    if(fODMap_P.count(wl))
	        return fODMap_P[wl];
	    else
	        return 0;
	    }
	else if (scattering=="M"){ // Molecules
  	    if(fODMap_M.count(wl))
	        return fODMap_M[wl];
	    else
	        return 0;
	    }
	else
	    return 0;
}

ClassImp(LidarTools::Analyser)
