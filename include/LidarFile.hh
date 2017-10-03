/** @file LidarFile.hh
 *
 * @brief LidarFile class definition
 *
 * Class to read/write Lidar data files in ascii and ROOT format
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_LIDARFILE
#define LIDARTOOLS_LIDARFILE

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#include <TArrayF.h>
#endif

#include <map>
#include <cstdlib>      // atof

#include <sashfile/FileHandler.hh>
#include <sash/DataSet.hh>
#include <sash/HESSArray.hh>
#include <sash/Time.hh>
#include <atmosphere/LidarEvent.hh>

namespace LidarTools {
 
 /** @class LidarFile
  * 
  * @brief Manage the Lidar data I/O for the Analyser
  * 
  * 
  */
  class LidarFile
  {
  
  public:
    /** @brief class constructor from file name
     *
     * The constructor is overloaded. This one takes a std::string with
     * the file name.
     * 
     * Both ascii and ROOT file are supported.
     * 
     * @param filename the file name as std::string
     * @param verbose a bool to turn verbosity on or off
     * 
     * @see Read(std::string)
     */
    LidarFile(std::string, Bool_t verbose=false);

    /** @brief class constructor from  a run number
     *
     * The constructor is overloaded. This one takes an Int_t for the
     * run number.
     * 
     * Data are then looked for in standard HESS location, via a 
     * SashFile::FileHandler
     * 
     * @param RunNumber the run number, e.g. 67220
     * @param verbose a bool to turn verbosity on or off
     * 
     * @see Read(Int_t) SashFile::FileHandler
     */
    LidarFile(Int_t, Bool_t verbose=false);

    /** @brief class destructor
     * Deletes the fFileHandler pointer if it exists (i.e. for ROOT files)
     */
    virtual ~LidarFile();

    /** @brief Public proxy to read a Lidar data file from a file path
     * 
     * ROOT and Ascii files are supported via specific private methods
     *
     * 
     * @see ReadAscii ReadROOT ReadFromRunNumber
     * 
     */
    int Read();

    /** @brief Write data to a Lidar data file with normalized file name
     * in the same directory
     *
     * @see atmosphere::LidarEvent
     * 
     */
    void WriteToAscii(); 

    /** @brief Clear data members (file name, range array and signal map)
     * 
     */
    void Reset();

    /** @brief Set verbosity on or off
     *
     * @param verbose a bool, true or false
     */
    void SetVerbose(Bool_t verbose) {fVerbose=verbose;}

    // Getters
    /** @brief Returns the run number
     * @return int
     */
    Int_t GetRunNumber() const {return fRunNumber;}

    /** @brief Returns the sequence number
     * @return int
     */
    Int_t GetSeqNumber() const {return fSeqNumber;}

    /** @brief Returns the run time
     * @return time_t
     */
    time_t GetTime() const { return fTimeStamp.GetTime(); }

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
    /** @brief Read a Lidar data file in ascii format  from a file path
     * 
     * @see ReadROOT
     * 
     */
    void ReadAscii();

    /** @brief Read a Lidar data file in ROOT format from a file path
     * 
     * This is done through a SashFile::FileHandler.
     * 
     * @see ReadAscii
     * 
     */
    int ReadROOT();

    /** @brief Read a Lidar data file in ROOT format from run number
     * 
     * This is done through a SashFile::FileHandler.
     * 
     * @see ReadROOT
     * 
     */
    int ReadFromRunNumber();

    /** @brief Save Sash DataSet locally
     * 
     */
    int SaveDataSetToLocalMap();

    // Members
    /** @brief boolean to print some results if true */
    Bool_t fVerbose; //!

    /** @brief File name */
    std::string fFileName;
    /** @brief Run Number */
    Int_t fRunNumber;
    /** @brief Sequence Number for multiple Lidar shoot for one run */
    Int_t fSeqNumber;
    /** @brief Time Stamp */
    Sash::Time fTimeStamp;
    
    /** @brief File Handler for ROOT file*/
    SashFile::FileHandler *fFileHandler;
        
    /** @brief Array of raw altitudes */
    TArrayF fRange;
    /** @brief Raw data map
     * 
     * The key is the wavelength as an integer.
     * 
     * The value is an array of float
     */
    std::map<Int_t, TArrayF> fSignalMap;
    
    
  protected:
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::LidarFile,2);
#endif

  }; // class

}; // namespace

#endif
