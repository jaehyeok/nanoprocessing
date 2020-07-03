#!/usr/bin/env python
import sys
import os
import os.path
import getpass

username = getpass.getuser()
year=sys.argv[1]

outputdir="root://cms-xrdr.private.lo:2094//xrd/store/user/"+username+"/nanoprocessing/2016/processed/"
#samplelist="samples/samples2016.txt"
#inputmctag="RunIISummer16NanoAODv4"
samplelist="samples/samples2016_v6.txt"
inputmctag="RunIISummer16NanoAODv6"

if year == "2017":
	outputdir="root://cms-xrdr.private.lo:2094//xrd/store/user/"+username+"/nanoprocessing/2017/processed/"
	#outputdir="/xrootd_user/"+username+"/xrootd/nanoprocessing/2017"
	samplelist="samples/samples2017_v6.txt"
	inputmctag="RunIIFall17NanoAODv6"
if year == "2018":
	outputdir="root://cms-xrdr.private.lo:2094//xrd/store/user/"+username+"/nanoprocessing/2018/processed/"
	#outputdir="/xrootd_user/"+username+"/xrootd/nanoprocessing/2018"
	samplelist="samples/samples2018_v6.txt"
	inputmctag="RunIIAutumn18NanoAODv6"

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

# make file list and condor submisstion script
lines = f.readlines()
for line in lines:
  if "#" in line:
    continue;
  splits = line.split("\t")
  # mc
  if "13TeV" in splits[0]:
    f_list = os.listdir("/xrootd/store/mc/"+inputmctag+"/"+splits[0]+"")
    process=line.split('_13TeV')[0]
    list_file = open("/cms/ldap_home/"+username+"/flist/"+year+"/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir("/xrootd/store/mc/"+inputmctag+"/"+splits[0]+"/"+subdir)
      for files in f_list_subdir:
        filename="/xrootd/store/mc/"+inputmctag+"/"+splits[0]+"/"+subdir+"/"+files+"\n"
        if "BB833C07-5ECA-5E43-8A99-38083AACE497" not in filename:
          list_file.write(filename)
    condor_file = open("submit_scripts/"+year+"/condor_"+process+".jds", "w")
    condor_file.write("executable              = run.sh\n");
    condor_file.write("arguments               = "+inputmctag+"/"+splits[0]+" "+outputdir+" "+process+" " +list_processed+"\n");
    condor_file.write("transfer_input_files    = process_nano.exe flist_"+process+".txt\n");
    condor_file.write("log                     = log/run_"+process+".$(Cluster).$(Process).log\n");
    condor_file.write("output                  = log/run_"+process+"_output.$(Cluster).$(Process).out\n");
    condor_file.write("error                   = log/run_"+process+"_errors.$(Cluster).$(Process).err\n");
    condor_file.write("#should_transfer_files   = Yes\n");
    condor_file.write("#when_to_transfer_output = ON_EXIT\n");
    condor_file.write("queue");
    condor_file.close()
    print("condor_submit submit_scripts/condor_"+process+".jds")
    #os.system("condor_submit condor_"+process+".jds")
  # data
  elif "Run201" in splits[0]:
    f_list = os.listdir(splits[0])
    process=line.split('/')[5]+line.split('/')[4]
    list_file = open("/cms/ldap_home/"+username+"/flist/"+year+"/flist_"+process+".txt", "w")
    for subdir in f_list:
      f_list_subdir = os.listdir(splits[0]+"/"+subdir)
      for files in f_list_subdir:
        filename=splits[0]+"/"+subdir+"/"+files+"\n"
        list_file.write(filename)
    condor_file = open("submit_scripts/"+year+"/condor_"+process+".jds", "w")
    condor_file.write("executable              = run.sh\n");
    condor_file.write("arguments               = "+splits[0]+" "+outputdir+" "+process+" "+list_processed+"\n");
    condor_file.write("transfer_input_files    = process_nano.exe flist_"+process+".txt\n");
    condor_file.write("log                     = log/run_"+process+".$(Cluster).$(Process).log\n");
    condor_file.write("output                  = log/run_"+process+"_output.$(Cluster).$(Process).out\n");
    condor_file.write("error                   = log/run_"+process+"_errors.$(Cluster).$(Process).err\n");
    condor_file.write("#should_transfer_files   = Yes\n");
    condor_file.write("#when_to_transfer_output = ON_EXIT\n");
    condor_file.write("queue");
    condor_file.close()
    print("condor_submit submit_scripts/condor_"+process+".jds")
    #os.system("condor_submit condor_"+process+".jds")
f.close()

