#!/usr/bin/env python
import sys
import os
import os.path
import getpass
import re
import glob

username = getpass.getuser()
year    = sys.argv[1]
#process = sys.argv[2]

if year == "2016": inputdir="/data3/nanoprocessing/processed_230607/2016"
elif year == "2017": inputdir="/data3/nanoprocessing/processed_230607/2017"
elif year == "2018": inputdir="/data3/nanoprocessing/processed_230607/2018"
elif year == "UL2016_preVFP": inputdir="/data3/nanoprocessing/processed_241029_signal/UL2016_preVFP"
elif year == "UL2016": inputdir="/data3/nanoprocessing/processed_241029_signal/UL2016"
elif year == "UL2017": inputdir="/data3/nanoprocessing/processed_241029_signal/UL2017"
elif year == "UL2018": inputdir="/data3/nanoprocessing/processed_241029_signal/UL2018"
else:
  print("Wrong year --- [Pre Legacy: 2016, 2017, 2018] --- [Ultra Legacy: UL2016_preVFP, UL2016, UL2017, UL2018")

##########################################

process_incl = glob.glob(inputdir+"/*")

for process in process_incl:
#  proc = process.split("/")[-1].split("_13TeV")[0]
  proc = process.split("/")[-1]
  if 'Run' in proc: continue
  print(proc)

  slurm_file = open("/home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/btagEfficiency/slurm_"+proc+"_btag.sh", "w")
  slurm_file.write("#!/bin/sh\n")
  slurm_file.write("#SBATCH -J btag_"+"_"+year+"_"+proc+"\n")
  slurm_file.write("#SBATCH -N 1\n")
  slurm_file.write("#SBATCH -n 1\n")
#  slurm_file.write("#SBATCH --mem=1m\n")
  slurm_file.write("#SBATCH -o /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/btagEfficiency/out/out_%x.o%j.txt\n")
  slurm_file.write("#SBATCH -e /home/"+username+"/nanoprocessing/condor/submit_scripts/"+year+"/btagEfficiency/err/err_%x.e%j.txt\n")
  slurm_file.write("cd /home/"+username+"/nanoprocessing\n")
  slurm_file.write("echo $PWD\n")
  slurm_file.write("srun ./gen_btagEff.exe "+year+" "+proc+"\n")

  slurm_file.close()
