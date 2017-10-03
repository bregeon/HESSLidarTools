/** @file LidarFileSet.C
 *
 * @brief LidarFileSet class implementation
 *
 * @author Johan Bregeon
*/

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <sstream>

#include "LidarFileSet.hh"

LidarTools::LidarFileSet::LidarFileSet(std::string fpath, Bool_t verbose)
: fVerbose(verbose), fFilePathList(fpath)
{
  if(fVerbose) std::cout<<"[LidarTools::LidarFileSet] Constructor"<<std::endl;
}

LidarTools::LidarFileSet::~LidarFileSet()
{
  if(fVerbose) std::cout<<"[LidarTools::LidarFileSet] Destructor"<<std::endl;
}

// Initialize - create vector of LidarFile
int LidarTools::LidarFileSet::Init()
{
if(fVerbose) std::cout << "[LidarTools::LidarFileSet] Read list of input files"<< std::endl;
  
  std::ifstream is_file(fFilePathList.c_str(), std::ifstream::in); 
  std::string line;
  while( std::getline(is_file, line) ){
    std::cout<<line<<std::endl;
    LidarFile *lidar=new LidarFile(line);      
    fLidarFileVec.push_back(lidar);
    fRunNumberVec.push_back(lidar->GetRunNumber());
    } // end of while
  // close file
  is_file.close();

return 0;
}

// Initialize - create vector of LidarFile
int LidarTools::LidarFileSet::MergeDataSet()
{
if(fVerbose) std::cout << "[LidarTools::LidarFileSet] Merge data from all files"<< std::endl;

  std::vector<LidarFile*>::iterator lf;
  // Loop on maps for
  for(lf=fLidarFileVec.begin();lf!=fLidarFileVec.end();++lf){
    (*lf)->Read();
    std::map<Int_t, TArrayF> signal=(*lf)->GetSignalMap();
    std::map<Int_t, TArrayF>::iterator it;
    for (it=signal.begin(); it!=signal.end(); ++it){
      Int_t wl=it->first;
      TArrayF sarray=signal[wl];
      // if mergemap has key wl then loop to add,       ,
      // else insert wl array pair in map
      // count nb files in another map -- not really needed
      if(fSignalMap.count(wl)>0){
        for(int i=0; i<sarray.GetSize(); i++)
          fSignalMap[wl][i]+=sarray[i];
        }
      else
        fSignalMap[wl]=sarray;
      }  
    }

  // Assume range is the sme for all runs
  lf=fLidarFileVec.begin();
  fRange = (*lf)->GetRange();  

return 0;
}

ClassImp(LidarTools::LidarFileSet)
