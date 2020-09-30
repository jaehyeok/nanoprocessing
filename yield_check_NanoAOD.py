import sys
import os
import os.path
import glob
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString, TH1D
ROOT.gROOT.SetBatch(True)
year = sys.argv[1]

inputdir_1="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed_0925/"
inputdir_2="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed/"
outputdir="plots/yield/"
#os.mkdir(outputdir)

flistdir = "/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
h1 = ROOT.TH1D("h1","",10,0,10E+11)
h2 = ROOT.TH1D("h2","",10,0,10E+11)

print('-------------------------------------------processed vs processed_0925---------------------------------------------')
print('%46s %3s %15s %15s %15s' %(year, "tag", "processed", "processed_0925", "ratio"))
#print('%46s %4s %20s' %(year, "tag", "v7_sys_muf[0]"))
print('--------------------------------------------------------------------------------------------------------')

for i, mcname in enumerate(flists):
	tag = mcname.replace('.txt','').replace('flist_','')
	mc1 = TChain("tree")
	mc2 = TChain("tree")
	mc1.Add(inputdir_1+"*_"+tag+"*.root")
	mc2.Add(inputdir_2+"*_"+tag+"*.root")
	mc1.GetEntry()
	mc2.GetEntry()
	mc1.Draw("event>>h1","w_lumi","goff")
	mc2.Draw("event>>h2","w_lumi","goff")
	v6_weight = h1.Integral()
	v7_weight = h2.Integral()
	if v6_weight==0 or v7_weight==0:
		ratio=0
	else:
		ratio=v6_weight/v7_weight
	print('%50s %15.6f %15.6f %15.6f' %(tag, v6_weight, v7_weight, ratio))
	#print('%50s %20.2f' %(tag, v7_weight))
print('--------------------------------------------------------------------------------------------------------')
