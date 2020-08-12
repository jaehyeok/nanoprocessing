import sys
import os
import os.path
import glob
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString, TH1D
ROOT.gROOT.SetBatch(True)
year = sys.argv[1]

inputdir_1="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/v6/processed/"
inputdir_2="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed/"
outputdir="plots/yield/"
#os.mkdir(outputdir)

flistdir = "/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
h1_mj = ROOT.TH1D("h1_mj","",10,0,3000)
h2_mj = ROOT.TH1D("h2_mj","",10,0,3000)
#c = TCanvas("c","canvas",800,800)

print('-------------------------------------------NanoAODv6 vs NanoAODv7---------------------------------------------')
print('%46s %3s %15s %15s %15s' %(year, "tag", "v6_yield", "v7_yield", "ratio"))
print('--------------------------------------------------------------------------------------------------------------')

for i, mcname in enumerate(flists):
	tag = mcname.replace('.txt','').replace('flist_','')
	mc1 = TChain("tree")
	mc2 = TChain("tree")
	mc1.Add(inputdir_1+"*_fatjetbaby_"+tag+"*.root")
	mc2.Add(inputdir_2+"*_fatjetbaby_"+tag+"*.root")
	mc1.GetEntry()
	mc2.GetEntry()
	mc1.Draw("mj12>>h1_mj","w_lumi","goff")
	mc2.Draw("mj12>>h2_mj","w_lumi","goff")
	v6_yield = h1_mj.Integral()
	v7_yield = h2_mj.Integral()
	if v6_yield==0 or v7_yield==0:
		ratio = 0
	else:
		ratio = v6_yield/v7_yield
	#c.SaveAs(outputdir+tag+".png")
	print('%50s %15.2f %15.2f %15.2f' %(tag, v6_yield, v7_yield, ratio))
	#print('%50s %15.2f %15.2f %15.2f' %("v6", mc1.xsec, mc1.genWeight, mc1.sumWeight))
	#print('%50s %15.2f %15.2f %15.2f' %("v7", mc2.xsec, mc2.genWeight, mc2.sumWeight))
print('--------------------------------------------------------------------------------------------------------------')
