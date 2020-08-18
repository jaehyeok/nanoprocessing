import sys
import os
import os.path
import glob
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString, TH1D
ROOT.gROOT.SetBatch(True)
year = sys.argv[1]

#inputdir_1="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/NanoAODv7_data/"
inputdir_1="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/v6/processed/"
inputdir_2="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/processed/"
outputdir="plots/yield/"
#os.mkdir(outputdir)

flistdir = "/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
h1 = ROOT.TH1D("h1","",10,0,4e+11)
h2 = ROOT.TH1D("h2","",10,0,4e+11)

print('-------------------------------------------NanoAODv6 vs NanoAODv7---------------------------------------------')
print('%46s %3s %15s %15s %15s' %(year, "tag", "v6_yield", "v7_yield", "ratio"))
#print('%46s %3s %15s %15s %15s' %(year, "tag", "NanoAODv6_yield", "NanoAODv7_yield", "ratio"))
print('--------------------------------------------------------------------------------------------------------------')

for i, mcname in enumerate(flists):
	tag = mcname.replace('.txt','').replace('flist_','')
	#mc1 = TChain("tree")
	mc1 = TChain("tree")
	mc2 = TChain("tree")
	mc1.Add(inputdir_1+"*_fatjetbaby_"+tag+"*.root")
	mc2.Add(inputdir_2+"*_fatjetbaby_"+tag+"*.root")
	mc1.GetEntry()
	mc2.GetEntry()
	mc1.Draw("event>>h1","","goff")
	mc2.Draw("event>>h2","","goff")
	v6_yield = h1.Integral()
	v7_yield = h2.Integral()
	if v6_yield==0 or v7_yield==0:
		ratio = 0
	else:
		ratio = v6_yield/v7_yield
	print('%50s %15.2f %15.2f %15.8f' %(tag, v6_yield, v7_yield, ratio))
print('--------------------------------------------------------------------------------------------------------------')
