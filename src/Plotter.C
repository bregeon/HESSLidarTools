/** @file Plotter.C
 *
 * @brief Plotter class implementation
 *
 * @author Johan Bregeon
*/

#include "Plotter.hh"

// Constructor
LidarTools::Plotter::Plotter(LidarTools::Analyser *analyser,
                             Bool_t applyOffset,
                             Bool_t verbose)
: fVerbose(verbose),
  fApplyOffset(applyOffset),
  fAnalyser(analyser),
  fRawCanvas(0),
  fFilterCanvas(0),
  fMainCanvas(0),
  fExtraCanvas(0),
  fSummaryCanvas(0),
  fAngstExpCanvas(0)
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Constructor" << std::endl;
  
  if(fApplyOffset)
    fAltitudeOffset=fAnalyser->GetLidarAltitude();
  else
    fAltitudeOffset=0.;
  
  fRunNumber=fAnalyser->GetRunNumber();    
  fSeqNumber=fAnalyser->GetSeqNumber();
}


// Destructor
LidarTools::Plotter::~Plotter()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Destructor" << std::endl; 

//  delete fAnalyser; // not sure, given the initialization in the constructor
                      //if we have a local copy or the original pointer
// clear maps  
  fRawProfileMap.clear();
  fBkgProfMap.clear();
  fBkgHistMap.clear();
  fLnRawPowerMap.clear();
  fLnFilteredPowerMap.clear();
  fLnBinnedPowerMap.clear();
  fBinnedPowerDevMap.clear();
  fExtinctionMap.clear();
  fExtinctionMap_P.clear();
  fExtinctionMap_M.clear();
  fBackscatterMap.clear();
  fBackscatterMap_P.clear();
  fBackscatterMap_M.clear();

  delete fAngstExp;
  
  delete fRawCanvas;
  delete fFilterCanvas;
  delete fMainCanvas;
  delete fExtraCanvas;
  delete fSummaryCanvas;

}

// To make blue and green plots
Int_t LidarTools::Plotter::GetColor(Int_t wl)
{
  if(wl==355)
    return kBlue;
  else if (wl==532)
    return kGreen;
  else
    return 1;
}

TGraph* LidarTools::Plotter::AtmoGraph(TString name, TString title,
                                       Int_t wl)
{
  TGraph *graph = new TGraph();
  name+="_";
  name+=fRunNumber;
  name+="_";
  name+=fSeqNumber;
  name+="_";
  name+=wl;
  title+=" ";
  title+=wl;
  title+=" nm";
  graph->SetNameTitle(name, title);
  graph->SetMarkerColor(GetColor(wl));
  return graph;
}

// Initialize Raw profile graphs
void LidarTools::Plotter::InitRawProfile(Int_t wl)
{
  TGraph* raw=AtmoGraph("RawProfile", "Raw Lidar Profile", wl);
  fRawProfileMap[wl]=raw;
  gDirectory->Add(raw);
}

// Initialize background profile graphs
void LidarTools::Plotter::InitBkgProf(Int_t wl)
{
  TGraph* raw=AtmoGraph("LidarBkgProfile", "Lidar Background Profile", wl);
  fBkgProfMap[wl]=raw;
  gDirectory->Add(raw);
}

// Initialize background histograms
void LidarTools::Plotter::InitBkgHist(Int_t wl)
{
  
  TString name="LidarBkgHist_";
  name+=fRunNumber;
  name+="_"; 
  name+=fSeqNumber;
  name+="_"; 
  name+=wl;
  TString title="Lidar Background Histogram ";
  title+=wl;
  title+=" nm";
  TH1F *bkg = new TH1F(name, title,1000, -0.01, 0.01);
  bkg->SetLineColor(GetColor(wl));
  fBkgHistMap[wl]=bkg;
  // TH1F is added by ROOT already
  //gDirectory->Add(bkg);
}

// Initialize Ln Raw Power profile
void LidarTools::Plotter::InitLnRawPowerProfile(Int_t wl)
{
  TGraph* lnpow=AtmoGraph("LidarLnRawPWProfile", "Lidar Ln(RawPower) profile", wl);
  fLnRawPowerMap[wl]=lnpow;
  gDirectory->Add(lnpow);
}

// Initialize Ln Filtered Power profile
void LidarTools::Plotter::InitLnFilteredPowerProfile(Int_t wl)
{
  TGraph* lnpow=AtmoGraph("LidarLnFilteredPWProfile", "Lidar Ln(FilteredPower) profile", wl);
  fLnFilteredPowerMap[wl]=lnpow;
  gDirectory->Add(lnpow);
}

// Initialize Ln BinnedPower profile
void LidarTools::Plotter::InitLnBinnedPowerProfile(Int_t wl)
{
  TGraph* lnpow=AtmoGraph("LidarLnBinnedPWProfile", "Lidar Ln(BinnedPower) profile", wl);
  fLnBinnedPowerMap[wl]=lnpow;
  gDirectory->Add(lnpow);
}

// Initialize Binned Power Deviation profiles (Power, PowerStdDev, Power/StdDev)
void LidarTools::Plotter::InitBinnedPowerDevProfiles(Int_t wl)
{
  TGraph* pow=AtmoGraph("LidarBinnedPWProfile", "Lidar BinnedPower profile", wl);
  fBinnedPowerMap[wl]=pow;
  gDirectory->Add(pow);

  TGraph* powdev=AtmoGraph("LidarBinnedPWDevProfile", "Lidar BinnedPowerDev profile", wl);
  fBinnedPowerDevMap[wl]=powdev;
  gDirectory->Add(powdev);

  TGraph* powdevratio=AtmoGraph("LidarBinnedPWDevRatioProfile", "Lidar BinnedPower Deviation Ratio profile", wl);
  fBinnedPowerDevRatioMap[wl]=powdevratio;
  gDirectory->Add(powdevratio);
}

