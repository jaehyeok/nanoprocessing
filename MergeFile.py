import os
import sys
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString
year=sys.argv[1]

simple_in = "/cms/ldap_home/jaehyeok/nanoprocessing/condor/xrootd/"
simple_out = "/cms/scratch/yjeong/CMSSW_7_1_0/src/nanoprocessing/mc/"
#list_dir = "/cms/ldap_home/yjeong/flist/"
list_dir = "/cms/ldap_home/jaehyeok/flist/"

inputdir = simple_in+"2016v4/2019_12_10/skim_rpvfitnbge0/"
outputdir = simple_out+"2016/"
flists = os.listdir(list_dir+"2016/")

if year == "2017":
	inputdir = simple_in+"2017v4/2019_10_23/skim_rpvfit/"
	outputdir = simple_out+"2017/"
	flists = os.listdir(list_dir+"2017/")

if year == "2018":
	inputdir = simple_in+"2018v4/2019_10_23/skim_rpvfit/"
	outputdir = simple_out+"2018/"
	flists = os.listdir(list_dir+"2018/")

print('---------------------------------------------------'+year+'-------------------------------------------------------')
print('%40s %20s %25s %20s' %("tag", "Merged files", "Not merged event", 'Merged event'))
print('--------------------------------------------------------------------------------------------------------------')

for flist in flists:
	tag = flist.replace('flist_','').replace('.txt','')
	mc = TChain("tree")
	mc.Add(inputdir+"*_fatjetbaby_"+tag+"_rpvfitnbge0"+".root")
	nfiles=mc.Add(inputdir+"*_fatjetbaby_"+tag+"_rpvfitnbge0"+".root")
	nfiles=str(nfiles)
	t1 = mc.GetEntries()
	t1 = str(t1)
	mc.Merge(outputdir+tag+"_"+nfiles+".root")
	t2 = mc.GetEntries()
	t2 = str(t2)
	print('%40s %20s %25s %20s' %(tag, nfiles, t1, t2))
print('--------------------------------------------------------------------------------------------------------------')
