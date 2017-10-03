/** @file GlidingAveFilter.hh
 *
 * @brief GlidingAveFilter class definition
 *
 * Class to bin data using a gliding average filter
 * https://geocap.atlassian.net/wiki/display/ug/Filters+and+gradient+algorithms
 * http://rosettacode.org/wiki/Averages/Simple_moving_average#C.2B.2B
 * http://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average 
 *
 * Implemented from ergastiria/datapros20080421/program/klett/glidingavgan.for
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_GLIDING
#define LIDARTOOLS_GLIDING

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#endif

namespace LidarTools {

 /** @class GlidingAveFilter
  * 
  * @brief Move a window over the data and calcutate the window average
  *
  */
  class GlidingAveFilter
  {
  
  public:

    /** @brief Constructor
     * 
     */
    GlidingAveFilter(Bool_t verbose=false);
    
    /** @brief Destructor
     * 
     */
    virtual ~GlidingAveFilter() {}
    
    /** @brief Initialize, ingest fata
     * 
     *  @param data input data as a pointer to an array of floats
     *  @param nPoints lenght of the array of data
     */
    void Init(float* data, int nPoints);

    /** @brief Run the algorithm
     * 
     * @param nWindows required number of windows (bins)
     */
    void MoveWindow(int nWindows);
    
    // Get Output
    /** @brief Return the vector of mean values */
    std::vector<float> GetMeanVec()     {return fMeanVec;} 
    /** @brief Return the vector of mean values */
    std::vector<float> GetStdDevVec()   {return fStdDevVec;} 

    // Simple getters and setters    
    /** @brief Return the mean */
    float GetWMean()     const    {return fWMean;} 
    /** @brief Return the average deviation */
    float GetWAveDev()   const    {return fWAveDev;} 
    /** @brief Return the standard deviation */
    float GetWStdDev()   const    {return fWStdDev;} 
    /** @brief Return the skewness */
    float GetWSkewness() const    {return fWSkewness;} 
    /** @brief Return the Kurtosis */
    float GetWKurtosis() const    {return fWKurtosis;} 

  private:
    /** @brief Window data moments */
    void Moments(float* wdata, int n);
    
    /** @brief Set flag if algorithm has been run
     * 
     * @param flag a boolean flag
     */
    void SetMoveWindowDone(bool flag) {fMoveWindowDone=flag;}

  protected:
    /** @brief boolean to print some results if true */
    Bool_t fVerbose; //!

    /** @brief boolean to know if something has been done */
    Bool_t fMoveWindowDone; //!
    
    /** @brief number of points */
    int fN;

    /** @brief input data vector */
    std::vector<float> fDataVec;
    /** @brief output data vector of mean values */
    std::vector<float> fMeanVec;
    /** @brief output data vector of StdDev values */
    std::vector<float> fStdDevVec;
    
    /** @brief current window mean */
    float fWMean;
    /** @brief current window Average Deviation */
    float fWAveDev;
    /** @brief current window Standard Deviation */
    float fWStdDev;
    /** @brief current window Skewness */
    float fWSkewness;
    /** @brief current window Curtosis */
    float fWKurtosis;    
    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::GlidingAveFilter,1);
#endif

  }; // class

}; // namespace

#endif
