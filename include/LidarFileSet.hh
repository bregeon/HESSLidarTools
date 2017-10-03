/** @file LidarFileSet.hh
 *
 * @brief LidarFileSet class definition
 *
 * Class to read/write a set of Lidar data files in ascii and ROOT format
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_LidarFileSet
#define LIDARTOOLS_LidarFileSet

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#include <TArrayF.h>
#endif

#include <map>
#include <cstdlib>      // atof

#include <LidarTools/LidarFile.hh>

namespace LidarTools {
 
 /** @class LidarFileSet
  * 
  * @brief Manage the Lidar data I/O for the Analyser
  * 
  * 
  */
  class LidarFileSet
  {
  
  public:

    /** @brief class constructor from  a list of paths
     *
     * @param fpath the input file name with a list of file path
     * @param verbose a bool to turn verbosity on or off
     * 
     * @see Read(Int_t) SashFile::FileHandler
     */
    LidarFileSet(std::string fpath, Bool_t verbose=false);

    /** @brief class destructor
     * Deletes the fFileHandler pointer if it exists (i.e. for ROOT files)
     */
    virtual ~LidarFileSet();

    /** @brief Initialize, creates the vector of LidarFile
     */
    int Init();

    /** @brief Merge data in one map and one range array
     */
    int MergeDataSet();

    /** @brief Set verbosity on or off
     *
     * @param verbose a bool, true or false
     */
    void SetVerbose(Bool_t verbose) {fVerbose=verbose;}

    // Getters
    /** @brief Returns the first run number
     * @return int
     */
    int GetRunNumber() const {return fRunNumberVec.front();}

    /** @brief Returns the array of altitudes
     * @return TArrayF
     */
    TArrayF GetRange() const {return fRange;}

    /** @brief Returns the map of signal for all wavelength
     * @return std::map<Int_t, TArrayF>
     */
    std::map<Int_t, TArrayF> GetSignalMap() const {return fSignalMap;}

  private:
    // methods

    // Members
    /** @brief boolean to print some results if true */
    Bool_t fVerbose; //!
    
    /** @brief Input file name with the list of file paths */
    std::string fFilePathList;

    /** @brief Array of raw altitudes */
    TArrayF fRange;
    
    /** @brief Raw data map
     * 
     * The key is the wavelength as an integer.
     * 
     * The value is an array of float
     */
    std::map<Int_t, TArrayF> fSignalMap;
    
    /** @brief Vector of LidarFile pointers */
    std::vector<LidarFile*> fLidarFileVec;

    /** @brief Vector of run number */
    std::vector<int> fRunNumberVec;
    
    
  protected:
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::LidarFileSet,1);
#endif

  }; // class

}; // namespace

#endif
