import sys
import os
import os.path
import glob
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString, TH1D
ROOT.gROOT.SetBatch(True)
year = sys.argv[1]

inputdir="/xrootd_user/yjeong/xrootd/nanoprocessing/"+year+"/merged_norm/"

flistdir = "/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
h_btag = ROOT.TH1D("h_btag","",1,0,100)
h_lumi = ROOT.TH1D("h_lumi","",1,0,100)
h_weight = ROOT.TH1D("h_weight","",1,0,100)
h_isr = ROOT.TH1D("h_isr","",1,0,100)
h_pu = ROOT.TH1D("h_pu","",1,0,100)

print('-------------------------------------------btag_dcsv_weight check---------------------------------------------')
print('%46s %3s %15s %15s %15s' %(year, "tag", "w_btag_dcsv", "weight/lumi", "w_pu"))
print('--------------------------------------------------------------------------------------------------------------')

for i, mcname in enumerate(flists):
	tag = mcname.replace('.txt','').replace('flist_','')
	mc = TChain("tree")
	mc.Add(inputdir+tag+"*.root")
	mc.GetEntry()
	mc.Draw("w_btag_dcsv>>h_btag","","goff")
	mc.Draw("w_lumi>>h_lumi","","goff")
	mc.Draw("weight>>h_weight","","goff")
	mc.Draw("w_isr>>h_isr","","goff")
	mc.Draw("w_pu>>h_pu","","goff")
	btag_mean = h_btag.GetMean()
	lumi_mean = h_lumi.GetMean()
	weight_mean = h_weight.GetMean()
	isr_mean = h_isr.GetMean()
	pu_mean = h_pu.GetMean()
	round(lumi_mean,7)
	round(weight_mean,7)
	if lumi_mean==0 or weight_mean==0:
		ratio_weight_lumi = 0
	else:
		ratio_weight_lumi = weight_mean/lumi_mean
	print('%50s %15.2f %15.2f %15.2f' %(tag, btag_mean, ratio_weight_lumi, pu_mean))
print('--------------------------------------------------------------------------------------------------------------')


