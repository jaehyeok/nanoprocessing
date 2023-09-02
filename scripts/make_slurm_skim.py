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
#process = sys.argv[2]

if year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/processed_230731_elecutBased/UL2016_preVFP"
elif year == "UL2016": inputdir="/data3/nanoprocessing/processed_230731_elecutBased/UL2016"
elif year == "UL2017": inputdir="/data3/nanoprocessing/processed_230731_elecutBased/UL2017"
elif year == "UL2018": inputdir="/data3/nanoprocessing/processed_230731_elecutBased/UL2018"
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
  if 'Run' in proc: continue
  print(proc)

  slurm_file = open("/home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/skim/slurm_"+proc+"_"+skim+".sh", "w")
  slurm_file.write("#!/bin/sh\n")
  slurm_file.write("#SBATCH -J "+year+"_skim"+proc+"\n")
  slurm_file.write("#SBATCH -N 1\n")
  slurm_file.write("#SBATCH -n 1\n")
#  slurm_file.write("#SBATCH --mem=1m\n")
  slurm_file.write("#SBATCH -o /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/skim/out/out_%x.o%j.txt\n")
  slurm_file.write("#SBATCH -e /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/skim/err/err_%x.e%j.txt\n")
  slurm_file.write("cd /home/"+username+"/nanoprocessing\n")
  slurm_file.write("echo $PWD\n")
  slurm_file.write("srun ./skim.exe "+year+" "+proc+" "+skim+" "+"\n")

  slurm_file.close()
