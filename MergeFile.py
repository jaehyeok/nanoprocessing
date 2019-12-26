import os
import sys
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString
year=sys.argv[1]

simple_in = "/cms/ldap_home/jaehyeok/nanoprocessing/condor/xrootd/"
simple_out = "/cms/scratch/yjeong/CMSSW_7_1_0/src/nanoprocessing/mc_merged/"

inputdir = simple_in+"2016v4/2019_12_10/skim_rpvfitnbge0/"
outputdir = simple_out+"2016_1/"
flists = os.listdir(inputdir)

if year == "2017":
	inputdir = simple_in+"2017v4/2019_10_23/skim_rpvfit/"
	outputdir = simple_out+"2017/"
	flists = os.listdir(inputdir)

if year == "2018":
	inputdir = simple_in+"2018v4/2019_10_23/skim_rpvfit/"
	outputdir = simple_out+"2018/"
	flists = os.listdir(inputdir)

process=[]

for i in range(0,len(flists)-1):
	tag=flists[i].replace('_rpvfitnbge0','').replace('.root','').split('fatjetbaby_')
	process.append(tag[1])

process = list(set(process))

print('---------------------------------------------------'+year+'-------------------------------------------------------')
print('%40s %15s %20s %15s %15s' %("tag", "Merged files", "Not merged event", 'Merged event', 'Event ratio'))
print('--------------------------------------------------------------------------------------------------------------')

for flist in process:
	mc = TChain("tree")
	mc.Add(inputdir+"*_fatjetbaby_"+flist+"_rpvfitnbge0"+".root")
	nfiles=mc.Add(inputdir+"*_fatjetbaby_"+flist+"_rpvfitnbge0"+".root")
	nfiles=str(nfiles)
	t1 = mc.GetEntries()
	mc.Merge(outputdir+flist+"_"+nfiles+".root")
	t2 = mc.GetEntries()
	if t1==0 or t2==0:
		t3 = 0
	else:
		t3 = t1/t2
	t1 = str(t1)
	t2 = str(t2)
	t3 = str(t3)
	print('%40s %15s %20s %15s %15s' %(flist, nfiles, t1, t2, t3))
print('--------------------------------------------------------------------------------------------------------------')
