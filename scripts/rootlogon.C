{
gSystem->AddIncludePath("-I$HESSUSER/include");
gSystem->AddIncludePath("-I$HESSROOT/include");

//gSystem->SetIncludePath("-I$HESSROOT/include");
gROOT->SetMacroPath(".:$HESSROOT");

gSystem->Load("libThread.so");
gSystem->Load("libHistPainter.so");
if(gROOT->IsBatch())
  gSystem->Load("libGui.so");
gSystem->Load("libASImage.so");
gSystem->Load("libsimpletable.so");
gSystem->Load("librootutilities.so");
gSystem->Load("librootgui.so");
gSystem->Load("librootstash.so");
gSystem->Load("librootcrashdata.so");
gSystem->Load("librootcrash.so");
gSystem->Load("librootHSenv.so");
gSystem->Load("librootHSbase.so");
gSystem->Load("librootHSthread.so");
gSystem->Load("librootHSevent.so");
gSystem->Load("librootsashfile.so");
gSystem->Load("librootatmosphere.so");
//gSystem->Load("librootastro.so");
gSystem->Load("librootsashastro.so");
gSystem->Load("librootdisplay.so");
gSystem->Load("librootmonitor.so");
gSystem->Load("libroottracking.so");
gSystem->Load("librootpointing.so");
//gSystem->Load("librootpointingcorrection.so");
gSystem->Load("librootccd.so");
gSystem->Load("librootcalibration.so");
gSystem->Load("librootmathutils.so");
//gSystem->Load("libintensity.so");
//gSystem->Load("libreco.so");
gSystem->Load("librootpariscalibration.so");
gSystem->Load("librootmontecarlo.so");
//gSystem->Load("libparisreco.so");
gSystem->Load("librootparisrunquality.so");
gSystem->Load("librootfromdb.so");

//gSystem->Load("libRootKaskade.so");

gSystem->Load("libLidarTools.so");

gROOT->SetStyle("Plain");
gStyle->SetOptFit(111);
gStyle->SetPalette(1);

//Analysis
// gSystem->Load("libparisanalysis.so");
// gSystem->Load("libparisanalysisSimu.so");
// gSystem->Load("libparisanalysisHillas.so");

//Models
// gSystem->Load("libHessModelGen.so");
// gSystem->Load("libHessModelUtil.so");
// gSystem->Load("libparisanalysisModel.so");

// Load macro
//gROOT->LoadMacro("parisanalysis/scripts/analysis_base.C+");

}



