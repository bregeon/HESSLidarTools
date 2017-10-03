import ROOT
f=ROOT.TFile.Open("Crab_161116_Sp4070/Crab_161116_Sp4070.root")

mytgs=[]
for tg in f.GetListOfKeys():
  if "AngstExp_" in tg.GetName():
    mytgs.append(f.Get(tg.GetName()))

c=ROOT.TCanvas()
mytgs[0].Draw("AP*")
mytgs[0].SetTitle("Angstroem Exponent 355/532 Sp4070")
mytgs[0].GetXaxis().SetTitle("Angstroem Exponent")
mytgs[0].GetYaxis().SetTitle("Altitude m (a.s.l.)")
for tg in mytgs[1:]:
  tg.Draw("SL")
