#!/usr/bin/env python
import sys
import os
import os.path
import getpass

username = getpass.getuser()

#  ./norm_weights.exe /xrootd_user/"+username+"/xrootd/nanoprocessing/2016/merged_rpvfitnbge0/ SMS-T1tbs_RPV_mGluino1600 /xrootd_user/jaehyeok/babies/2016v6/norm_weights /xrootd_user/"+username+"/xrootd/nanoprocessing/2016/processed

year=sys.argv[1] ##2016, 2017, 2018

inputdir     = "/data2/nanoprocessing/"+year+"/merged_rpvfitnbge0_0317/"
outdir       = "/data2/nanoprocessing/"+year+"/merged_norm_0317/"
prenormdir   = "/data2/nanoprocessing/"+year+"/processed_0317/"
#os.mkdir(outdir)

# list of tags (=processes)
f_list = os.listdir(inputdir)
tag_list = sorted(list(set(f_list)))

for files in tag_list:
  # skip data
  if("JetHT" in files):
    continue;

  # this works for only merged files
  # for processed different treatment is needed
  filename = files.split(".")[0]
  tag  = filename.rsplit('_', 1)[0]

  # generate two file lists
  flist_prenorm = open("/home/"+username+"/flist/norm/"+year+"/flist_prenorm_"+tag+".txt", "w")
  for files in os.listdir(prenormdir):
    #if(tag+"_" in files):
    if(tag in files):
      flist_prenorm.write(prenormdir+"/"+files+"\n")
  flist_tonorm  = open("/home/"+username+"/flist/norm/"+year+"/flist_tonorm_"+tag+".txt", "w")
  for files in os.listdir(inputdir):
    if(tag in files):
      flist_tonorm.write(inputdir+"/"+files+"\n")

  condor_file = open("submit_scripts_norm/"+year+"/slurm_"+tag+"_norm.sh", "w")
  condor_file.write("#!/bin/sh \n")
  condor_file.write("#SBATCH -J " + tag +"\n")
  condor_file.write("#SBATCH -N 1\n")
  condor_file.write("#SBATCH -n 1\n")
  condor_file.write("#SBATCH -o log/run_%x.o%j.txt\n")
  condor_file.write("#SBATCH -e log/run_%x.e%j.txt\n")
  condor_file.write("cd ~/nanoprocessing\n")
  condor_file.write("srun ./norm_weights.exe "+inputdir+" "+tag+" "+outdir+" " +prenormdir+"\n")
  condor_file.close()
  print("condor_submit submit_scripts_norm/"+year+"/condor_"+tag+"_norm.jds")
  #os.system("condor_submit condor_"+tag+"_norm.jds")
