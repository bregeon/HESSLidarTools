/** @file SavGolFilter.C
 *
 * @brief SavGolFilter class implementation
 *
 * @todo Clean ingest and output, use TArrayF ?
 * 
 * @author Johan Bregeon
*/

#include <iostream> 
#include <cmath>
#include <list>

#include "SavGolFilter.hh"

// Constructor
LidarTools::SavGolFilter::SavGolFilter(bool verbose):
fVerbose(verbose)
{
if(verbose)std::cout<<"[LidarTools::SavGolFilter] Constructor"<<std::endl;
}

void LidarTools::SavGolFilter::Init(float* data, int nPoints)
{
    // ingest data
    fN=nPoints;
    for(int i=0; i<nPoints; i++)
        fDataVec.push_back(*(data+i));
        
}


// IMin
int LidarTools::SavGolFilter::IMin(int ia, int ib)
{
  if (ia<=ib) return ia;
  else return ib;
}

// MoveWindow
void LidarTools::SavGolFilter::MoveWindow(int nl, int nr, int m)
{
    if(fVerbose) std::cout<<"[LidarTools::SavGolFilter] Move Window for nl="
                          <<nl<<", nr="<<nr<<", m="<<m<<std::endl;
    
    // calculate coeffs -> stored in fCoeffs
    CalculateCoefficients(nl, nr, m);
    
    // nl = 5 -> first point i=5
    // a0, a1, a2, a3, a4, a5, a-5, a-4, a-3, a-2, a-1
    // fCoeff in wrap around order, and index starts from 1
    std::vector<float>::iterator val;
    // use a list for the window that is used as a circular buffer
    std::list<float> window;
    int i=0;
    for (val=fDataVec.begin(); val!=fDataVec.end(); ++val){
        // Fill window
        window.push_back(*val);
        i++;
        // Just copy the first values, to keep the total number of points
        if(i<=nl) fMeanVec.push_back(*val);
        // When we have the first window filled, start interpolating
        if(i>=nl+nr+1){
            float sum=0.;
            int j=0, cindex=0;
            for(std::list<float>::iterator it=window.begin(); it!=window.end(); ++it){
                if(j<nl+1) cindex=nl-j+1;
                else cindex=(nl+nr+1)+nl-j+1;
                //std::cout<<j<<" "<<cindex<<" "<<fCoeffs[cindex]<<" "<<*it<<std::endl;
                sum+=(*it)*fCoeffs[cindex];
                j++;
                }
            // Save value
            fMeanVec.push_back(sum);
            //remove first element of window - use as a fifo
            window.pop_front();   
            }         
        }
        
    // Just copy the last values from the last window, to keep the total number of points
    for(int j=0; j<nl+nr; j++){
        if(j<nl) window.pop_front();
        else {
            fMeanVec.push_back(window.front());
            window.pop_front();
            }
        }
   SetMoveWindowDone(true);
   if(fVerbose) std::cout<<"[LidarTools::SavGolFilter] Done"<<std::endl;           
}

// Calculate Coefficients
/*------------------------------------------------------------------------------------------- 
 USES lubksb,ludcmp given below. 
 Returns in c(np), in wrap-around order (see reference) consistent with the argument respns 
 in routine convlv, a set of Savitzky-Golay filter coefficients. nl is the number of leftward 
 (past) data points used, while nr is the number of rightward (future) data points, making 
 the total number of data points used nl +nr+1. ld is the order of the derivative desired 
 (e.g., ld = 0 for smoothed function). m is the order of the smoothing polynomial, also 
 equal to the highest conserved moment; usual values are m = 2 or m = 4. 
-------------------------------------------------------------------------------------------*/
void LidarTools::SavGolFilter::CalculateCoefficients(int nl, int nr, int m)
{
    if(fVerbose){
        std::cout<<"[LidarTools::SavGolFilter] Calculate Coefficients for nl="
                 <<nl<<", nr="<<nr<<", m="<<m<<std::endl;
	}
        
  // Store parameters to members
  NLeftPoints=nl;
  NRightPoints=nr;

  // do not understand why np should be different from nr+nl+1
  int np=nr+nl+1;
  
  int ld=0; // for smoothing -- >0 for derivatives not used here
  int d,icode,imj, mm;
  int indx[MMAX+2];
  float fac, sum;
  MAT   a;
  float b[MMAX+2];

  if (np<nl+nr+1 || nl<0 || nr<0 || ld>m || m>MMAX || nl+nr<m) {
    printf("\n Bad args in savgol.\n");
    return;
  }

  for (int i=1; i<=MMAX+1; i++) {
    for (int j=1; j<=MMAX+1; j++) a[i][j]=0.0;
    b[i]=0.0;
    indx[i]=0;
  }

  //Set up the normal equations of the desired leastsquares fit.
  for (int ipj=0; ipj<=2*m; ipj++) { 
    sum=0.0;
    if (ipj==0) sum=1.0;
    for (int k=1; k<=nr; k++) sum += (float) pow(k,ipj);
    for (int k=1; k<=nl; k++) sum += (float) pow(-k,ipj);
    mm=IMin(ipj,2*m-ipj);
    imj=-mm;
    do {
      a[1+(ipj+imj)/2][1+(ipj-imj)/2]=sum;
      imj += 2;
    } while (imj<=mm); 
  }

  LUDCMP(a,m+1,MMAX+1,indx,&d,&icode);    //Solve them: LU decomposition
 
  for (int j=1; j<=m+1; j++) b[j]=0.0;
  b[ld+1]=1.0;    //Right-hand side vector is unit vector, depending on which derivative we want.

  LUBKSB(a,m+1,MMAX+1,indx,b);      //Backsubstitute, giving one row of the inverse matrix.
  
  for (int kk=1; kk<=np; kk++)          //Zero the output array (it may be bigger than the number
    fCoeffs[kk]=0.0;                      //of coefficients.

  for (int k=-nl; k<=nr; k++) {         //Each Savitzky-Golay coefficient is the dot product
    sum=b[1];                       //of powers of an integer with the inverse matrix row.
    fac=1.0;
    for (int mm=1; mm<=m; mm++) {
      fac *= k;
      sum += b[mm+1]*fac;
    } 
    int kk=((np-k) % np) + 1;           //Store in wrap-around order}
    fCoeffs[kk]=sum;
  }
}

