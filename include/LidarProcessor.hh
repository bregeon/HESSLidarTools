/** @file LidarProcessor.hh
 *
 * @brief LidarProcessor class definition
 *
 * Wrapper class to run a full Lidar data analysis
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_LIDARPROCESSOR
#define LIDARTOOLS_LIDARPROCESSOR

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#endif

#include <iostream> 

#include "LidarTools/LidarFile.hh"
#include "LidarTools/Analyser.hh"
#include "LidarTools/Plotter.hh"

namespace LidarTools {

 /** @class LidarProcessor
  * 
  * @brief Wrapper to run a full Lidar data analysis
  *
  */
  class LidarProcessor
  {
  
  public:
  
    /** @brief class constructor from run number
     *
     * The constructor is overloaded. This one takes an interger.
     * 
     * @param runnumber the run number as integer
     * @param verbose a bool to turn verbosity on or off
     * 
     */
    LidarProcessor(Int_t, Bool_t verbose=false);
    
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
     */
    LidarProcessor(std::string, Bool_t verbose=false);


    /** @brief class destructor
     * 
     * Need to delete LidaFilr, Analyser and Plotter
     */
    virtual ~LidarProcessor();

    /** @brief Set verbosity on or off
     *
     * @param verbose a bool, true or false
     */
    void SetVerbose(Bool_t verbose) {fVerbose=verbose;}

    /** @brief Initialize
     *
     */
    int Init();
    
    /** @brief Overwrite a parameter of the analysis configuration.
     *
     * @param key the configuration map parameter key
     * @param value the configuration map parameter value
     */
    void OverwriteConfigParam(std::string, std::string);
    
    /** @brief  Launch data processing
     *
     * @param offset apply LidarAltitude offset on all plots
     * @param display a bool to decide to dispaly or not a ROOT TCanvas
     * with analys results
     */
    int Process(Bool_t offset, Bool_t display);

    /** @brief Save data analysis plots to ROOT file
     *
     * @param fname the output ROOT file path
     */
    void SaveAs(std::string);
    
    /** @brief Get a pointer to the Analyser
     * 
     *  @return Analyser
     */
    LidarTools::Analyser* GetAnalyser() {return fAnalyser;}

    /** @brief Get a pointer to the Plotter
     * 
     *  @return Plotter
     */
    LidarTools::Plotter* GetPlotter() {return fPlotter;}
    
  private:
    /** @brief boolean to print some results on standard out if true */
    Bool_t fVerbose; //!
    /** @brief int run number */
    Int_t fRunNumber;
    /** @brief std::string filename */
    std::string fFileName;
    
    /** @brief a LidarFile pointer for the I/O */
    LidarTools::LidarFile *fLidarFile;
    /** @brief an Analyser to run the data analysis */
    LidarTools::Analyser  *fAnalyser;
    /** @brief a Plotter to display results */
    LidarTools::Plotter   *fPlotter;
  
  protected:
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::LidarProcessor,1);
#endif

  }; // class

}; // namespace

#endif
