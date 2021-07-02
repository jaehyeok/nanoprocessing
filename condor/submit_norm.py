#!/usr/bin/env python
import sys
import os
import os.path
import getpass

username = getpass.getuser()

#  ./norm_weights.exe /xrootd_user/"+username+"/xrootd/nanoprocessing/2016/merged_rpvfitnbge0/ SMS-T1tbs_RPV_mGluino1600 /xrootd_user/jaehyeok/babies/2016v6/norm_weights /xrootd_user/"+username+"/xrootd/nanoprocessing/2016/processed

year=sys.argv[1] ##2016, 2017, 2018

inputdir     = "/xrootd_user/"+username+"/xrootd/nanoprocessing/"+year+"/merged_rpvfitnbge0_bkgJer/"
outdir       = "/xrootd_user/"+username+"/xrootd/nanoprocessing/"+year+"/merged_norm_bkgJer/"
prenormdir   = "/xrootd_user/"+username+"/xrootd/nanoprocessing/"+year+"/processed_bkgJer/"
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
  flist_prenorm = open("/cms/ldap_home/"+username+"/flist/norm/"+year+"/flist_prenorm_"+tag+".txt", "w")
  for files in os.listdir(prenormdir):
    #if(tag+"_" in files):
    if(tag in files):
      flist_prenorm.write(prenormdir+"/"+files+"\n")
  flist_tonorm  = open("/cms/ldap_home/"+username+"/flist/norm/"+year+"/flist_tonorm_"+tag+".txt", "w")
  for files in os.listdir(inputdir):
    if(tag in files):
      flist_tonorm.write(inputdir+"/"+files+"\n")

  condor_file = open("submit_scripts_norm/"+year+"/condor_"+tag+"_norm.jds", "w")
  condor_file.write("executable              = run_norm.sh\n")
  condor_file.write("arguments               =  "+inputdir+" "+tag+" "+outdir+" "+prenormdir+"\n")
  condor_file.write("transfer_input_files    = norm_weights.exe normlist_"+tag+"_"+year+".txt\n")
  condor_file.write("log                     = log/norm_"+tag+".$(Cluster).$(Process).log\n")
  condor_file.write("output                  = log/norm_"+tag+"_output.$(Cluster).$(Process).out\n")
  condor_file.write("error                   = log/norm_"+tag+"_errors.$(Cluster).$(Process).err\n")
  condor_file.write("#should_transfer_files   = Yes\n")
  condor_file.write("#when_to_transfer_output = ON_EXIT\n")
  condor_file.write("queue")
  condor_file.close()
  print("condor_submit submit_scripts_norm/"+year+"/condor_"+tag+"_norm.jds")
  #os.system("condor_submit condor_"+tag+"_norm.jds")
