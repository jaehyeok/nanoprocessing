#!/usr/bin/env python
import sys
import os
import os.path
import getpass
import re
import glob

username = getpass.getuser()
year=sys.argv[1]

outputdir="root://cms-xrdr.private.lo:2094//xrd/store/user/"+username+"/nanoprocessing/"+year+"/processed_deepTvsQCD/"

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
#list_processed = "flist_"+splits[4]+"_"+splits[5]+".txt"
list_processed = "flist_outputdir_"+splits[8]+"_"+splits[9]+".txt"
#list_processed = "flist_outputdir_"+splits[9]+"_"+splits[10]+".txt"#FIXME

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
    f_list = glob.glob("/xrootd/store/mc/"+inputmctag+"/"+splits[0].rstrip()+"*/*")
    #f_list = glob.glob("/xrootd/store/user/"+username+"/"+inputmctag+"/"+splits[0].rstrip()+"*/*")#FIXME
    process=line.split('_13TeV')[0]
    list_file = open("/cms/ldap_home/"+username+"/flist/"+year+"/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir(subdir)
      for files in f_list_subdir:
        filename=subdir+"/"+files+"\n"
        #print(filename)
        if "BB833C07-5ECA-5E43-8A99-38083AACE497" not in filename:
          list_file.write(filename)
    condor_file = open("submit_scripts/"+year+"/condor_"+process+".jds", "w")
    condor_file.write("executable              = run.sh\n");
    condor_file.write("arguments               = "+inputmctag+"/"+splits[0].rstrip()+" "+outputdir+" "+process+" " +list_processed+"\n");
    condor_file.write("transfer_input_files    = process_nano.exe flist_"+process+".txt\n");
    condor_file.write("log                     = log/run_"+process+".$(Cluster).$(Process).log\n");
    condor_file.write("output                  = log/run_"+process+"_output.$(Cluster).$(Process).out\n");
    condor_file.write("error                   = log/run_"+process+"_errors.$(Cluster).$(Process).err\n");
    condor_file.write("#should_transfer_files   = Yes\n");
    condor_file.write("#when_to_transfer_output = ON_EXIT\n");
    condor_file.write("queue");
    condor_file.close()
    print("condor_submit submit_scripts/"+year+"/condor_"+process+".jds")
    #os.system("condor_submit condor_"+process+".jds")
  # data
  elif "Run201" in line:
    f_list = os.listdir(line.rstrip())
    process=line.split('/')[5]+line.split('/')[4]
    list_file = open("/cms/ldap_home/"+username+"/flist/"+year+"/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir(splits[0].rstrip()+"/"+subdir)
      for files in f_list_subdir:
        filename=splits[0].rstrip()+"/"+subdir+"/"+files+"\n"
        list_file.write(filename)
    condor_file = open("submit_scripts/"+year+"/condor_"+process+".jds", "w")
    condor_file.write("executable              = run.sh\n");
    condor_file.write("arguments               = "+splits[0].rstrip()+" "+outputdir+" "+process+" "+list_processed+"\n");
    condor_file.write("transfer_input_files    = process_nano.exe flist_"+process+".txt\n");
    condor_file.write("log                     = log/run_"+process+".$(Cluster).$(Process).log\n");
    condor_file.write("output                  = log/run_"+process+"_output.$(Cluster).$(Process).out\n");
    condor_file.write("error                   = log/run_"+process+"_errors.$(Cluster).$(Process).err\n");
    condor_file.write("#should_transfer_files   = Yes\n");
    condor_file.write("#when_to_transfer_output = ON_EXIT\n");
    condor_file.write("queue");
    condor_file.close()
    print("condor_submit submit_scripts/"+year+"/condor_"+process+".jds")
    #os.system("condor_submit condor_"+process+".jds")
f.close()

