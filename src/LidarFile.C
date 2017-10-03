/** @file LidarFile.C
 *
 * @brief LidarFile class implementation
 *
 * @author Johan Bregeon
*/

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <sstream>
#include <cmath>        // abs

#include "LidarFile.hh"

LidarTools::LidarFile::LidarFile(std::string filename, Bool_t verbose)
: fVerbose(verbose), fFileName(filename), fRunNumber(0), fSeqNumber(1), fFileHandler(0)
{
  if(fVerbose) std::cout<<"[LidarTools::LidarFile] Constructor"<<std::endl;
}

LidarTools::LidarFile::LidarFile(Int_t RunNumber, Bool_t verbose)
: fVerbose(verbose), fFileName(""), fRunNumber(RunNumber), fFileHandler(0)
{
  if(fVerbose) std::cout<<"[LidarTools::LidarFile] Constructor"<<std::endl;
}

LidarTools::LidarFile::~LidarFile()
{
  if(fVerbose) std::cout<<"[LidarTools::LidarFile] Destructor"<<std::endl;
  if(fFileHandler)
      delete fFileHandler;
}

// Read a Lidar data file may be text or ROOT
int LidarTools::LidarFile::Read()
{
if(fVerbose) std::cout << "[LidarTools::LidarFile] Read data"<< std::endl;

int rc=0;

// Open data file from run number
if(fRunNumber>0)
  rc=ReadFromRunNumber();  
else{
// Open data file from file name
  if(fVerbose) std::cout << "[LidarTools::LidarFile] Opening file "<<fFileName << std::endl;
  if (fFileName.substr(fFileName.find_last_of(".") + 1) == "root")
      rc=ReadROOT();
  else if (fFileName.substr(fFileName.find_last_of(".") + 1) == "txt")
      ReadAscii();
  else{
      std::cout <<"Unknown file type, aborting..."<<std::endl;
      rc=2;
      }

  //Assume we try to read a standard file and try to get the run number
  // run_054209_Lidar_0010.root.txt
  std::string justname=fFileName.substr(fFileName.find_last_of('/')+1);
  std::string runstr=justname.substr(4, 6);
  std::istringstream(runstr) >> fRunNumber;
  if(fRunNumber<1000 || fRunNumber>200000)
    std::cout<<"[LidarTools::LidarFile::Read] Probably could not parse Run number, found: "
             <<fRunNumber<<std::endl;
  // Get also the sequence number
  std::string seqstr=justname.substr(17, 4);
  std::istringstream(seqstr) >> fSeqNumber;
  if(fSeqNumber<1 || fSeqNumber>100)
    std::cout<<"[LidarTools::LidarFile::Read] Probably could not parse Sequnce number, found: "
             <<fSeqNumber<<std::endl;

  }
return rc;
}

// Read a Lidar ROOT data from run number
int LidarTools::LidarFile::ReadFromRunNumber()
{
if(fVerbose) std::cout << "[LidarTools::LidarFile] Run number "<<fRunNumber << std::endl;
  // Clean up before loading data
  Reset();
  // Open data file from run number
  fFileHandler = new SashFile::FileHandler(fRunNumber); //, SashFile::FileHandler::mMeteoData);
  // Save to local map 
  int rc=SaveDataSetToLocalMap();
  // assume here that fSeqNumber=1 as initialized
  return rc;
}

// Read a Lidar ROOT data
int LidarTools::LidarFile::ReadROOT()
{
if(fVerbose) std::cout << "[LidarTools::LidarFile] ROOT file "<<fFileName << std::endl;
  // Clean up before loading data
  Reset();
  // Open data file
  fFileHandler = new SashFile::FileHandler();
  fFileHandler->AddFile(fFileName.c_str());
  
  // Hack
  //TFile *lidarf=new TFile(fFileName.c_str());
  //Sash::DataSet* lidar2 = (Sash::DataSet*)lidarf->Get("Lidar");
  //fFileHandler->AddDataSet(lidar2);
  
  // Save to local map 
  int rc=SaveDataSetToLocalMap();
  return rc;
}

