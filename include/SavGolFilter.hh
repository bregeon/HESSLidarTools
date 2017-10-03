/** @file SavGolFilter.hh
 *
 * @brief SavGolFilter class definition
 *
 * Class to filter noise
 *
 * ---------------------------------------------------------------- *
 * Reference:  "Numerical Recipes By W.H. Press, B. P. Flannery,    *
 *              S.A. Teukolsky and W.T. Vetterling, Cambridge       *
 *              University Press, 1986 - 1992" [BIBLI 08].          *
 *                                                                  *
 *                              C++ Release By J-P Moreau, Paris    *
 *                                      (www.jpmoreau.fr)           *
 ******************************************************************
 * 
 * @author Johan Bregeon
*/

#ifndef LIDARTOOLS_SAVGOL
#define LIDARTOOLS_SAVGOL

#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
#include <TROOT.h>
#endif

namespace LidarTools {

/** @brief Maximum number of filter coefficients */
#define  NP      50
/** @brief Maximum order of smoothing polynomial */
#define  MMAX     6    
/** @brief matrix */
typedef  float MAT[MMAX+2][MMAX+2]; // Matrix


 /** @class SavGolFilter
  * 
  * @brief Move a window over the data and calcutate the window average
  *
  */
  class SavGolFilter
  {
  
  public:

    /** @brief Constructor
     * 
     */
    SavGolFilter(Bool_t verbose=false);
    
    /** @brief Destructor
     * 
     */
    virtual ~SavGolFilter() {}
    
    /** @brief Initialize, ingest data
     *
     *  @param data input data as a pointer to an array of floats
     *  @param nPoints lenght of the array of data
     */
    void Init(float* data, int nPoints);

    /** @brief Run the smoothing algorithm 
     *
     *  @param nl number of points on the left of the smoothing window
     *  @param nr number of points on the right of the smoothing window
     *  @param m smoothing polynome degree
     */
    void MoveWindow(int nl, int nr, int m);

    /** @brief Calculate the Savitsky-Golay coefficients
     * 
     *  @param nl number of points on the left of the smoothing window
     *  @param nr number of points on the right of the smoothing window
     *  @param m smoothing polynome degree
     */
    void CalculateCoefficients(int nl, int nr, int m);

    /** @brief Print the Savitsky-Golay coefficients */
    void PrintCoefficients();
    
    // Get Output
    /** @brief Return the vector of mean values */
    std::vector<float> GetMeanVec()     {return fMeanVec;} 


  private:

    /** @brief Set flag to true if the algorithm has run
     * 
     *  @param flag the boolean flag 
     */
    void SetMoveWindowDone(bool flag) {fMoveWindowDone=flag;}

    /** @brief fCoeffs Savitsky Golay Coeffcients
     *
     *  Note: index 0 not used here.
     */
    float fCoeffs[NP+1];

    /** @brief boolean to print some results if true */
    int NRightPoints;
    /** @brief boolean to print some results if true */
    int NLeftPoints;

    /** @brief IMin internal function from NR
     * 
     *  @param ia lower index
     *  @param ib upper index
     */
    int IMin(int ia, int ib);

    /** @brief Lower Upper Matrix decomposition 
     *  from NR
     */
    void LUDCMP(MAT A, int N, int np, int *INDX, int *D, int *CODE);
    
    /** @brief Solves Matrix linear equations
     *  from NR 
     */
    void LUBKSB(MAT A, int N, int np, int *INDX, float *B);
    
    
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


    
#if defined(USE_ROOT) || defined(ROOT_VERSION_CODE)
    ClassDef(LidarTools::SavGolFilter,1);
#endif

  }; // class

}; // namespace

#endif
