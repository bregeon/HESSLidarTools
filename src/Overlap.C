/** @file Overlap.C
 *
 * @brief Overlap class implementation
 *
 * @author Johan Bregeon
*/

#include <iostream> 
#include <vector>
#include "Overlap.hh"

// Constructor
LidarTools::Overlap::Overlap(std::string filename, Bool_t verbose)
{
  fVerbose=verbose;
  if(fVerbose)std::cout<<"[LidarTools::Overlap] Creating a Overlap object"<<std::endl;
  fDataFileName=filename;
  ReadData(); 
}

// Read points from text file
void LidarTools::Overlap::ReadData()
{
  if(fVerbose) std::cout << "[LidarTools::Overlap] Read data from file: "<< fDataFileName << std::endl;

  std::ifstream is_file(fDataFileName.c_str(), std::ifstream::in); 
  // check if file exists
  if(!is_file.good()){
      std::cout << "[LidarTools::Overlap] Overlap file does not exist, aborting\n"<< fDataFileName<< std::endl;

      exit(2);
      } 

    
  std::string line;

  while( std::getline(is_file, line) )
  {
    if (line.compare(0,1,"#") !=0 && line.compare(0,1,"%")!=0)
    {
      std::string tmpline=line.substr(line.find_first_not_of(" "));
      size_t iSplit=tmpline.find_first_of(" ");
      Float_t height  = atof(tmpline.substr(0,iSplit).c_str());
      Float_t overlap = atof(tmpline.substr(iSplit).c_str())/100.;;
      fOverlap.push_back(std::make_pair(height,overlap));
      //if(fVerbose) std::cout<<height<<" "<<overlap<<std::endl;
    }
    else
    {
      if(fVerbose) std::cout<<"[LidarTools::Overlap] Header or commented line: "<<line<<std::endl;
    }
  } // end of while*
  // close file
  is_file.close();

  // Check if we have something read in
  if(fOverlap.size()<2){
      std::cout<<"[LidarTools::Overlap] Overlap function has less than 2 points aborting."<<std::endl;
      exit(1);
      }
}

// Dump data to terminal
void LidarTools::Overlap::Dump()
{
  if(fVerbose) std::cout << "[LidarTools::Overlap] Dump data " << std::endl;
  std::vector < std::pair<float,float> > :: iterator it;
  for (it=fOverlap.begin(); it!=fOverlap.end(); it++)
    std::cout<<it->first<<" "<<it->second<<std::endl;
}

//Get the correction factor for a given height
Float_t LidarTools::Overlap::GetOverlap(Float_t height)
{
  //if(fVerbose) std::cout << "[LidarTools::Overlap] GetOverlap at "<<height<<" m" << std::endl;
  std::vector < std::pair<float,float> > :: iterator it;
  for (it=fOverlap.begin(); it!=fOverlap.end(); it++)
    {
    if (height<it->first)
      return it->second;
    }
  return 1.;
}

ClassImp(LidarTools::Overlap)