// Initialize Extinction profiles
void LidarTools::Plotter::InitExtinctionProfiles(Int_t wl)
{
  TGraph* alpha=AtmoGraph("Extinction_T", "Lidar Atmosphere Extinction", wl);
  alpha->SetMarkerStyle(kFullCircle);
  alpha->SetMarkerSize(0.8);
  fExtinctionMap[wl]=alpha;
  gDirectory->Add(alpha);
  
  TGraph* alpha_p=AtmoGraph("Extinction_P", "Mie scattering extinction", wl);
  alpha_p->SetMarkerStyle(kOpenCircle);  
  alpha_p->SetMarkerSize(0.8);
  fExtinctionMap_P[wl]=alpha_p;
  gDirectory->Add(alpha_p);

  TGraph* alpha_m=AtmoGraph("Extinction_M", "Rayleigh scattering extinction", wl);
  alpha_m->SetLineWidth(2);
  alpha_m->SetMarkerStyle(kDot);  
  fExtinctionMap_M[wl]=alpha_m;
  gDirectory->Add(alpha_m);
  
  TGraph* alphamodel=AtmoGraph("ExtinctionModel", "Lidar Atmosphere Extinction Model", wl);
  alphamodel->SetLineColor(kRed);
  alphamodel->SetLineWidth(2);
  alphamodel->SetMarkerStyle(kDot);
  fExtinctionModelMap[wl]=alphamodel;
  gDirectory->Add(alphamodel);
  
}


// Initialize Backscatter profile
void LidarTools::Plotter::InitBackscatterProfiles(Int_t wl)
{
  
  TGraph* beta=AtmoGraph("Backscatter", "Lidar Atmosphere Backscatter", wl);
  beta->SetMarkerStyle(kFullCircle);
  beta->SetMarkerSize(0.8);
  fBackscatterMap[wl]=beta;
  gDirectory->Add(beta);
  
  TGraph* beta_p=AtmoGraph("Backscatter_P", "Mie scattering backscatter", wl);  
  beta_p->SetMarkerStyle(kOpenCircle);  
  beta_p->SetMarkerSize(0.8);
  fBackscatterMap_P[wl]=beta_p;
  gDirectory->Add(beta_p);

  TGraph* beta_m=AtmoGraph("Backscatter_M", "Rayleigh scattering backscatter", wl);  
  beta_p->SetMarkerStyle(kDot);
  fBackscatterMap_M[wl]=beta_m;
  gDirectory->Add(beta_m);
  
}

// Initialize Lidar ratio profile
void LidarTools::Plotter::InitLidarRatio(Int_t wl)
{
  TGraph* ratio=AtmoGraph("LidarRatio", "Lidar Ratio", wl);  
  fLidarRatioMap[wl]=ratio;
  gDirectory->Add(ratio);
}

// Initialize Lidar opacity profile
void LidarTools::Plotter::InitOpacityProfile(Int_t wl)
{
  TGraph* opacity=AtmoGraph("Opacity", "Atmosphere opacity", wl);  
  fOpacityMap[wl]=opacity;
  gDirectory->Add(opacity);

  TGraph* opacitymodel=AtmoGraph("OpacityModel", "Atmosphere opacity model", wl); 
  opacitymodel->SetLineColor(kRed);
  opacitymodel->SetLineWidth(2);
  opacitymodel->SetMarkerStyle(kDot);   
  fOpacityModelMap[wl]=opacitymodel;
  gDirectory->Add(opacitymodel);
}

// Initialize Lidar transmission profile
void LidarTools::Plotter::InitTransmissionProfile(Int_t wl)
{
  TGraph* trans=AtmoGraph("Transmission", "Atmosphere transmission", wl);  
  fTransmissionMap[wl]=trans;
  gDirectory->Add(trans);

  TGraph* transmodel=AtmoGraph("TransmissionModel", "Atmosphere transmission model", wl); 
  transmodel->SetLineColor(kRed);
  transmodel->SetLineWidth(2);
  transmodel->SetMarkerStyle(kDot);   
  fTransmissionModelMap[wl]=transmodel;
  gDirectory->Add(transmodel);
}

// Initialize Lidar OD results graph
void LidarTools::Plotter::InitODResults(Int_t wl)
{
  TGraph* od=AtmoGraph("OD", "Optical Depth", wl);  
  fODResultsMap[wl]=od;
  gDirectory->Add(od);
}

// Initialize Lidar transmission profile
void LidarTools::Plotter::InitAngstExpProfile()
{
  fAngstExp=AtmoGraph("AngstExp", "Angstroem Exponent", 355);  
  gDirectory->Add(fAngstExp);
}
    
// Fill raw signal graph
void LidarTools::Plotter::FillRawProfile(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillRawProfile] Fill raw graphs wl "<<wl<< std::endl;
   TArrayF range  = fAnalyser->GetAltitudes();
   TArrayF signal = fAnalyser->GetRawSignal(wl);   
   for(Int_t i=0; i<range.GetSize(); i++)    
        fRawProfileMap[wl]->SetPoint(i, signal[i], range[i]+fAltitudeOffset);
}

// Fill background profile
void LidarTools::Plotter::FillBkgProf(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillBkgProf] Fill backgroud prof wl "<<wl<< std::endl;
   
   TArrayF range  = fAnalyser->GetRawRange();
   Float_t step=range[1]-range[0];   
   if(fVerbose) std::cout << "[LidarTools::FillBkgProf] step size "<<step<< std::endl;
   // Get Data
   TArrayF bkg    = fAnalyser->GetFullBkg(wl);
   Float_t bgmin  = fAnalyser->GetParamBkgMin();
   // Initialize local arrays   
   TArrayF val(5), ind(5);
   for(Int_t j=0; j<5; j++){
      val[j]=0;
      ind[j]=0;
      }
   // Fill in local arrays
   for(Int_t i=0; i<bkg.GetSize(); i++) {
     Float_t alt=bgmin+i*step*1000;
     for(Int_t j=0; j<5; j++){
        // assume bkg is always taken between 20 k and 25 km
        if(alt>20000+j*1000 && alt<21000+j*1000){
           val[j]+=bkg[i]; 
           ind[j]+=1;
           }
        }
     }
  // Fill TGraph   
  for(Int_t j=0; j<5; j++){
    fBkgProfMap[wl]->SetPoint(j, 20500+j*1000, val[j]/ind[j]);
    }
  
  // Debug
