#!/usr/bin/env python
import sys
import os
import os.path
import getpass
import re
import glob

username = getpass.getuser()
year = sys.argv[1]
datamc = sys.argv[2]

if datamc == "data":
  if year == "2016": inputdir="/data/data/2016"
  elif year == "2017": inputdir="/data/data/2017"
  elif year == "2018": inputdir="/data/data/2018"
  #elif year == "UL2016_preVFP": inputdir="/data3/store/data/UL2016_preVFP"
  #elif year == "UL2016": inputdir="/data3/store/data/UL2016"
  #elif year == "UL2017": inputdir="/data3/store/data/UL2017"
  #elif year == "UL2018": inputdir="/data3/store/data/UL2018"
  #elif year == "UL2016_preVFP": inputdir="/data3/store/data/SingleMuonDataSet/UL2016_preVFP"
  #elif year == "UL2016": inputdir="/data3/store/data/SingleMuonDataSet/UL2016"
  #elif year == "UL2017": inputdir="/data3/store/data/SingleMuonDataSet/UL2017"
  #elif year == "UL2018": inputdir="/data3/store/data/SingleMuonDataSet/UL2018"
  elif year == "UL2016_preVFP": inputdir="/data3/store/data/UL2016_preVFP"
  elif year == "UL2016": inputdir="/data3/store/data/UL2016"
  elif year == "UL2017": inputdir="/data3/store/data/UL2017"
  elif year == "UL2018": inputdir="/data3/store/data/UL2018"
  else:
    print("Wrong year --- [Pre Legacy: 2016, 2017, 2018] --- [Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")
elif datamc == "mc":
  if year == "2016": inputdir="/data/mc/NanoAODv7_JER/RunIISummer16NanoAODv7"
  elif year == "2017": inputdir="/data/mc/NanoAODv7_JER/RunIIFall17NanoAODv7"
  elif year == "2018": inputdir="/data/mc/NanoAODv7_JER/RunIIAutumn18NanoAODv7"
  elif year == "UL2016_preVFP": inputdir="/data3/store/mc_JER/RunIISummer20UL16NanoAODAPVv9"
  elif year == "UL2016": inputdir="/data3/store/mc_JER/RunIISummer20UL16NanoAODv9"
  elif year == "UL2017": inputdir="/data3/store/mc_JER/RunIISummer20UL17NanoAODv9"
  elif year == "UL2018": inputdir="/data3/store/mc_JER/RunIISummer20UL18NanoAODv9"
  else:
    print("Wrong year --- [Pre Legacy: 2016, 2017, 2018] --- [Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")
else:
    print("Wrong datamc --- You should type [data or mc] in lower case")

#########################################################

process_incl = glob.glob(inputdir+"/*/")

if datamc == "data":
  if year == "UL2016_preVFP": file_incl = glob.glob(inputdir+"/Run2016*/*/*/*HIPM*/*/*")
  elif year == "UL2016": file_incl = glob.glob(inputdir+"/Run2016*/*/*/UL2016*/*/*")
  elif year == "UL2017": file_incl = glob.glob(inputdir+"/Run2017*/*/*/*/*/*")
  elif year == "UL2018": file_incl = glob.glob(inputdir+"/Run2018*/*/*/*/*/*")
  else: file_incl = glob.glob(inputdir+"/*/*/*/*/*/*")
elif datamc == "mc":
  file_incl = glob.glob(inputdir+"/*/*/*/*/*")

for process in process_incl:
#  proc = process.split("/")[-2].split("_13TeV")[0]
  proc = process.split("/")[-2]
  print(proc)
#  if "SMS" not in proc:
#      continue

  flist = open("/home/"+username+"/nanoprocessing/flist/"+year+"/flist_"+proc+".txt","w")
  slurm_file = open("/home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/slurm_"+proc+".sh", "w")

############ make flist ############
  for file in file_incl:
    if process in file: 
      flist.write(file+"\n")

############ get outputdir ############
      outputdir_piece  = file.split(inputdir)[1].split("/")

      if datamc == "data":
        #outputdir = "/data3/nanoprocessing/processed_230724_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_230903_dy_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_230903_rpvfitnbge0_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230904_rpvfitnbge0_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230904_dy_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_231019_singlemu_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_231020_4top_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_231020_qcdfake_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_241010_data_hem/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_241124_d0dz_test_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241124_d0dz_test_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_241201_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_241201_singlemu_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_241201_data_hem/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241201_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241201_data_hem/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241201_singlemu_data/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241201_qcdfake_data/"+year+"/"+outputdir_piece[1]
        outputdir = "/data3/nanoprocessing/skimmed_241201_dy_data/"+year+"/"+outputdir_piece[1]
      elif datamc == "mc":
        #outputdir = "/data3/nanoprocessing/processed_230724/"+year+"/"+outputdir_piece[1]+"/"+outputdir_piece[2]+"/"+outputdir_piece[3]+"/"+outputdir_piece[4]
        #outputdir = "/data3/nanoprocessing/skimmed_230903_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_230903_rpvfitnbge0/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230904_rpvfitnbge0/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230904_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_230915_muonsf/"+year+"/"+outputdir_piece[1]+"/"+outputdir_piece[2]+"/"+outputdir_piece[3]+"/"+outputdir_piece[4]
        #outputdir = "/data3/nanoprocessing/skimmed_230920_rpvfitnbge0/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_230920_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230921_rpvfitnbge0/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230921_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_230929_rpvfitnbge0/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_230929_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230929_rpvfitnbge0/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_230929_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/merged_230929_rpvfitnbge0/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/merged_230929_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_231020_4top/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_231020_qcdfake/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_231020_qcdfake/"+year+"/"+outputdir_piece[1]

        #outputdir = "/data3/nanoprocessing/processed_241029_signal/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241029_signal/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_241029_signal/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_241124_d0dz_test/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241124_d0dz_test/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/processed_241201/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241201_qcdfake/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/skimmed_241201_dy/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_241201/"+year+"/"+outputdir_piece[1]
        #outputdir = "/data3/nanoprocessing/norm_241201_dy/"+year+"/"+outputdir_piece[1]
        outputdir = "/data3/nanoprocessing/norm_250110/"+year+"/"+outputdir_piece[1]

      if not os.path.isdir(outputdir): os.makedirs(outputdir)  # if you do not have directory for outputfile, do uncomment this line

############ make slurm file ############
  slurm_file.write("#!/bin/sh\n")
#  slurm_file.write("#SBATCH --mem-per-cpu=8GB\n")
  slurm_file.write("#SBATCH -J "+year+"_"+proc+"\n")
  slurm_file.write("#SBATCH -N 1\n")
  slurm_file.write("#SBATCH -n 1\n")
  slurm_file.write("#SBATCH -o /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/out/out_%x.o%j.txt\n")
  slurm_file.write("#SBATCH -e /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/err/err_%x.e%j.txt\n")
  slurm_file.write("cd /home/"+username+"/nanoprocessing\n")
  slurm_file.write("srun ./process_nano.exe "+process+" "+outputdir+" "+proc+" flist_"+proc+".txt "+year+"\n")






  slurm_file.close()
  flist.close()



