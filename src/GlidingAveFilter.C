/** @file GlidingAveFilter.C
 *
 * @brief GlidingAveFilter class implementation
 *
 * @todo Clean ingest and output, use TArrayF ?
 * 
 * @author Johan Bregeon
*/

#include <iostream> 
#include <cmath>

#include "GlidingAveFilter.hh"

// Constructor
LidarTools::GlidingAveFilter::GlidingAveFilter(bool verbose):
fVerbose(verbose), fMoveWindowDone(false)
{
if(verbose)std::cout<<"[LidarTools::GlidingAveFilter] Constructor"<<std::endl;
}

void LidarTools::GlidingAveFilter::Init(float* data, int nPoints)
{
    // ingest data
    // to be removed - should use a const ref
    // copy array of float to a vector of float
    fN=nPoints;
    for(int i=0; i<nPoints; i++)
        fDataVec.push_back(*(data+i));
        
}

void LidarTools::GlidingAveFilter::MoveWindow(int nWidth)
{
    
    if(fVerbose){
        std::cout<<"[LidarTools::GlidingAveFilter] Move Window"<<std::endl;
        std::cout<<"[LidarTools::GlidingAveFilter] Windows of width "<<nWidth<<std::endl;
	}

    // to be removed
    // copy vector of float to a simple array of float
    float mydata[fN];
    std::vector<float>::iterator val;
    int k=0;
    for (val=fDataVec.begin(); val!=fDataVec.end(); ++val){
        mydata[k]=*val;
        k++;
        }
        
    //  move the window
    float wdata[nWidth];
    int nWH=ceil(nWidth/2);
    if(fVerbose)
        std::cout<<"[LidarTools::GlidingAveFilter] Window Half Width "<<nWH<<std::endl;
    // Loop on all point
    for(int i=nWH; i<fN-nWH; i+=nWH){
        if(fVerbose)
            std::cout<<"point "<<i/nWH<<" "<<i-nWH<<"-"<<i+nWH<<" ";
        // Loop on local window
        for(int k=0; k<nWidth;k++){
            // std::cout<<i-nWH+k<<std::endl;
            wdata[k]=mydata[i-nWH+k];
            }
        // Calculate current window moments
        Moments(wdata,nWidth);
        float fWidth=(wdata[nWidth-1]-wdata[0])/2.;
        if(fVerbose){
            float SNratio=GetWMean()/GetWStdDev();
            std::cout<<"\tMean "<<GetWMean()<<"\tWidth "<<fWidth
                     <<"\tStdDev "<<GetWStdDev()
                     <<"\tS/N Ratio "<< SNratio << std::endl;
            }            
        // Fill Mean and StdDev to vectors
        fMeanVec.push_back(GetWMean());
        fStdDevVec.push_back(GetWStdDev());
        }
   SetMoveWindowDone(true);
    
}

// Stolen from the numerical recipes
// NR p.613 --- 14.1 Moments of a Distribution: Mean, Variance, Skewness
// Given an array of data[1..n] , this routine returns its mean ave, average
// deviation adev, standard deviation sdev,
//variance var,skewness skew,and kurtosis curt
void LidarTools::GlidingAveFilter::Moments(float* wdata, int n)
{
    
    float ave=0., adev=0., sdev=0., var=0., skew=0., curt=0.;
    float ep=0.0, sum=0.0;

    //First pass to get the mean.
    for (int j=0;j<n;j++)
        sum += wdata[j];
    ave=sum/n;

// Second pass to get the rst (absolute), second, third, and fourth moments of the
// deviation from the mean.
    for (int j=0;j<n;j++) {
        float s=wdata[j]-ave;
        adev += fabs(s);
        ep   += s;
        var  += pow(s,2);
        skew += pow(s,3);
        curt += pow(s,4);
    }
    adev /= n;
    var=(var-ep*ep/n)/(n-1);

    // Corrected two-pass formula.
    sdev=sqrt(var);

    //Put the pieces together according to the conventional denitions.
    if (var) {
        skew /= (n*var*sdev);
        curt=curt/(n*var*var)-3.0;
        }
    else
        std::cout<<"No skew/kurtosis when variance = 0 (in moment)"<<std::endl;

    // Store results to members
    fWMean=ave;
    fWAveDev=adev;
    fWStdDev=sdev;
    fWSkewness=skew;
    fWKurtosis=curt;
    
    //std::cout<<"\tMean "<<fWMean<<" StdDev "<<fWStdDev<<std::endl;
    
}

    /** @brief Mark class implementation for ROOT */
ClassImp(LidarTools::GlidingAveFilter);
