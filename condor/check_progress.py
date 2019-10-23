#!/usr/bin/env python
import sys
import os
import os.path

outputdir="/xrootd_user/jaehyeok/xrootd/2016v4/2019_10_15/processed/"
splits = outputdir.split("/")
list_output_file = open("/cms/ldap_home/jaehyeok/flist/flist_outputdir_"+splits[4]+"_"+splits[5]+".txt", "w")

lines = os.listdir(outputdir)
for line in lines:
	if ".root" not in line: continue
	list_output_file.write(line+"\n")

list_output_file.close()