//  std::cout << "[LidarTools::FillBkgProf]  Dump background prof wl "<<wl<< std::endl;  
//  for(Int_t j=0; j<5; j++){
//    std::cout << GetBkgProf(wl)->GetX()[j]<<"\t"<<GetBkgProf(wl)->GetY()[j] << std::endl;  
//    }

}

// Fill background histogram
void LidarTools::Plotter::FillBkgHist(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillBkgHist] Fill backgroud hist wl "<<wl<< std::endl;
   TArrayF bkg = fAnalyser->GetFullBkg(wl);
   for(Int_t i=0; i<bkg.GetSize(); i++)     
       fBkgHistMap[wl]->Fill(bkg[i]);
}

// Fill Ln(Raw Power) profile
void LidarTools::Plotter::FillLnRawPower(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillLnRawPower] Fill Ln(raw pow) wl "<<wl<< std::endl;
   TArrayF altitudes = fAnalyser->GetAltitudes();
   TArrayF pow = fAnalyser->GetPower(wl);
   float RangeToAltitude=fAnalyser->GetRangeToAltitude(); // correction = 0.9659;
   
   for(Int_t i=0; i<altitudes.GetSize(); i++)    
        fLnRawPowerMap[wl]->SetPoint(i, log(pow[i]), altitudes[i]/RangeToAltitude+fAltitudeOffset);
}

// Fill Ln(Power) profile
void LidarTools::Plotter::FillLnFilteredPower(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillLnFilteredPower] Fill Ln(filt pow) wl "<<wl<< std::endl;
   TArrayF altitudes = fAnalyser->GetAltitudes();
   TArrayF pow = fAnalyser->GetFilteredPower(wl);   
   float RangeToAltitude=fAnalyser->GetRangeToAltitude(); // correction = 0.9659;

   for(Int_t i=0; i<altitudes.GetSize(); i++)    
        fLnFilteredPowerMap[wl]->SetPoint(i, log(pow[i]), altitudes[i]/RangeToAltitude+fAltitudeOffset);
}

// Fill Ln(Power) profile
void LidarTools::Plotter::FillLnBinnedPower(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillLnBinnedPower] Fill Ln(bin pow) wl "<<wl<< std::endl;
   TArrayF altitudeBins = fAnalyser->GetBinsCenterAltitude();
   TArrayF pow = fAnalyser->GetBinnedPower(wl);   
   for(Int_t i=0; i<altitudeBins.GetSize(); i++)    
        fLnBinnedPowerMap[wl]->SetPoint(i, log(pow[i]), altitudeBins[i]+fAltitudeOffset);
}

// Fill Power Deviation profile
void LidarTools::Plotter::FillBinnedPowerDev(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillBinnedPowerDev] Fill bin pow deviation wl "<<wl<< std::endl;
   TArrayF altitudeBins = fAnalyser->GetBinsCenterAltitude();
   TArrayF pow = fAnalyser->GetBinnedPower(wl);   
   TArrayF powdev = fAnalyser->GetBinnedPowerDev(wl);   
   for(Int_t i=0; i<altitudeBins.GetSize(); i++) {
        fBinnedPowerMap[wl]->SetPoint(i, pow[i], altitudeBins[i]+fAltitudeOffset);
        fBinnedPowerDevMap[wl]->SetPoint(i, powdev[i], altitudeBins[i]+fAltitudeOffset);
        fBinnedPowerDevRatioMap[wl]->SetPoint(i, pow[i]/powdev[i], altitudeBins[i]+fAltitudeOffset);
	    }
}

// Fill Extinction and Backscatter profiles
void LidarTools::Plotter::FillExtinctionBackscatter(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillExtinctionBackscatter] Fill Extinction wl "<<wl<< std::endl;
   TArrayF altitudeBins = fAnalyser->GetBinsCenterAltitude();
   TArrayF alpha = fAnalyser->GetAlphaProfile(wl);   
   TArrayF beta = fAnalyser->GetBetaProfile(wl);   
   TArrayF alpha_model = fAnalyser->GetAlphaModelProfile(wl);
   for(Int_t i=0; i<alpha.GetSize(); i++){
        fExtinctionMap[wl]->SetPoint(i, alpha[i], altitudeBins[i]+fAltitudeOffset);
        fBackscatterMap[wl]->SetPoint(i, beta[i], altitudeBins[i]+fAltitudeOffset);
        fLidarRatioMap[wl]->SetPoint(i, alpha[i]/beta[i], altitudeBins[i]+fAltitudeOffset);
        fExtinctionModelMap[wl]->SetPoint(i, alpha_model[i], altitudeBins[i]+fAltitudeOffset);
	}
	
	// if details are available
    TArrayF alpha_p = fAnalyser->GetAlphaProfile(wl,"P");
    TArrayF alpha_m = fAnalyser->GetAlphaProfile(wl,"M");
    TArrayF beta_p = fAnalyser->GetBetaProfile(wl,"P");
    TArrayF beta_m = fAnalyser->GetBetaProfile(wl,"M");

    if(1)//fAnalyser->hasDetails(wl)){
      { 
       for(Int_t i=0; i<alpha_p.GetSize(); i++){
           fExtinctionMap_P[wl]->SetPoint(i, alpha_p[i], altitudeBins[i]+fAltitudeOffset);
           fBackscatterMap_P[wl]->SetPoint(i, beta_p[i], altitudeBins[i]+fAltitudeOffset);

//           fLidarRatioMap[wl]->SetPoint(i, alpha_p[i]/beta_p[i], altitudeBins[i]+fAltitudeOffset);           

           fExtinctionMap_M[wl]->SetPoint(i, alpha_m[i], altitudeBins[i]+fAltitudeOffset);
           fBackscatterMap_M[wl]->SetPoint(i, beta_m[i], altitudeBins[i]+fAltitudeOffset);
      	   }	  
      }
}

