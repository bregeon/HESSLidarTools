#include <fstream>      // std::ifstream
#include <sstream>

#ifndef __CINT__
#include "atmosphere/LidarEvent.hh"

#include <sash/TreeWriter.hh>
#include <sash/Folder.hh>
#include <sash/HESSArray.hh>
#include <sash/List.hh>
#include <sash/RunHeader.hh>
#include <TFile.h>
#endif

// Sash::Time    fTime;          
// Stash::Coordinate fTarget;   
// Int_t fPC, fRoof, fDisplayRoof, fLaser;
// Int_t fVoltage532, fVoltage355, fGain532, fGain355;
// Int_t fFrequency, fNbPoints, fNbShots;
// Float_t fRangeMax, fRangeMin, fLaserPower, fElevationAngle;
// Float_t fNoise532, fNoise355;
// TArrayF fRange;
// TArrayF fSignal532;
// TArrayF fSignal355;
// TArrayF fDarkSignal532;
// TArrayF fDarkSignal355;
// TArrayF fVRsq532;
// TArrayF fVRsq355;


int convertOne(std::string fileName="/data/Hess/data/LidarData_2009_2010/resultsm_20090628_0128",
             int runNumber=99999)
{
  // Create output file name with run number  
  TString outfile="";
  int ii=1;
  while(ii<50){
    outfile="run_0";
    outfile+=runNumber;
    outfile+="_Lidar_00";
    outfile+=ii;
    outfile+=".root";
    std::cout<<outfile<<std::endl;
    TFile f(outfile);
    if (f.IsZombie()) {
       //cout << "File does not exist, good to go" << endl;
       ii=100;
      }
    else{
       cout << "File exists, increase index by one" << endl;
       ii+=1;
      }
    }
  // Open output file
  TFile *f = new TFile(outfile, "RECREATE");
  
  Sash::TreeWriter* w = new Sash::TreeWriter("TreeWriter", "TextToRootConverter");
  Sash::Folder* folder = Sash::Folder::GetFolder("Lidar");
  Sash::HESSArray* hess = folder->GetHESSArray();

  Sash::RunHeader* runHeader = hess->Handle<Sash::RunHeader>();  
  // Fill run header information
  runHeader->Print();
   
  // Get the time stamp from the file name
  // resultsm_20090628_0128 =  %y%m%d%H%M
  std::string timestr = fileName.substr(fileName.find_last_of("/")+1);
  struct tm tm;
  memset(&tm, 0, sizeof(struct tm));
  strptime(timestr.c_str(), "resultsm_%Y%m%d_%H%M", &tm);
  Sash::Time fTimeStamp = Sash::Time(mktime(&tm));                    
  std::cout<<"Time stamp from file name: "<<" "<<timestr<<" "<<fTimeStamp<<std::endl;

  // Fill Lidar data
  for(unsigned int i=0; i<3; ++i) {    

    // Create a lidar event
    Atmosphere::LidarEvent* rEvent = hess->Handle<Atmosphere::LidarEvent>();
    
    // Read data file and file data container
    std::ifstream is_file(fileName.c_str(), std::ifstream::in); 
    std::string line;
    std::getline(is_file, line); // first line is junk
    // Parameters
    std::vector<std::string> params;
    for(int k=0;k<15;k++){
      std::getline(is_file, line);
      unsigned iKey=line.find(":");       
      std::string key   = line.substr(0,iKey);
      std::string svalue = line.substr(iKey+1);
      //std::cout<<"Found key: '"<<key<<"'"
      //        <<" value: '"<<svalue<<"'"<<std::endl;
      // clean up white spaces
      svalue.erase( svalue.find_last_not_of( " \f\t\v" ) + 1 );
      params.push_back(svalue);
      }
    std::getline(is_file, line); // column headers

    // Save parameters
    rEvent->GetVoltage355()= atoi(params[0].c_str());
    rEvent->GetGain532()   = atol(params[1].c_str());
    rEvent->GetGain355()   = atol(params[2].c_str());
    rEvent->GetVoltage532()= atoi(params[3].c_str());
    rEvent->GetFrequency() = atoi(params[4].c_str());
    rEvent->GetNbPoints()  = atoi(params[5].c_str());
    rEvent->GetRangeMax()  = atof(params[6].c_str());
    rEvent->GetRangeMin()  = atof(params[7].c_str());
    rEvent->GetNbShots()   = atoi(params[8].c_str());
    rEvent->GetLaserPower()= atof(params[9].c_str());
    //rEvent->GetElevationAngle()=atof(params[10].c_str());
    rEvent->GetNoise355()  = atof(params[11].c_str());
    rEvent->GetNoise532()  = atof(params[12].c_str());
    rEvent->GetElevationAngle() = atof(params[13].c_str()); // this one changes !
    //std::cout<<line<<std::endl;
    // AZIMUTH (degres) : 140.00

    std::vector <float> vrange, vwl1, vwl2;
 
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
    TArrayF fRange(Npoints);
    TArrayF awl1(Npoints);
    TArrayF awl2(Npoints);
    TArrayF vr2wl1(Npoints);
    TArrayF vr2wl2(Npoints);
  
    // Fill arrays
    for(int l=0; l<Npoints; l++)
      {
      fRange[l]=vrange[l];
      awl1[l]=vwl1[l];
      awl2[l]=vwl2[l];
      vr2wl1[l]=-vwl1[l]*vrange[l]*vrange[l];
      vr2wl2[l]=-vwl2[l]*vrange[l]*vrange[l];
      }  
    // Fill Tree
    rEvent->GetTime()     =fTimeStamp;
    rEvent->GetRange()    =fRange;
    rEvent->GetSignal355()=awl1;
    rEvent->GetSignal532()=awl2;
    rEvent->GetVRsq355()  =vr2wl1;
    rEvent->GetVRsq532()  =vr2wl2;
    std::cout<<endl;

    folder->NextEntry(w, "Handle<Atmosphere::LidarEvent>");
  }
  
  f->Write();
  f->Close();
  return 0;
}

void oldTxtToRootConverter(std::string listname="Filename_RunNumber_list.txt")
{
    std::ifstream is_file(listname.c_str(), std::ifstream::in); 
    std::string fname;
    int run;
    while(!is_file.eof()){
      is_file>>fname>>run;
      std::cout<<fname<<" "<<run<<std::endl;
      if(run>54687)
        convertOne(fname, run);
      }
}
