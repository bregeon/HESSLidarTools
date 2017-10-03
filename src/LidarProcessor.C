/** @file LidarProcessor.C
 *
 * @brief LidarProcessor class implementation
 *
 * @author Johan Bregeon
*/

#include "LidarProcessor.hh"

// Constructor from file path
LidarTools::LidarProcessor::LidarProcessor(std::string filename,
                                           Bool_t verbose)
: fVerbose(verbose),
  fRunNumber(0),
  fFileName(filename),
  fLidarFile(0),
  fAnalyser(0),
  fPlotter(0)
{
  if(fVerbose) std::cout << "[LidarTools::LidarProcessor] Constructor from file name" << std::endl;
}

// Constructor from run number
LidarTools::LidarProcessor::LidarProcessor(Int_t runnumber,
                                           Bool_t verbose)
: fVerbose(verbose),
  fRunNumber(runnumber),
  fFileName(""),
  fLidarFile(0),
  fAnalyser(0),
  fPlotter(0)
{
  if(fVerbose) std::cout << "[LidarTools::LidarProcessor] Constructor from run number" << std::endl;
}


// Destructor
LidarTools::LidarProcessor::~LidarProcessor()
{
  if(fVerbose) std::cout << "[LidarTools::LidarProcessor] Destructor" << std::endl;

  //delete fPlotter;
  delete fAnalyser;
  delete fLidarFile;
}

// Initialize
int LidarTools::LidarProcessor::Init()
{
  if(fVerbose) std::cout << "[LidarTools::LidarProcessor] Initialize" << std::endl;

  // Open file
  if(fRunNumber>0)
      fLidarFile = new LidarTools::LidarFile(fRunNumber, fVerbose);
  else
      fLidarFile = new LidarTools::LidarFile(fFileName, fVerbose);
  // Load data and return if something wrong happened
  int rc=fLidarFile->Read();
  if(rc==0){      
    // Create Analyser  
    fAnalyser  = new LidarTools::Analyser(fLidarFile->GetRange(),
                                        fLidarFile->GetSignalMap(),
                                        fVerbose);  
    fAnalyser->SetRunNumber(fLidarFile->GetRunNumber());
    fAnalyser->SetSeqNumber(fLidarFile->GetSeqNumber());
    fAnalyser->SetTime(fLidarFile->GetTime());
    rc+=fAnalyser->SetConfig();
    }
  
  return rc;
}

// Overwrite an analysis configuration parameter on the fly
void LidarTools::LidarProcessor::OverwriteConfigParam(std::string key, std::string value)
{
  if(fVerbose) std::cout << "[LidarTools::LidarProcessor] Overwrite config" << std::endl;
  fAnalyser->OverwriteConfigParam(key,value);
}

// Process data
int LidarTools::LidarProcessor::Process(Bool_t applyOffset=true, Bool_t display=false)
{
  if(fVerbose) std::cout << "[LidarTools::LidarProcessor] Process data" << std::endl;
  int rc=0;
  rc+=fAnalyser->ProcessData();

  if(rc==0){
    // Plot them with the Plotter class
    fPlotter = new LidarTools::Plotter(fAnalyser, applyOffset, fVerbose);
    fPlotter->InitAll();
    fPlotter->FillAll();
    if (display)
        fPlotter->DisplayAll();
    }
  else
    {
      std::cout << "[LidarTools::LidarProcessor] Failed to process data" << std::endl;
      rc=1;
    }  
  return rc;
}

// Save plot to a ROOT file
void LidarTools::LidarProcessor::SaveAs(std::string fname)
{
  if(fVerbose) std::cout << "[LidarTools::LidarProcessor] Save data" << std::endl;
  // Save to disk
  fPlotter->SaveAs(fname);
}

ClassImp(LidarTools::LidarProcessor)
