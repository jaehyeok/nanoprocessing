import sys
import os
import os.path
import glob
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString, TH1D
ROOT.gROOT.SetBatch(True)
year = sys.argv[1]

inputdir_1="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/merged_norm_btagv6/"
inputdir_2="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/merged_norm/"
outputdir="plots/yield/"
#os.mkdir(outputdir)

flistdir = "/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
h1 = ROOT.TH1D("h1","",10,-10,10)
h2 = ROOT.TH1D("h2","",10,-10,10)

print('-------------------------------------------btagv6 vs btagv7---------------------------------------------')
print('%46s %3s %15s %15s %15s' %(year, "tag", "btagv6_weight", "btagv7_weight", "ratio"))
print('--------------------------------------------------------------------------------------------------------')

for i, mcname in enumerate(flists):
	tag = mcname.replace('.txt','').replace('flist_','')
	mc1 = TChain("tree")
	mc2 = TChain("tree")
	mc1.Add(inputdir_1+"*"+tag+"*.root")
	mc2.Add(inputdir_2+"*"+tag+"*.root")
	mc1.GetEntry()
	mc2.GetEntry()
	mc1.Draw("weight>>h1","","goff")
	mc2.Draw("weight>>h2","","goff")
	v6_weight = h1.GetMean()
	v7_weight = h2.GetMean()
	if v6_weight==0 or v7_weight==0:
		ratio = 0
	else:
		ratio = v6_weight/v7_weight
	print('%50s %15.6f %15.6f %15.6f' %(tag, v6_weight, v7_weight, ratio))
print('--------------------------------------------------------------------------------------------------------')
