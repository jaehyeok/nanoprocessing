#!/usr/bin/env python
import sys
import os
import os.path
import getpass
import re
import glob

#############################################################################
# After processed, we need to get the path of the processed file for skimming
#############################################################################

username = getpass.getuser()
year   = sys.argv[1]
datamc = sys.argv[2]
skim   = sys.argv[3]

if datamc == "data":
  if year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"_data/UL2016_preVFP"
  elif year == "UL2016": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"_data/UL2016"             #2016F
  elif year == "UL2017": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"_data/UL2017"
  elif year == "UL2018": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"_data/UL2018"
  else:
    print("Wrong year --- [Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")
elif datamc == "mc":
  if year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"/UL2016_preVFP"
  elif year == "UL2016": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"/UL2016"
  elif year == "UL2017": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"/UL2017"
  elif year == "UL2018": inputdir="/data3/nanoprocessing/norm_230904_"+skim+"/UL2018"
  else:
    print("Wrong year --- [Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")
else:
    print("Wrong datamc --- You should type [data or mc] in lower case")

#########################################################

process_incl = glob.glob(inputdir+"/*/")

if datamc == "data":
  if year == "UL2016_preVFP": file_incl = glob.glob(inputdir+"/Run2016*/*")
  elif year == "UL2016": file_incl = glob.glob(inputdir+"/Run2016*/*")
  elif year == "UL2017": file_incl = glob.glob(inputdir+"/Run2017*/*")
  elif year == "UL2018": file_incl = glob.glob(inputdir+"/Run2018*/*")
  else: file_incl = glob.glob(inputdir+"/*/*/*/*/*/*")
elif datamc == "mc":
  file_incl = glob.glob(inputdir+"/*/*")

for process in process_incl:
#  proc = process.split("/")[-2].split("_13TeV")[0]
  proc = process.split("/")[-2]
  print(proc)

  flist = open("/home/"+username+"/nanoprocessing/flist/norm/"+year+"/flist_"+proc+"_"+skim+".txt","w")

############ make flist ############
  for file in file_incl:
    if process in file: 
      flist.write(file+"\n")

  flist.close()



