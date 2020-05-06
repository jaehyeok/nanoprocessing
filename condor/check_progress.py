#!/usr/bin/env python
import sys
import os
import os.path
import glob
import ROOT
from ROOT import TChain, TSelector, TTree, TH1F, TCanvas, TPad, TStyle, TString
ROOT.gROOT.SetBatch(True)
year=sys.argv[1]

outputdir="/xrootd_user/yjeong/xrootd/nanoprocessing/2016/processed_sys_v2/"
if year == "2017":
	outputdir="/xrootd_user/yjeong/xrootd/nanoprocessing/2017/"
if year == "2018":
	outputdir="/xrootd_user/yjeong/xrootd/nanoprocessing/2018/"

flistdir="/cms/ldap_home/yjeong/flist/"+year
flists = os.listdir(flistdir)
'''
print('---------------------------------------------cross section check----------------------------------------------')
print('%46s %3s %26s' %(year, "tag", "Xsection"))
print('--------------------------------------------------------------------------------------------------------------')
for i, mcname in enumerate(flists):
	tag=mcname.replace('.txt','').replace('flist_','')
	mc = TChain("tree")
	mc.Add(outputdir+"*_fatjetbaby_"+tag+".root")
	mc.GetEntry()
	print('%50s %26.1f' %(tag, mc.xsec))
print('--------------------------------------------------------------------------------------------------------------')
'''

print('--------------------------------------------------------------------------------------------------------------')
print('%46s %3s %10s %15s %15s' %(year, "tag", "flist", "processed", "completion"))
print('--------------------------------------------------------------------------------------------------------------')

# compare flist and the output directory, and generate a status summary table

for flist in flists:
#flist_WW_TuneCP5.txt
	tag=flist.replace('.txt','').replace('flist_','') # extract tag
	if "outputdir" in tag:
		continue
	num_lines = sum(1 for line in open(flistdir+"/"+flist)) # number of files in flist
	num_processed = sum(1 for line in glob.glob(outputdir+"*_fatjetbaby_"+tag+"*")) # number of files processed

	print('%50s %10d %15d %15.1f%%' %(tag, num_lines, num_processed, 1.0*num_processed/num_lines*100))
print('--------------------------------------------------------------------------------------------------------------')

# generate list of files that have been processed
splits = outputdir.split("/")
list_output_file = open("/cms/ldap_home/yjeong/flist/flist_outputdir_"+splits[4]+"_"+splits[5]+".txt", "w")

lines = os.listdir(outputdir)
for line in lines:
	if ".root" not in line: continue
	list_output_file.write(line+"\n")

list_output_file.close()

