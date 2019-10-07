#!/usr/bin/env python
import sys
import os
import os.path

#outputdir="/xrootd_user/jaehyeok/xrootd/2017v4/2019_09_09"
outputdir="root://cms-xrdr.private.lo:2094//xrd/store/user/jaehyeok/2017v4/2019_09_11/"

f = open("samples/samples2017.txt", 'r')
lines = f.readlines()
for line in lines:
  if "#" in line:
    continue;
  splits = line.split("\t")
  # mc
  if "13TeV" in splits[0]:
    f_list = os.listdir("/xrootd/store/mc/RunIIFall17NanoAODv4/"+splits[0]+"")
    process=line.split('_13TeV')[0]
    list_file = open("/cms/ldap_home/jaehyeok/flist/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir("/xrootd/store/mc/RunIIFall17NanoAODv4/"+splits[0]+"/"+subdir)
      for files in f_list_subdir:
        filename="/xrootd/store/mc/RunIIFall17NanoAODv4/"+splits[0]+"/"+subdir+"/"+files+"\n"
        if "BB833C07-5ECA-5E43-8A99-38083AACE497" not in filename:
          list_file.write(filename)
    condor_file = open("condor_"+process+".jds", "w")
    condor_file.write("executable              = run.sh\n");
    condor_file.write("arguments               = "+splits[0]+" "+outputdir+" "+process+"\n");
    condor_file.write("transfer_input_files    = process_nano.exe flist_"+process+".txt\n");
    condor_file.write("log                     = log/run_"+process+".$(Cluster).$(Process).log\n");
    condor_file.write("output                  = log/run_"+process+"_output.$(Cluster).$(Process).out\n");
    condor_file.write("error                   = log/run_"+process+"_errors.$(Cluster).$(Process).err\n");
    condor_file.write("#should_transfer_files   = Yes\n");
    condor_file.write("#when_to_transfer_output = ON_EXIT\n");
    condor_file.write("queue");
    condor_file.close()
    print("condor_submit condor_"+process+".jds")
    #os.system("condor_submit condor_"+process+".jds")
  # data
  elif "Run201" in splits[0]:
    f_list = os.listdir(splits[0])
    process=line.split('/')[5]+line.split('/')[4]
    list_file = open("/cms/ldap_home/jaehyeok/flist/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir(splits[0]+"/"+subdir)
      for files in f_list_subdir:
        filename=splits[0]+"/"+subdir+"/"+files+"\n"
        list_file.write(filename)
    condor_file = open("condor_"+process+".jds", "w")
    condor_file.write("executable              = run.sh\n");
    condor_file.write("arguments               = "+splits[0]+" "+outputdir+" "+process+"\n");
    condor_file.write("transfer_input_files    = process_nano.exe flist_"+process+".txt\n");
    condor_file.write("log                     = log/run_"+process+".$(Cluster).$(Process).log\n");
    condor_file.write("output                  = log/run_"+process+"_output.$(Cluster).$(Process).out\n");
    condor_file.write("error                   = log/run_"+process+"_errors.$(Cluster).$(Process).err\n");
    condor_file.write("#should_transfer_files   = Yes\n");
    condor_file.write("#when_to_transfer_output = ON_EXIT\n");
    condor_file.write("queue");
    condor_file.close()
    print("condor_submit condor_"+process+".jds")
    #os.system("condor_submit condor_"+process+".jds")
f.close()

