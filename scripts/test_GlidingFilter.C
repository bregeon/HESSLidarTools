/** @file test_GlidingFilter.C
 *
 * @brief Test the GlidingAveFilter class
 *
 * @author Johan Bregeon
*/

#include <iostream> 

#include "LidarTools/GlidingAveFilter.hh"

void test_GlidingFilter()
{
  float data[10]={1,2,3,4,5,7,9,10,15,13};
  float data2[10]={100,200,300,400,500,700,900,1000,1500,1300};

  std::cout<<"Hello "<<data[3]<<std::endl;  

  LidarTools::GlidingAveFilter *g = new LidarTools::GlidingAveFilter();
  g->Init(data2, 10);
  // Window width
  g->MoveWindow(5);
  
  std::cout<<std::endl<<"Last Window Moments"<<std::endl;
  std::cout<<"Mean "<<g->GetWMean()<<std::endl;  
  std::cout<<"Ave Dev "<<g->GetWAveDev()<<std::endl;  
  std::cout<<"Std Dev "<<g->GetWStdDev()<<std::endl;  
  std::cout<<"Skewness "<<g->GetWSkewness()<<std::endl;  
  std::cout<<"Kurtosis "<<g->GetWKurtosis()<<std::endl;  
  
  std::vector<float> meanVec=g->GetMeanVec();
  std::vector<float>::iterator val;
  for (val=meanVec.begin(); val!=meanVec.end(); ++val){
        std::cout<<(*val)<<std::endl;        
        }

}