// Fill opacity profiles
void LidarTools::Plotter::FillOpacity(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillOpacity] Fill Opacity wl "<<wl<< std::endl;
   TArrayF altitudeBins = fAnalyser->GetBinsCenterAltitude();
   TArrayF opacity = fAnalyser->GetOpacityProfile(wl);   
   TArrayF opacitymodel = fAnalyser->GetOpacityModelProfile(wl);   
   for(Int_t i=0; i<opacity.GetSize(); i++){
        fOpacityMap[wl]->SetPoint(i, altitudeBins[i]+fAltitudeOffset, opacity[i]);
        fOpacityModelMap[wl]->SetPoint(i, altitudeBins[i]+fAltitudeOffset, opacitymodel[i]);
        }
}

// Fill opacity profiles
void LidarTools::Plotter::FillTransmission(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillTransmission] Fill Transmission wl "<<wl<< std::endl;
   TArrayF altitudeBins = fAnalyser->GetBinsCenterAltitude();
   TArrayF trans = fAnalyser->GetTransmissionProfile(wl);   
   TArrayF transmodel = fAnalyser->GetTransmissionModelProfile(wl);   
   for(Int_t i=0; i<trans.GetSize(); i++){
        fTransmissionMap[wl]->SetPoint(i, altitudeBins[i]+fAltitudeOffset, trans[i]);
        fTransmissionModelMap[wl]->SetPoint(i, altitudeBins[i]+fAltitudeOffset, transmodel[i]);
        }
}

// Fill opacity profiles
void LidarTools::Plotter::FillODResults(Int_t wl)
{
   if(fVerbose) std::cout << "[LidarTools::FillODResults] Fill OD results wl "<<wl<< std::endl;

  Float_t od_M=fAnalyser->GetRayleighOD(wl);
  Float_t od=fAnalyser->GetOD(wl);
  Float_t od_P=fAnalyser->GetAOD(wl);
  Float_t od_model=fAnalyser->GetModelOD(wl);
  Float_t od_model_p=fAnalyser->GetModelAOD(wl);

  fODResultsMap[wl]->SetPoint(0, 1, od_M);
  fODResultsMap[wl]->SetPoint(1, 2, od);
  fODResultsMap[wl]->SetPoint(2, 3, od_P);
  fODResultsMap[wl]->SetPoint(3, 4, od_model);
  fODResultsMap[wl]->SetPoint(4, 5, od_model_p);
}

// Fill Angstroem Exponent for wl1/wl2
void LidarTools::Plotter::FillAngstroemExp(Int_t wl1, Int_t wl2)
{
   if(fVerbose) std::cout << "[LidarTools::FillAngstroemExp] Fill Angstroem Exponent for wl1/wl2="<<wl1<<"/"<<wl2<< std::endl;

   TArrayF altitudeBins = fAnalyser->GetBinsCenterAltitude();
//   TArrayF alpha_p_wl1 = fAnalyser->GetAlphaProfile(wl1,"P");
//   TArrayF alpha_p_wl2 = fAnalyser->GetAlphaProfile(wl2,"P");
   TArrayF od_p_wl1 = fAnalyser->GetOpacityProfile(wl1, "P");
   TArrayF od_p_wl2 = fAnalyser->GetOpacityProfile(wl2, "P");
   
    if(1)//fAnalyser->hasDetails(wl)){
      { 
       for(Int_t i=0; i<od_p_wl1.GetSize(); i++){
//            std::cout<<i<<" "<<alpha_p_wl1[i]<<" "<<alpha_p_wl2[i]<<" "
//                     <<log(abs(alpha_p_wl1[i]/alpha_p_wl2[i])) / log((Float_t)wl2/wl1)
//                     <<" "<<altitudeBins[i]+fAltitudeOffset<<std::endl;
//           fAngstExp->SetPoint(i, log(abs(alpha_p_wl1[i]/alpha_p_wl2[i])) / log((Float_t)wl2/wl1), altitudeBins[i]+fAltitudeOffset);
           fAngstExp->SetPoint(i, - ( log(od_p_wl1[i]) - log(od_p_wl2[i]) ) /
                                    ( log((Float_t)wl1) - log((Float_t) wl2) ), altitudeBins[i]+fAltitudeOffset);
      	   }	  
      }
}

// Init all graphs for one wave length
void LidarTools::Plotter::InitAll(Int_t wl)
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Initialize graphs wl "<<wl<< std::endl;
   InitRawProfile(wl);
   InitBkgProf(wl);
   InitBkgHist(wl);
   InitLnRawPowerProfile(wl);
   InitLnFilteredPowerProfile(wl);
   InitLnBinnedPowerProfile(wl);
   InitBinnedPowerDevProfiles(wl);
   InitExtinctionProfiles(wl);
   InitBackscatterProfiles(wl);
   InitLidarRatio(wl);
   InitOpacityProfile(wl);
   InitTransmissionProfile(wl);
   InitODResults(wl);
}

// Init all graphs
void LidarTools::Plotter::InitAll()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Initialize all graphs"<< std::endl;
  std::vector<Int_t> wlvec=fAnalyser->GetWavelengths();
  std::vector<Int_t>::iterator wl;
  for (wl=wlvec.begin(); wl!=wlvec.end(); ++wl){
    InitAll(*wl);
    fWaveLengthVec.push_back(*wl);
    }
  // Combine results from different wavelengths
  InitAngstExpProfile();
}

// Fill all graphs for one wave length
void LidarTools::Plotter::FillAll(Int_t wl)
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Fill all graphs wl "<<wl<< std::endl;
  FillRawProfile(wl);
  FillBkgProf(wl);
  FillBkgHist(wl);
  FillLnRawPower(wl);

  if(fAnalyser->hasFiltered())
    FillLnFilteredPower(wl);
    
  FillLnBinnedPower(wl);
  FillBinnedPowerDev(wl);
  FillExtinctionBackscatter(wl);
  FillOpacity(wl);
  FillTransmission(wl);
  FillODResults(wl);
}

