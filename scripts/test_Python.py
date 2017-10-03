import ROOT
ROOT.gROOT.SetBatch(1)

filename = '/data/Hess/data/root_files/run_067220_Lidar_001.root'
Sp355 = "20"
Sp532 = "20"

p = ROOT.LidarTools.LidarProcessor(filename, True)
p.Init()
# p.OverwriteConfigParam("LidarTheta","15");
# 300 bins, min width = 7 m, for 100 bins min width =22 m,
# original binning is 2.3 m
p.OverwriteConfigParam("NBins","100");
p.OverwriteConfigParam("LogBins","0");
p.OverwriteConfigParam("SGFilter", "0")
# p.OverwriteConfigParam("AltMin","800");
# p.OverwriteConfigParam("AltMax","14000");
# p.OverwriteConfigParam("R0","12000");

# p.OverwriteConfigParam("AlgName","Fernald84");
# p.OverwriteConfigParam("Fernald_Sp355",Sp355);
# p.OverwriteConfigParam("Fernald_Sp532",Sp532);
# p.OverwriteConfigParam("Fernald_sratio","1.01");

# p.OverwriteConfigParam("BkgFudgeFactor","1.0");

p.Process(True, True)
p.SaveAs("testPy.root")
