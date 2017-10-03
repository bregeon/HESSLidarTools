/** @file test_AtmoPlotter.C
 *
 * @brief Test the AtmoPlotter class
 *
 * @author Johan Bregeon
*/

void test_AtmoPlotter()
{
  gROOT->SetStyle("Plain");
 
  LidarTools::AtmoPlotter *plotter = new LidarTools::AtmoPlotter(1835);
 
//  plotter->InitAbsorption("../data/atm_trans_1800_1_10_0_0_1800.dat");
  plotter->InitAbsorption("../data/atm_trans_1835_1_0_0_0_1835.dat");
    
  int blue=355, green=532;
  plotter->FillTauGraph(blue);
  plotter->FillAlphaGraph(blue);
  plotter->FillTauGraph(green);
  plotter->FillAlphaGraph(green);
  TGraph *blueTau=plotter->GetTauGraph(blue);
  TGraph *blueAlpha=plotter->GetAlphaGraph(blue);
  TGraph *greenTau=plotter->GetTauGraph(green);
  TGraph *greenAlpha=plotter->GetAlphaGraph(green);
    
  TCanvas *can = new TCanvas("ProfCan","Profile Canvas", 30, 50, 750, 850);
  can->Divide(2,2);
  can->cd(1);
  blueTau->Draw("AP*");
  blueTau->GetXaxis()->SetTitle("Optical Depth");
  blueTau->GetYaxis()->SetTitle("Altitude (m)");
  blueTau->GetYaxis()->SetRangeUser(0,25000);
  blueTau->GetYaxis()->SetTitleOffset(1.25);
  can->cd(3);
  blueAlpha->Draw("AP*");
  blueAlpha->GetXaxis()->SetTitle("Rayleigh Extinction (m^{-1})");
  blueAlpha->GetYaxis()->SetTitle("Altitude (m)");
  can->cd(2);
  greenTau->Draw("AP*");
  greenTau->GetXaxis()->SetTitle("Optical Depth");
  greenTau->GetYaxis()->SetTitle("Altitude (m)");
  greenTau->GetYaxis()->SetRangeUser(0,25000);
  greenTau->GetYaxis()->SetTitleOffset(1.25);
  can->cd(4);
  greenAlpha->Draw("AP*");
  greenAlpha->GetXaxis()->SetTitle("Rayleigh Extinction (m^{-1})");
  greenAlpha->GetYaxis()->SetTitle("Altitude (m)");
  can->Update();

  TFile *outputFile = new TFile("Test_TransPlotter.root","recreate");
  can->Write();
  blueTau->Write();
  blueAlpha->Write();
  greenTau->Write();
  greenAlpha->Write();
  outputFile->Write();  
}