// Fill all graphs
void LidarTools::Plotter::FillAll()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Fill all graphs"<< std::endl;
  std::vector<Int_t>::iterator wl;
  for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl)
    FillAll(*wl);
  // Combine results from different wavelengths
  FillAngstroemExp(355, 532);      // @todo hardcoded !
}

// Display all graphs
void LidarTools::Plotter::DisplayAll()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display plots"<< std::endl;
  // always display the raw data and main canvas
  DisplayRaw();
  DisplayBkg();
  DisplayStdDev();
  DisplayFilter();
  DisplayMain();
  
   // If extra info available plot it and full summary
   if(1){//fAnalyser->hasDetails(*wl)){
       DisplayExtra();
       DisplaySummary();
       DisplayAngstExpAOD();
   }   
}

// Display Raw graphs
void LidarTools::Plotter::DisplayRaw()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display low level data canvas"<< std::endl;
  
   TString canvasname="LidarRawCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fRawCanvas = new TCanvas(canvasname, "Lidar raw data", 30,50,650,850);

   Int_t nwl=fWaveLengthVec.size();      
   fRawCanvas->Divide(nwl,3);

   Int_t i=1;
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
       fRawCanvas->cd(i);
       GetRawProfile(*wl)->Draw("AP*");
       GetRawProfile(*wl)->GetXaxis()->SetTitle("Raw Signal (mV)");
       GetRawProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");
      
       fRawCanvas->cd(i+nwl);
       GetLnBinnedPowerProfile(*wl)->Draw("AP*");
       GetLnBinnedPowerProfile(*wl)->GetXaxis()->SetTitle("Ln(V*R2)");
       GetLnBinnedPowerProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");
       
       fRawCanvas->cd(i+2*nwl);
       GetExtinctionProfile(*wl)->Draw("AP");
       GetExtinctionProfile(*wl)->GetXaxis()->SetTitle("Extinction (m^{-1})");
       GetExtinctionProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");

       i++;
   }
  gDirectory->Add(fRawCanvas);
}

// Display bkg graphs
void LidarTools::Plotter::DisplayBkg()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display background information canvas"<< std::endl;
  
   TString canvasname="LidarBkgCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fBkgCanvas = new TCanvas(canvasname, "Lidar background  data", 50,50,950,850);

   Int_t nwl=fWaveLengthVec.size();      
   fBkgCanvas->Divide(nwl,2);

   Int_t i=1;
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
       fBkgCanvas->cd(i);
       GetBkgHist(*wl)->Draw();
       GetBkgHist(*wl)->GetXaxis()->SetTitle("Raw Signal (mV)");

       
       fBkgCanvas->cd(i+nwl);
       // To set axis
       TGraph *fake = new TGraph(2);
       fake->SetPoint(0, 20000, -0.01);
       fake->SetPoint(1,25000,0.01);
       fake->SetTitle("Lidar background profile");
       fake->Draw("AP");
       fake->GetXaxis()->SetTitle("Altitude (m)");
       fake->GetYaxis()->SetTitle("Raw Signal Mean (mV)");
       GetBkgProf(*wl)->Draw("PS*");

       // Fit and display bkg drift
       TF1 *pol1 = new TF1("pol1_"+*wl, "pol1", 20000,25000);
       GetBkgProf(*wl)->Fit(pol1,"Q");
       Float_t drift=abs(pol1->GetParameter(1)*5000/GetBkgHist(*wl)->GetMean()*100);       
       std::cout << "[LidarTools::Plotter] Bkg drift for wl "<< *wl<< " = "<<drift<<"%"<<std::endl;
       
       // print drift value on graph
       TText *text=new TText();
       TString dts = "drift = ";
       dts += drift;
       dts += "%";
       text->DrawTextNDC(0.6, 0.4, dts);
      
       i++;
   }
  gDirectory->Add(fBkgCanvas);
}

// Display StdDev graphs
void LidarTools::Plotter::DisplayStdDev()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display StdDev information canvas"<< std::endl;
  
   TString canvasname="LidarStdDevCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fStdDevCanvas = new TCanvas(canvasname, "Lidar StdDev data", 50,50,650,850);

   Int_t nwl=fWaveLengthVec.size();      
   fStdDevCanvas->Divide(nwl,3);

   Int_t i=1;
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
       fStdDevCanvas->cd(i);       
       GetBinnedPowerProfile(*wl)->Draw("AP*");
       GetBinnedPowerProfile(*wl)->GetXaxis()->SetTitle("Power (V*R2)");
       GetBinnedPowerProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");
       
       fStdDevCanvas->cd(i+1*nwl);
       GetBinnedPowerDevProfile(*wl)->Draw("AP*");
       GetBinnedPowerDevProfile(*wl)->GetXaxis()->SetTitle("StdDev(V*R2)");
       GetBinnedPowerDevProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");
              
       fStdDevCanvas->cd(i+2*nwl);
       // To set axis
       TGraph *fake = new TGraph(2);
       fake->SetPoint(1, 0, 0);
       fake->SetPoint(2,20,20000);
       fake->SetTitle("Lidar BinnedPower StdDev Ratio profile");
       fake->Draw("AP");
       fake->GetXaxis()->SetTitle("(V*R2)/StdDev(V*R2)");
       fake->GetYaxis()->SetTitle("Altitude (m)");
       GetBinnedPowerDevRatioProfile(*wl)->Draw("PS*");
       
       i++;
   }
  gDirectory->Add(fStdDevCanvas);
}
// Display all graphs
void LidarTools::Plotter::DisplayFilter()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display filter canvas"<< std::endl;
  
   TString canvasname="LidarFilterCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fFilterCanvas = new TCanvas(canvasname, "Lidar Filter results", 50,50,650,850);

   Int_t nwl=fWaveLengthVec.size();      
   fFilterCanvas->Divide(nwl,3);

   Int_t i=1;
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
       fFilterCanvas->cd(i);
       GetLnRawPowerProfile(*wl)->Draw("APL");
       GetLnRawPowerProfile(*wl)->GetXaxis()->SetTitle("Ln(V*R2)");
       GetLnRawPowerProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");
       
       fFilterCanvas->cd(i+nwl);              
       GetLnFilteredPowerProfile(*wl)->Draw("APL");
       GetLnFilteredPowerProfile(*wl)->GetXaxis()->SetTitle("Ln(V*R2)");
       GetLnFilteredPowerProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");
       
       fFilterCanvas->cd(i+2*nwl);              
       GetLnBinnedPowerProfile(*wl)->Draw("AP*");
       GetLnBinnedPowerProfile(*wl)->GetXaxis()->SetTitle("Ln(V*R2)");
       GetLnBinnedPowerProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");
       i++;
   }
  gDirectory->Add(fFilterCanvas);
}

