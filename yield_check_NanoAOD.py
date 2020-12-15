import sys
import os
import os.path
import glob
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString, TH1D
ROOT.gROOT.SetBatch(True)
year = sys.argv[1]

inputdir_1="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed_1214/"
inputdir_2="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed_1211/"
outputdir="plots/yield/"
#os.mkdir(outputdir)

flistdir = "/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
h1 = ROOT.TH1D("h1","",10,-1,1)
h2 = ROOT.TH1D("h2","",10,-1,1)
h3 = ROOT.TH1D("h3","",10,-1,1)

print('-------------------------------------------processed vs processed_0925---------------------------------------------')
print('%41s %8s %15s %15s %15s' %(year, "tag", "ratio_1214/1211", "ratio/l1pre_nom", "l1pre_nom"))
print('--------------------------------------------------------------------------------------------------------')

for i, mcname in enumerate(flists):
	tag = mcname.replace('.txt','').replace('flist_','')
	mc1 = TChain("tree")
	mc2 = TChain("tree")
	mc1.Add(inputdir_1+"*_"+tag+"*.root")
	mc2.Add(inputdir_2+"*_"+tag+"*.root")
	mc1.GetEntry()
	mc2.GetEntry()
	mc1.Draw("weight>>h1","","goff")
	mc2.Draw("weight>>h2","","goff")
        mc1.Draw("l1pre_nom>>h3","","goff")
	v6_weight = h1.GetMean()
	v7_weight = h2.GetMean()
	l1pre_nom = h3.GetMean()
	if v6_weight==0 or v7_weight==0:
		ratio=0
	else:
		ratio=v6_weight/v7_weight #should be l1pre_nom weight
	ratio_2 = ratio/l1pre_nom #should be 1
	print('%50s %15.6f %15.6f %15.6f' %(tag, ratio, ratio_2, l1pre_nom))
print('--------------------------------------------------------------------------------------------------------')
