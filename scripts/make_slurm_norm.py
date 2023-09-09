#!/usr/bin/env python
import sys
import os
import os.path
import getpass
import re
import glob

username = getpass.getuser()
year    = sys.argv[1]
skim    = sys.argv[2]
datamc  = sys.argv[3]
#process = sys.argv[2]

if(skim == "rpvfitnbge0"):
  if(datamc == "data"):
    if year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0_data/UL2016_preVFP"
    elif year == "UL2016": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0_data/UL2016"
    elif year == "UL2017": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0_data/UL2017"
    elif year == "UL2018": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0_data/UL2018"
    else:
      print("Wrong year --- Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")

  elif(datamc == "mc"):
    if year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0/UL2016_preVFP"
    elif year == "UL2016": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0/UL2016"
    elif year == "UL2017": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0/UL2017"
    elif year == "UL2018": inputdir="/data3/nanoprocessing/skimmed_230903_rpvfitnbge0/UL2018"
    else:
      print("Wrong year --- Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")
elif(skim == "dy"):
  if(datamc == "data"):
    if year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/skimmed_230903_dy_data/UL2016_preVFP"
    elif year == "UL2016": inputdir="/data3/nanoprocessing/skimmed_230903_dy_data/UL2016"
    elif year == "UL2017": inputdir="/data3/nanoprocessing/skimmed_230903_dy_data/UL2017"
    elif year == "UL2018": inputdir="/data3/nanoprocessing/skimmed_230903_dy_data/UL2018"
    else:
      print("Wrong year --- Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")

  elif(datamc == "mc"):
    if year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/skimmed_230903_dy/UL2016_preVFP"
    elif year == "UL2016": inputdir="/data3/nanoprocessing/skimmed_230903_dy/UL2016"
    elif year == "UL2017": inputdir="/data3/nanoprocessing/skimmed_230903_dy/UL2017"
    elif year == "UL2018": inputdir="/data3/nanoprocessing/skimmed_230903_dy/UL2018"
    else:
      print("Wrong year --- Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")

if skim == "rpvfitnbge0": skimcut = skim
elif skim == "dy": skimcut = skim
else:
  print("Wrong skimcut --- Skim cut: rpvfitnbge0, dy")

##########################################

process_incl = glob.glob(inputdir+"/*")

for process in process_incl:
#  proc = process.split("/")[-1].split("_13TeV")[0]
  proc = process.split("/")[-1]
#  if 'Run' in proc: continue
  print(proc)

  slurm_file = open("/home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/norm/slurm_"+proc+"_"+skim+"_norm.sh", "w")
  slurm_file.write("#!/bin/sh\n")
  slurm_file.write("#SBATCH -J norm_"+year+"_"+skim+"_"+proc+"\n")
  slurm_file.write("#SBATCH -N 1\n")
  slurm_file.write("#SBATCH -n 1\n")
#  slurm_file.write("#SBATCH --mem=1m\n")
  slurm_file.write("#SBATCH -o /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/norm/out/out_%x.o%j.txt\n")
  slurm_file.write("#SBATCH -e /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/norm/err/err_%x.e%j.txt\n")
  slurm_file.write("cd /home/"+username+"/nanoprocessing\n")
  slurm_file.write("echo $PWD\n")
  slurm_file.write("srun ./norm_weights.exe "+year+" "+proc+" "+"\n")

  slurm_file.close()