// Display all graphs
void LidarTools::Plotter::DisplayMain()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display main canvas"<< std::endl;
  
   TString canvasname="LidarDataCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fMainCanvas = new TCanvas(canvasname, "Lidar data", 30,50,650,850);

   Int_t nwl=fWaveLengthVec.size();      
   fMainCanvas->Divide(nwl,2);

   Int_t i=1;
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
      
       // Optical depth
       fMainCanvas->cd(i);
       GetOpacityModelProfile(*wl)->Draw("AL");
       GetOpacityProfile(*wl)->Draw("PS");
       GetOpacityProfile(*wl)->GetXaxis()->SetTitle("Altitude (m)");
       GetOpacityProfile(*wl)->GetYaxis()->SetTitle("Optical Depth");

       TLegend *leg2 = new TLegend(0.1,0.75,0.6,0.9);
       TString header="Run ";
       header+=fRunNumber;
       leg2->SetHeader(header);
       TString date(fAnalyser->GetTimeString());
       leg2->AddEntry("",date,"");     
       leg2->AddEntry(GetOpacityProfile(*wl),"Lidar Optical depth","P");
       leg2->AddEntry(GetOpacityModelProfile(*wl),"Optical depth model","L");       
       leg2->SetTextSize(0.03);
       leg2->Draw("same");             

       // Transmission
       fMainCanvas->cd(i+nwl);
       GetTransmissionModelProfile(*wl)->Draw("AL");
       GetTransmissionProfile(*wl)->Draw("PS");
       GetTransmissionProfile(*wl)->GetXaxis()->SetTitle("Transmission (prob)");
       GetTransmissionProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");

       TLegend *leg1 = new TLegend(0.4,0.75,0.9,0.9);
       leg1->SetHeader(header);
       leg1->AddEntry("",date,"");     
       leg1->AddEntry(GetTransmissionProfile(*wl),"Transmission","P");
       leg1->AddEntry(GetTransmissionModelProfile(*wl),"Transmission model","L");       
       leg1->SetTextSize(0.03);
       leg1->Draw("same");             

       i++;
   }
  gDirectory->Add(fMainCanvas);
}

// Display Extra Detailed graphs
void LidarTools::Plotter::DisplayExtra()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display extra plots"<< std::endl;
  
   TString canvasname="ExtraCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fExtraCanvas = new TCanvas(canvasname, "Extra Lidar info", 30,50,650,850);

   Int_t nwl=fWaveLengthVec.size();      
   fExtraCanvas->Divide(nwl,3);

   Int_t i=1;
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
       fExtraCanvas->cd(i);
       GetExtinctionProfile(*wl)->Draw("AP");       
       GetExtinctionProfile(*wl)->GetXaxis()->SetTitle("Extinction (m^{-1})");
       GetExtinctionProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");

       GetExtinctionProfile(*wl,"P")->Draw("PS");       
       GetExtinctionProfile(*wl,"M")->Draw("SL");       

       fExtraCanvas->cd(i+nwl);
       GetBackscatterProfile(*wl)->Draw("AP");       
       GetBackscatterProfile(*wl)->GetXaxis()->SetTitle("Backscatter (m^{-1})");
       GetBackscatterProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");

       GetBackscatterProfile(*wl,"P")->Draw("PS");       
       GetBackscatterProfile(*wl,"M")->Draw("SL");       
       
       fExtraCanvas->cd(i+2*nwl);      
       GetLidarRatioProfile(*wl)->Draw("AP*");
       GetLidarRatioProfile(*wl)->GetXaxis()->SetTitle("Ratio");
       GetLidarRatioProfile(*wl)->GetYaxis()->SetTitle("Altitude (m)");

       i++;
   }
   
   gDirectory->Add(fExtraCanvas);

}