// LUCMP
/**************************************************************
* Given an N x N matrix A, this routine replaces it by the LU *
* decomposition of a rowwise permutation of itself. A and N   *
* are input. INDX is an output vector which records the row   *
* permutation effected by the partial pivoting; D is output   *
* as -1 or 1, depending on whether the number of row inter-   *
* changes was even or odd, respectively. This routine is used *
* in combination with LUBKSB to solve linear equations or to  *
* invert a matrix. Return code is 1, if matrix is singular.   *
**************************************************************/
void LidarTools::SavGolFilter::LUDCMP(MAT A, int N, int np, int *INDX, int *D, int *CODE)
{
	
/** @brief number of something */
#define NMX  100

 float AMAX,DUM,SUM,TINY;
 float VV[NMX];
 int   I=0,IMAX=0,J=0,K=0;

 TINY = (float)1e-12;

 *D=1; *CODE=0;

 for (I=1; I<=N; I++) {
   AMAX=0.0;
   for (J=1; J<=N; J++)
     if (fabs(A[I][J]) > AMAX) AMAX=(float) fabs(A[I][J]);
   if (AMAX < TINY) {
     *CODE = 1;
     return;
   }
   VV[I] = (float)1.0 / AMAX;
 }

 for (J=1; J<=N; J++) {
   for (I=1; I<J; I++) {
     SUM = A[I][J];
     for (K=1; K<I; K++)
       SUM -= A[I][K]*A[K][J];
     A[I][J] = SUM;
   }
   AMAX = 0.0;
   for (I=J; I<=N; I++) {
     SUM = A[I][J];
     for (K=1; K<J; K++)
       SUM -= A[I][K]*A[K][J];
     A[I][J] = SUM;
     DUM = VV[I]*(float)fabs(SUM);
     if (DUM >= AMAX) {
       IMAX = I;
       AMAX = DUM;
     }
   }  
   
   if (J != IMAX) {
     for (K=1; K<=N; K++) {
       DUM = A[IMAX][K];
       A[IMAX][K] = A[J][K];
       A[J][K] = DUM;
     }
     *D = -(*D);
     VV[IMAX] = VV[J];
   }

   INDX[J] = IMAX;
   if ((float)fabs(A[J][J]) < TINY)  A[J][J] = TINY;

   if (J != N) {
     DUM = (float)1.0 / A[J][J];
     for (I=J+1; I<=N; I++)  A[I][J] *= DUM;
   } 
 } //j loop

}


/*****************************************************************
* Solves the set of N linear equations A . X = B.  Here A is    *
* input, not as the matrix A but rather as its LU decomposition, *
* determined by the routine LUDCMP. INDX is input as the permuta-*
* tion vector returned by LUDCMP. B is input as the right-hand   *
* side vector B, and returns with the solution vector X. A, N and*
* INDX are not modified by this routine and can be used for suc- *
* cessive calls with different right-hand sides. This routine is *
* also efficient for plain matrix inversion.                     *
*****************************************************************/
void LidarTools::SavGolFilter::LUBKSB(MAT A, int N, int np, int *INDX, float *B)
{

  float SUM;
  int I,II,J,LL;

  II = 0;

  for (I=1; I<=N; I++) {
    LL = INDX[I];
    SUM = B[LL];
    B[LL] = B[I];
    if (II != 0)
      for (J=II; J<I; J++)
        SUM -= A[I][J]*B[J];
    else if (SUM != 0.0)
      II = I;
    B[I] = SUM;
  }

  for (I=N; I>0; I--) {
    SUM = B[I];
    if (I < N)
      for (J=I+1; J<=N; J++)
        SUM -= A[I][J]*B[J];
    B[I] = SUM / A[I][I];
  }

}

// IMin
void LidarTools::SavGolFilter::PrintCoefficients()
{
  std::cout<<"Savitzky-Golay Filter Coefficients"<<std::endl;
  for (int i=1; i<=NLeftPoints+NRightPoints+1; i++)
      printf("%10.6f", fCoeffs[i]);
  std::cout<<std::endl;
}

    /** @brief Mark class implementation for ROOT */
ClassImp(LidarTools::SavGolFilter);
