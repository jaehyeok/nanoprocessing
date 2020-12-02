import os
import sys
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString
year=sys.argv[1]#2016, 2017, 2018

simple_PATH = "/xrootd_user/yjeong/xrootd/nanoprocessing/"

inputdir = simple_PATH+year+"/skim_rpvfitnbge0_1130_/"
outputdir = simple_PATH+year+"/merged_norm/"
#os.mkdir(outputdir)

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