// Display summary graphs
void LidarTools::Plotter::DisplaySummary()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display summary plots"<< std::endl;
  
   TString canvasname="SummaryCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fSummaryCanvas = new TCanvas(canvasname , "Summary Lidar information", 30,50,950,850);

   Int_t nwl=fWaveLengthVec.size();      
   fSummaryCanvas->Divide(nwl);

   // To set axis
   TGraph *fake[2];
   fake[0] = new TGraph(2);
   fake[0]->SetPoint(1,1e-6,1);
   fake[0]->SetPoint(2,0.16e-3,13000);
   fake[1] = new TGraph(2);
   fake[1]->SetPoint(1,1e-6,1);
   fake[1]->SetPoint(2,0.08e-3,13000);

   Int_t i=1;
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
       fSummaryCanvas->cd(i);
       
       TString name="Extinction_";
       name+=fRunNumber;       
       name+="_";
       name+=fSeqNumber;
       name+="_";
       name+=(*wl);
       TString title="Lidar Atmosphere Extinction ";
       title+=(*wl);
       title+=" nm";
       fake[i-1]->SetNameTitle(name, title);
              
       fake[i-1]->Draw("AP");
       fake[i-1]->GetXaxis()->SetTitle("Extinction (m^{-1})");
       fake[i-1]->GetXaxis()->SetTitleSize(0.03);
       fake[i-1]->GetXaxis()->SetLabelSize(0.025);
       fake[i-1]->GetYaxis()->SetTitle("Altitude (m)");
       fake[i-1]->GetYaxis()->SetTitleOffset(1.7);
       fake[i-1]->GetYaxis()->SetTitleSize(0.03);
       fake[i-1]->GetYaxis()->SetLabelSize(0.025);
              
       GetExtinctionProfile(*wl)->Draw("PS");       
       GetExtinctionProfile(*wl,"P")->Draw("PS");       
       GetExtinctionProfile(*wl,"M")->Draw("SL");
       GetExtinctionModelProfile(*wl)->Draw("SL");
       
       TLegend *leg = new TLegend(0.4,0.75,0.9,0.9);
       TString header="Run ";
       header+=fRunNumber;
       header+="_";
       header+=fSeqNumber;
       leg->SetHeader(header);
       TString date(fAnalyser->GetTimeString());
       leg->AddEntry("",date,"");
       
       leg->AddEntry(GetExtinctionProfile(*wl),"Total extinction","P");
       leg->AddEntry(GetExtinctionProfile(*wl,"P"),"Aerosols extinction","P");
       leg->AddEntry(GetExtinctionProfile(*wl,"M"),"Rayleigh extinction","L");
       leg->AddEntry(GetExtinctionModelProfile(*wl),"Extinction model","L");
       
       ConfigHandler *myConfig = fAnalyser->GetConfig();
       TString ln="LegParam_";
       ln+=(*wl);
       TString r0="R0 / AC = ";
       r0+=int(myConfig->GetParamR0(*wl)+myConfig->GetLidarAltitude());
       r0+=" m / ";
       r0+=int(myConfig->GetParamAC(*wl)*100);
       r0+="%";
       leg->AddEntry(ln,r0,"");
       
       TString aod ="AOD = ";
       Float_t val=GetAOD(*wl);
       std::stringstream stream;       
       stream << std::fixed << std::setprecision(3) << val;
       aod += stream.str();
       leg->AddEntry("",aod,"");
       
       leg->SetTextSize(0.025);
       leg->Draw("same");             

       i++;
   }
   fSummaryCanvas->Update();
   gDirectory->Add(fSummaryCanvas);

}

// Display summary graphs
void LidarTools::Plotter::DisplayAngstExpAOD()
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Display Angstroem Exponent plot and AODs"<< std::endl;
  
   TString canvasname="AEAODCanvas_";
   canvasname+=fRunNumber;
   canvasname+="_";
   canvasname+=fSeqNumber;
   fAngstExpCanvas = new TCanvas(canvasname , "Angstroem Exponent and Optical depth information", 30,50,650,850);

   fAngstExpCanvas->Divide(2);
   
   fAngstExpCanvas->cd(1);
   // To set axis
   TGraph *fake = new TGraph(2);
   fake->SetPoint(1,-0.5,0);
   fake->SetPoint(2,10,13000);
   fake->Draw("AP");
   fake->SetNameTitle("AngstroemExponentProfile", "Angstroem Exponent 355/532");          
   fake->Draw("AP");
   fake->GetXaxis()->SetTitle("Angstroem Exponent");
   fake->GetXaxis()->SetTitleSize(0.03);
   fake->GetXaxis()->SetLabelSize(0.025);
   fake->GetYaxis()->SetTitle("Altitude (m)");
   fake->GetYaxis()->SetTitleOffset(1.7);
   fake->GetYaxis()->SetTitleSize(0.03);
   fake->GetYaxis()->SetLabelSize(0.025);
   GetAngstExpProfile()->Draw("PS*");

   fAngstExpCanvas->cd(2);
   TGraph *odfake = new TGraph(2);
   odfake->SetPoint(0, 1, 0);
   odfake->SetPoint(1, 5, 1);
   odfake->Draw("AP");
   odfake->SetNameTitle("AOD", "Optical Depths");          
   odfake->Draw("AP");
   //odfake->GetXaxis()->SetTitle("");
   odfake->GetXaxis()->SetTitleSize(0.03);
   odfake->GetXaxis()->SetLabelSize(0.025);
   odfake->GetYaxis()->SetTitle("Optical Depth");
   odfake->GetYaxis()->SetTitleOffset(1.7);
   odfake->GetYaxis()->SetTitleSize(0.03);
   odfake->GetYaxis()->SetLabelSize(0.025);
   
   // Correctly label X-axis OD_M, OD_T, OD_P, Model, Model_P
   odfake->GetXaxis()->SetBinLabel(odfake->GetXaxis()->FindBin(1), "OD Water");
   odfake->GetXaxis()->SetBinLabel(odfake->GetXaxis()->FindBin(2), "OD");
   odfake->GetXaxis()->SetBinLabel(odfake->GetXaxis()->FindBin(3), "AOD");
   odfake->GetXaxis()->SetBinLabel(odfake->GetXaxis()->FindBin(4), "Model OD");
   odfake->GetXaxis()->SetBinLabel(odfake->GetXaxis()->FindBin(5), "Model AOD");
   
   // Draw
   std::vector<Int_t>::iterator wl;
   for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl)
       GetODResults(*wl)->Draw("PS*");

   fAngstExpCanvas->cd();
   fAngstExpCanvas->Update();
   gDirectory->Add(fAngstExpCanvas);

}

