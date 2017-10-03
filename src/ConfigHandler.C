/** @file ConfigHandler.C
 *
 * @brief ConfigHandler class implementation
 *
 * @author Johan Bregeon
*/

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream
#include <cstring>      // strcmp

#include "ConfigHandler.hh"


// Constructor
LidarTools::ConfigHandler::ConfigHandler(Bool_t verbose)
: fVerbose(verbose)
{
  if(fVerbose)std::cout<<"[LidarTools::ConfigHandler] Constructor"<<std::endl;

  // Initialize map to default configuration
  Reset();
}

// Reset config
void LidarTools::ConfigHandler::Reset()
{
    /** Altitudes are in meters and above the LidarAltitude, except itself */
    /** Lidar altitude */
  fConfig["LidarAltitude"] = "1800.";
    /** Lidar theta, 15 degrees for runs since 2011 */
  fConfig["LidarTeta"] = "15.";
    /** QualityThr is the Threshold in V expected to be reached for good data */
  fConfig["QualityThr"]= "-5.0";
    /** AltMin is the Minimum altitude for the signal */
  fConfig["AltMin"]    = "800";
    /** AltMax is the Maximum altitude for the signal */
  fConfig["AltMax"]    = "10000.";
    /** BkgMin is the Minimum altitude for the background estimation */
  fConfig["BkgMin"]    = "20000.";
    /** BkgMax is the Maximum altitude for the background estimation */
  fConfig["BkgMax"]    = "25000.";
    /** BkgFudgeFactor is the background fudge factor to play around with background */
  fConfig["BkgFudgeFactor"] = "1.01";
    /** NBins is the Nb of bins in altitude     */
  fConfig["NBins"]     = "100";
    /** Log Bins is >0 if logarithmic bins are asked for */
  fConfig["LogBins"]     = "1";
    /** SGFilter >0 means to apply the Savitsky-Golay filter */
  fConfig["SGFilter"]     = "0";
  
    /* See Klett inversion method */    
    /** R0 is the Reference Altitude for 355 nm */
  fConfig["R0_355"]        = "10000.";
    /** R0 is the Reference Altitude for 532 nm */
  fConfig["R0_532"]        = "10000.";
    /** Klett_k as in beta=l*alpha^k */
  fConfig["Klett_k"]   = "1.";
    /** Klett_l as in beta=l*alpha^k */
  fConfig["Klett_l"]   = "1.";
    /** TauAltMin is the Minimum altitude from which to integrate the atmosphere opacity */
  fConfig["TauAltMin"] = "800";
    /** TauAltMax is the Maximum altitude from which to integrate the atmosphere opacity */
  fConfig["TauAltMax"] = "4000";

    /** Fernald 84 Sp(355 nm), Lidar extinction to backscatter ratio */
  fConfig["Fernald_Sp355"]= "50";
    /** Fernald 84 Sp(532 nm), Lidar extinction to backscatter ratio */
  fConfig["Fernald_Sp532"]= "70";
    /** Fernald 84 sratio, sratio=1+Beta_p/Beta_m */
  fConfig["Fernald_sratio"]= "1.01";
    /** Inversion algorithm name*/
  fConfig["AlgName"]   = "Fernald84";

    /** Minimal S/N ratio requested to start reverse integration - impacts R0 */
  fConfig["SNRatioThreshold"] = "5.";
   /** Optimize R0 requesting a minimal S/N > SNRatioThreshold */
  fConfig["OptimizeR0"] = "1";
   /** Find out and apply optimal alignment correction factor */
  fConfig["OptimizeAC"] = "0";
   /** Altitude down to which alignment correction factor optimization is done */
  fConfig["OptimizeAC_Hmin"] = "6000";
   /** Mis-alignment correction factor for 355 nm - 2% to 10% */
  fConfig["AlignCorr_355"] = "0.00";
   /** Mis-alignment correction factor for 532 nm - 0% to 2% */
  fConfig["AlignCorr_532"] = "0.00";

  /** Get HESS ROOT or USER */
  std::string softroot= getenv("HESSROOT");
  std::string hessuser = getenv("HESSUSER");
  if(!hessuser.empty()){
    softroot=hessuser;
  } 
 
    /** File name for the atmospheric transmission */
  std::string hessatmo(softroot);
  hessatmo+="/LidarTools/data/atm_trans_1800_1_10_0_0_1800.dat";
  fConfig["AtmoAbsorption"] = hessatmo;

    /** File name for the atmospheric profile */
  std::string hessprof(softroot);
  hessprof+="/LidarTools/data/atmprof10.dat";
  fConfig["AtmoProfile"] = hessprof;
  
    /** File name for the Lidar overlap function */
  std::string overlap(softroot);
  overlap+="/LidarTools/data/overlap_function.txt";
  fConfig["OverlapFunction"] = overlap;
}

// Read config from ASCII file
void LidarTools::ConfigHandler::Read(std::string filename)
{
  if(fVerbose) std::cout << "[LidarTools::ConfigHandler] Read config from file" << std::endl;

  std::ifstream is_file(filename.c_str(), std::ifstream::in); 
  std::string line;

  while( std::getline(is_file, line) )
  {
    std::string key; 

    if (line.compare(0,1,"#") !=0 && line.compare(0,1,"[")!=0)
    {
      unsigned iKey=line.find("=");
      if(iKey>0)
      {
      std::string key   = line.substr(0,iKey);
      std::string svalue = line.substr(iKey+1);
      if(fVerbose) std::cout<<"Found key: '"<<key<<"'"
               <<" value: '"<<svalue<<"'"<<std::endl;
       // clean up white spaces
       key.erase( key.find_last_not_of( " \f\t\v" ) + 1 );
       svalue.erase( svalue.find_last_not_of( " \f\t\v" ) + 1 );
       // save float to map
       SetParam(key, svalue);
       if(fVerbose) std::cout<<"Saved:  '"<<key<<"' = "<<svalue<<std::endl;
      }
    }
    else
    {
      if(fVerbose) std::cout<<"Header or commented line: "<<line<<std::endl;
    }
  } // end of while
  
  // close file !
  is_file.close();

}

// Write config
void LidarTools::ConfigHandler::Write(std::string filename)
{
if(fVerbose) std::cout << "[LidarTools::ConfigHandler] Write config to file" << std::endl;
std::cout << "[LidarTools::ConfigHandler] Write config to file "
          << "-- not implemented yet" << std::endl;

}

// Dump to std::out
void LidarTools::ConfigHandler::Dump()
{
  std::cout << "[LidarTools::ConfigHandler] Dump config map content"<<std::endl;

  std::map <std::string, std::string>::iterator it;
  for (it=fConfig.begin(); it!=fConfig.end(); ++it)
    std::cout << it->first << " = " << it->second << '\n';
  std::cout << "[LidarTools::ConfigHandler] Config Map Dumped"<<std::endl;
 

}

ClassImp(LidarTools::ConfigHandler)