int LidarTools::LidarFile::SaveDataSetToLocalMap()
{
  Sash::DataSet* lidar = fFileHandler->Get("Lidar");
  if(!lidar){
      std::cerr<<"[LidarTools::LidarFile] File corrupted aborting..."<<std::endl;
      return 1;
      }
  lidar->GetEntry(0);  

  Sash::HESSArray *hessarray=lidar->GetHESSArray();
  
  const Atmosphere::LidarEvent * event = hessarray->Get<Atmosphere::LidarEvent>();
  
  // Store Time Stamp
  fTimeStamp = event->GetTime();
  if(fVerbose) std::cout << "Time Stamp " << fTimeStamp << std::endl;
    
  // Get raw data into local data map
  fRange=event->GetRange();
  fSignalMap[532]=event->GetSignal532();
  fSignalMap[355]=event->GetSignal355();
  
  // return code
  return 0;
}

// Read a Lidar text file
void LidarTools::LidarFile::ReadAscii()
{
if(fVerbose) std::cout << "[LidarTools::LidarFile] Ascii file "<<fFileName << std::endl;
  // Clean up before loading data
  Reset();
  // Read data file and file data container
  std::ifstream is_file(fFileName.c_str(), std::ifstream::in); 
  std::string line;

  std::vector <float> vrange, vwl1, vwl2;
  // Read first line that should be the time stamp
  // Fri Jul  1 20:10:05 2011 = %s %M  %d %H:%M:%S %Y
  std::getline(is_file, line);
  struct tm tm;
  memset(&tm, 0, sizeof(struct tm));
  strptime(line.c_str(), "%a %b  %d %H:%M:%S %Y", &tm);
  fTimeStamp = Sash::Time(mktime(&tm));                    
  if(fVerbose) std::cout<<"Time stamp from header: "<<fTimeStamp<<std::endl;
 
  // Read Lidar data from file
  while( std::getline(is_file, line) )
  {
      float range=0., wl1=0., wl2=0.;
      std::istringstream sstream;
      sstream.str(line);
      sstream>>range>>wl1>>wl2;
      //std::cout<<range<<" "<<wl1<<" "<<wl2<<std::endl;
      vrange.push_back(range);
      vwl1.push_back(wl1);
      vwl2.push_back(wl2);
  } // end of while
  // close file
  is_file.close();
  // Finish
  int Npoints=vrange.size();
  fRange.Set(Npoints);
  TArrayF awl1(Npoints);
  TArrayF awl2(Npoints);
  
  // Fill arrays
  for(int i=0; i<Npoints; i++)
    {
    fRange[i]=vrange[i];
    if(fRunNumber<60248){
       awl1[i]=-abs(vwl1[i]);
       awl2[i]=-abs(vwl2[i]);
       }
    else{
       awl1[i]=vwl1[i];
       awl2[i]=vwl2[i];
       }
    }
  // Save into map assuming default wave lengths
  fSignalMap[355]=awl1;
  fSignalMap[532]=awl2;
  
}

// Clean up data containers
void LidarTools::LidarFile::Reset()
{
if(fVerbose) std::cout << "[LidarTools::LidarFile] Reset "<< std::endl;
  // clean up
  fSignalMap.clear();
  fRange.Reset();

}

//Write data to ASCII file
void LidarTools::LidarFile::WriteToAscii()
{
if(fVerbose) std::cout << "[LidarTools::LidarFile] Write data to file" << std::endl;

  // FileName - run_072070_Lidar_001.root.txt - fFileName
  std::string filename=fFileName+".txt";
  
  // Open data file
  std::ofstream os_file(filename.c_str(), std::ifstream::out);
  // Get Date and format it
  time_t time=GetTime();
  os_file<<ctime(&time);

  // Get Range and Signal
  TArrayF range = GetRange();
  TArrayF s355 = GetSignalMap()[355];
  TArrayF s532 = GetSignalMap()[532];

  // dump to file
  for(int i=0; i<range.GetSize(); i++) {
     os_file << range[i] << " " << s355[i] << " " << s532[i] << std::endl;
    } 

  // close file
  os_file.close();
if(fVerbose) std::cout << "[LidarTools::LidarFile] "<<filename<< " written on disk." << std::endl;

}


ClassImp(LidarTools::LidarFile)
