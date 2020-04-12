import os
import sys
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString
year=sys.argv[1]

simple_PATH = "/xrootd_user/yjeong/xrootd/nanoprocessing/"

inputdir = simple_PATH+"2016/skim_rpvfitnbge0_v5/"
outputdir = simple_PATH+"2016/merged_rpvfitnbge0_v5/"

if year == "2017":
	inputdir = simple_PATH+"2017/skim_rpvfitnbge0/"
	outputdir = simple_PATH+"2017/merged_rpvfitnbge0/"

if year == "2018":
	inputdir = simple_PATH+"2018/skim_rpvfitnbge0/"
	outputdir = simple_PATH+"2018/merged_rpvfitnbge0/"

flistdir = "/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
process=[]

for flist_ in flists:
	tag=flist_.replace('.txt','').replace('flist_','')
	process.append(tag)

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
