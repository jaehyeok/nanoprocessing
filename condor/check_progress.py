#!/usr/bin/env python
import sys
import os
import os.path
import glob

year=sys.argv[1]

outputdir="/xrootd_user/jaehyeok/xrootd/2016v4/2019_11_07/"
if year == "2017":
	outputdir="/xrootd_user/jaehyeok/xrootd/2017v4/2019_10_23/"
if year == "2018":
	outputdir="/xrootd_user/jaehyeok/xrootd/2018v4/2019_10_23/"

flistdir="/cms/ldap_home/jaehyeok/flist/"+year
print('------------------------------------------------------------------------------------------------')
print('%50s %10s %15s %15s' %("tag", "flist", "processed", "completion"))
print('------------------------------------------------------------------------------------------------')

# compare flist and the output directory, and generate a status summary table
flists = os.listdir(flistdir)
flists.sort()

for flist in flists:
#flist_WW_TuneCP5.txt
	tag=flist.replace('.txt','').replace('flist_','') # extract tag
	if "outputdir" in tag:
		continue
	num_lines = sum(1 for line in open(flistdir+"/"+flist)) # number of files in flist
	num_processed = sum(1 for line in glob.glob(outputdir+"/*"+tag+"*")) # number of files processed
	print('%50s %10d %15d %15.1f%%' %(tag, num_lines, num_processed, 1.0*num_processed/num_lines*100))
print('------------------------------------------------------------------------------------------------')


# generate list of files that have been processed
splits = outputdir.split("/")
list_output_file = open("/cms/ldap_home/jaehyeok/flist/flist_outputdir_"+splits[4]+"_"+splits[5]+".txt", "w")

lines = os.listdir(outputdir)
for line in lines:
	if ".root" not in line: continue
	list_output_file.write(line+"\n")

list_output_file.close()

