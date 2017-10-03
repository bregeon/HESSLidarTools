/** @file test_SavGolFilter.C
 *
 * @brief Test the SavGolFilter class
 *
 * @author Johan Bregeon
*/

#include <iostream> 

//#include "LidarTools/SavGolFilter.hh"

void test_SavGolFilter()
{
  float data2[10]={100,200,300,400,500,700,900,1000,1500,1300};

  int nl=2, nr=2, m=2; // left points, right points, polynome degree

  LidarTools::SavGolFilter *savgol = new LidarTools::SavGolFilter(true);
  savgol->CalculateCoefficients(nl, nr, m);
  
  // 5, 5, 4 from numerical recipes
  // a-5, a-4, a-3, a-2, a-1, a0, a1, a2, a3, a4, a5
  // 0.041958 -0.104895 -0.023310  0.139860  0.279720 0.333333  0.279720  0.139860 -0.023310 -0.104895  0.041958  
  // here wrap around order
  // a0, a-1, a-2, a-3, a-4, a-5, a5, a4, a3, a2, a1
  savgol->PrintCoefficients();
  
  savgol->Init(data2,10);
  savgol->MoveWindow(nl, nr, m);

  std::cout<<"\nResults"<<std::endl;
  std::vector<float> meanVec=savgol->GetMeanVec();
  std::vector<float>::iterator val;
  for (val=meanVec.begin(); val!=meanVec.end(); ++val){
        std::cout<<(*val)<<" ";        
        }
  std::cout<<std::endl;
}
