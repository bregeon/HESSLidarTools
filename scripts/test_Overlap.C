/** @file test_Overlap.C
 *
 * @brief Test the Overlap class
 *
 * Instantiate an overlap object from the default text file
 * and plot the overlap function.
 * 
 * @author Johan Bregeon
*/

void test_Overlap()
{
LidarTools::Overlap *over = new LidarTools::Overlap("../data/overlap_function.txt",verbose=true);

over->Dump();
const int nBins=100;
float hmax=5000., hmin=1.;

// Logarithmic bins
float binwidth=(TMath::Log(hmax)-TMath::Log(hmin))/nBins;
Float_t height[nBins];
for(int i=0; i<nBins;i++)
    height[i]=exp(log(hmin)+i*binwidth);
Float_t overlap[nBins];
for(int i=0; i<nBins; i++)
  overlap[i]=over->GetOverlap(height[i]);

// Plot Overlap
TGraph *gOver=new TGraph(nBins);
gOver->SetTitle("Overlap");

for(int i=0; i<nBins; i++){
  std::cout<<"Ovelap at "<<height[i]<<" m : "<<overlap[i]<<std::endl;
//  gOver->SetPoint(i, overlap[i], height[i]);
  gOver->SetPoint(i, height[i], overlap[i]);
  }
//[0]/(1+exp(-[1]*(x-[2])))  

TCanvas *can=new TCanvas("OverCan","OverlappCanvas",30,50,650,850);
can->Divide(1,2);
can->cd(1);
gOver->Draw("APL");

gOver->GetXaxis()->SetTitle("Overlap fraction");
gOver->GetYaxis()->SetTitle("Altitude (m)");
gOver->GetYaxis()->SetTitleOffset(1.25);


can->cd(2);
TF1 *fo=new TF1("fo","-0.538653+ 0.00232852*x -1.41939e-06*x**2 + 4.02829e-10*x**3 - 4.4005e-14*x**4", 800,3000);
fo->Draw();
}