// Save to disk
void LidarTools::Plotter::SaveAs(std::string filename)
{
  if(fVerbose) std::cout << "[LidarTools::Plotter] Save to disk: "<<filename<< std::endl;

  TFile *f = new TFile(filename.c_str(), "RECREATE");
  if(fBkgCanvas)
      fBkgCanvas->Write();
  if(fStdDevCanvas)
      fStdDevCanvas->Write();
  if(fRawCanvas)
      fRawCanvas->Write();
  if(fMainCanvas)
      fMainCanvas->Write();
  if(fExtraCanvas)
      fExtraCanvas->Write();
  if(fSummaryCanvas)
      fSummaryCanvas->Write();
  if(fAngstExpCanvas)
      fAngstExpCanvas->Write();

  std::vector<Int_t>::iterator wl;
  for (wl=fWaveLengthVec.begin(); wl!=fWaveLengthVec.end(); ++wl){
       if(GetRawProfile(*wl))
           GetRawProfile(*wl)->Write();       
       if(GetBkgProf(*wl))
           GetBkgProf(*wl)->Write();       
       if(GetBkgHist(*wl))
           GetBkgHist(*wl)->Write();       
       if(GetLnRawPowerProfile(*wl))
           GetLnRawPowerProfile(*wl)->Write();       
       if(GetLnFilteredPowerProfile(*wl))
           GetLnFilteredPowerProfile(*wl)->Write();       
       if(GetLnBinnedPowerProfile(*wl))
           GetLnBinnedPowerProfile(*wl)->Write();
       if(GetBinnedPowerProfile(*wl))
           GetBinnedPowerProfile(*wl)->Write();
       if(GetBinnedPowerDevProfile(*wl))
           GetBinnedPowerDevProfile(*wl)->Write();
       if(GetBinnedPowerDevRatioProfile(*wl))
           GetBinnedPowerDevRatioProfile(*wl)->Write();
           
       if(GetExtinctionProfile(*wl))
           GetExtinctionProfile(*wl)->Write();       
       if(GetExtinctionProfile(*wl, "P"))
           GetExtinctionProfile(*wl, "P")->Write();       
       if(GetExtinctionProfile(*wl, "M"))
           GetExtinctionProfile(*wl, "M")->Write();       
       if(GetExtinctionModelProfile(*wl))
           GetExtinctionModelProfile(*wl)->Write();       

       if(GetBackscatterProfile(*wl))
           GetBackscatterProfile(*wl)->Write();       
       if(GetBackscatterProfile(*wl, "P"))
           GetBackscatterProfile(*wl, "P")->Write();       
       if(GetBackscatterProfile(*wl, "M"))
           GetBackscatterProfile(*wl, "M")->Write();       

        if(GetOpacityProfile(*wl))
           GetOpacityProfile(*wl)->Write();       
       if(GetOpacityModelProfile(*wl))
           GetOpacityModelProfile(*wl)->Write();       
        if(GetTransmissionProfile(*wl))
           GetTransmissionProfile(*wl)->Write();       
       if(GetTransmissionModelProfile(*wl))
           GetTransmissionModelProfile(*wl)->Write();       
       if(GetODResults(*wl))
           GetODResults(*wl)->Write();       
           
 }
  
  // Save special TGraphs
  GetAngstExpProfile()->Write();
  
  // Save Config map
  ConfigHandler *myConfig = fAnalyser->GetConfig();
  std::map <std::string, std::string> map = myConfig->GetMap();
  TString mapName="ConfigMap";
  mapName+=fRunNumber;
  mapName+="_";
  mapName+=fSeqNumber;
 
  f->WriteObject(&map, mapName); 
  //~ ConfigHandler *config = fAnalyser->GetConfig(); 
  //~ f->WriteObject(config,"ConfigHandler"); 

  // Save config to a special TGraph
  // QualityThr, BkgFudgeFactor, SNRatioThreshold, OptimizeAC_Hmin, Fernald_sratio
  // R0_355, R0_532n Fernald_Sp355, Fernald_Sp532, AlignCorr_355, AlignCorr_532
  TGraph *gConf=new TGraph(11);
  TString graphName="ConfigTGraph_";
  graphName+=fRunNumber;
  graphName+="_";
  graphName+=fSeqNumber;
  gConf->SetName(graphName);
  gConf->SetTitle("Main configuration parameters");
  gConf->SetPoint(0, 0, myConfig->GetQualityThr());
  gConf->SetPoint(1, 1, myConfig->GetParamBkgFFactor());
  gConf->SetPoint(2, 2, myConfig->GetSNRatioThreshold());
  gConf->SetPoint(3, 3, myConfig->GetParamOptimizeAC_Hmin());
  gConf->SetPoint(4, 4, myConfig->GetFernald_sratio());
  gConf->SetPoint(5, 5, myConfig->GetParamR0(355));
  gConf->SetPoint(6, 6, myConfig->GetParamR0(532));
  gConf->SetPoint(7, 7, myConfig->GetParamFernaldSp(355));
  gConf->SetPoint(8, 8, myConfig->GetParamFernaldSp(532));
  gConf->SetPoint(9, 9, myConfig->GetParamAC(355)*100);
  gConf->SetPoint(10, 10, myConfig->GetParamAC(532)*100);
  gConf->Write();
  
  // Save file and close it
  f->Write();
  f->Close();
}

TGraph* LidarTools::Plotter::GetExtinctionProfile(Int_t wl, std::string scattering)
{
    if (scattering=="T") // Total
        if(fExtinctionMap.count(wl)) return fExtinctionMap[wl];
        else return 0;
    else if (scattering.compare("P")==0){ // Particles
        if(fExtinctionMap_P.count(wl)) return fExtinctionMap_P[wl];
        else return 0;
        }
    else if (scattering.compare("M")==0){ // Molecules
        if(fExtinctionMap_M.count(wl)) return fExtinctionMap_M[wl];
        else return 0;
        }
    else
        return 0;

}

TGraph* LidarTools::Plotter::GetBackscatterProfile(Int_t wl, std::string scattering)
{
    if (scattering=="T") // Total
        if(fBackscatterMap.count(wl)) return fBackscatterMap[wl];
        else return 0;
    else if (scattering.compare("P")==0){ // Particles
        if(fBackscatterMap_P.count(wl)) return fBackscatterMap_P[wl];
        else return 0;
        }
    else if (scattering.compare("M")==0){ // Molecules
        if(fBackscatterMap_M.count(wl)) return fBackscatterMap_M[wl];
        else return 0;
        }
    else
        return 0;

}

ClassImp(LidarTools::Plotter)
