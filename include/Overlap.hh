/** @file Overlap.hh
 *
 * @brief Overlap class definition
 *
 * Class to handle the Lidar geometric overlap function
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_OVERLAP
#define LIDARTOOLS_OVERLAP

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#endif

#include <cstdlib>
#include <fstream> 

namespace LidarTools {
	
 /** @class Overlap
  * 
  * @brief Handle the Lidar geometric overlap function
  *
  */
  class Overlap
  {
  
  public:
  
    /** @brief Constructor 
     * 
     * Initialize overlap function from text file
     * 
     * @param filename the text file with data points
     * @param verbose a bool to turn verbosity on or off
     */
    Overlap(std::string, Bool_t verbose=true);
    
    /** @brief Destructor */
    virtual ~Overlap() {}
    
    /** @brief Get overlap correction factor at a given height in meters
     * 
     * @param height an altidue in meters
     * @return Float_t
     */
    Float_t GetOverlap(Float_t);

    /** @brief Dump data to terminal */
    void Dump();
  
  private:
    
    /** @brief Read data from the text file
     *
     *  The input text file is the one given to the constructor.
     * 
     * @todo let constructor with no param and 
     * give file name to the read function.
     */
    void ReadData();
    
    /** @brief boolean to print some results if true */
    Bool_t fVerbose; //
    
    /** @brief Data file name */
    std::string fDataFileName;
    
    /** @brief Overlap coefficients as a vector of pair of floats */
    std::vector <std::pair <Float_t, Float_t> > fOverlap;

  protected:

    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::Overlap,1);
#endif

  }; // class

}; // namespace

#endif
