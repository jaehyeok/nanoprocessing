#!/usr/bin/env python
import sys
import os
import os.path
import getpass
import re
import glob

username = getpass.getuser()
year=sys.argv[1]

#outputdir="root://cms-xrdr.private.lo:2094//xrd/store/user/"+username+"/nanoprocessing/"+year+"/processed_JER_0903/"
outputdir="/data2/nanoprocessing/"+year+"/processed_test/"

#samplelist="samples/samples"+year+"UL_v2.txt"
samplelist="samples/samples"+year+"_v7.txt"
inputmctag="RunIISummer16NanoAODv7"

if year == "2017":
	#inputmctag="UL2017_MiniAODv1_NanoAODv2"
	inputmctag="RunIIFall17NanoAODv7"

if year == "2018":
	#inputmctag="UL2018_MiniAODv1_NanoAODv2"
	inputmctag="RunIIAutumn18NanoAODv7"

# print out inputs
print "------------------------------------------------------------------------------------------------------------"
print " year: "+year
print " output dir: "+outputdir
print " sample list: "+samplelist
print "------------------------------------------------------------------------------------------------------------"
print ""

# open sample list file to write
f = open(samplelist, 'r')

splits = outputdir.split("/")
list_processed = "flist_nanoprocessing_"+splits[2]+"_"+splits[3]+".txt"
#list_processed = "flist_outputdir_"+splits[8]+"_"+splits[9]+".txt"

# list of samples from sample list
lines_with_ext = f.readlines()
# remove everything after "asymptotic_v7" to include both ext and non-ext samples
lines_duplicates = [sample_with_ext.split("asymptotic_v8")[0] for sample_with_ext in lines_with_ext]
# remove duplicates
lines = list(dict.fromkeys(lines_duplicates))

for line in lines:
  if "#" in line:
    continue;
  #print(line) #FIXME
  splits = line.split("\t")
  # mc
  if "13TeV" in line:
    #f_list = glob.glob("/xrootd/store/mc/"+inputmctag+"/"+splits[0].rstrip()+"*/*")
    f_list = glob.glob("/data/mc/NanoAODv7/2016/"+inputmctag+"/"+splits[0].rstrip()+"*/*")
    #f_list = glob.glob("/xrootd/store/user/"+username+"/"+inputmctag+"/"+splits[0].rstrip()+"*/*")#FIXME
    process=line.split('_13TeV')[0]
    list_file = open("/home/yjeong/flist/"+year+"/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir(subdir)
      for files in f_list_subdir:
        filename=subdir+"/"+files+"\n"
        #print(filename)
        if "BB833C07-5ECA-5E43-8A99-38083AACE497" not in filename:
          list_file.write(filename)
    slurm_file = open("submit_scripts/"+year+"/slurm_"+process+".sh", "w")
    #slurm_file.write("executable              = run.sh\n");
    slurm_file.write("#!/bin/sh \n");
    slurm_file.write("#SBATCH -J " + process+"\n");
    slurm_file.write("#SBATCH -N 1\n");
    slurm_file.write("#SBATCH -n 1\n");
    slurm_file.write("#SBATCH -o log/run_%x.o%j.txt\n");
    slurm_file.write("#SBATCH -e log/err_%x.e%j.txt\n");
    slurm_file.write("cd ~/nanoprocessing\n");
    slurm_file.write("srun ./process_nano.exe "+inputmctag+"/"+splits[0].rstrip()+" "+outputdir+" "+process+" " +list_processed+"\n");
    slurm_file.close()
    print("sbatch submit_scripts/"+year+"/slurm_"+process+".sh")
    #os.system("condor_submit condor_"+process+".jds")
  # data
  elif "Run201" in line:
    f_list = os.listdir(line.rstrip())
    process=line.split('/')[5]+line.split('/')[4]
    list_file = open("/home/yjeong/flist/"+year+"/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir(splits[0].rstrip()+"/"+subdir)
      for files in f_list_subdir:
        filename=splits[0].rstrip()+"/"+subdir+"/"+files+"\n"
        list_file.write(filename)
    slurm_file = open("submit_scripts/"+year+"/slurm_"+process+".sh", "w")
    #slurm_file.write("executable              = run.sh\n");
    slurm_file.write("#!/bin/sh \n");
    slurm_file.write("#SBATCH -J " + process+"\n");
    slurm_file.write("#SBATCH -N 1\n");
    slurm_file.write("#SBATCH -n 1\n");
    slurm_file.write("#SBATCH -o log/run_%x.o%j.txt\n");
    slurm_file.write("#SBATCH -e log/err_%x.e%j.txt\n");
    slurm_file.write("cd ~/nanoprocessing\n");
    slurm_file.write("srun ./process_nano.exe "+inputmctag+"/"+splits[0].rstrip()+" "+outputdir+" "+process+" " +list_processed+"\n");
    slurm_file.close()
    print("sbatch submit_scripts/"+year+"/slurm_"+process+".sh")
    #os.system("condor_submit condor_"+process+".jds")
f.close()

