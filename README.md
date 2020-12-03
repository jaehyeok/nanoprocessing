nanoprocessing
---------------
This is a package to process CMS nanoAOD to flat ntuples. It is assumed that you are running this package at KISTI. 

# Setup
You can set up environment, you can do the following on a KISTI machine. 
```
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/slc7_amd64_gcc530/cms/cmssw/CMSSW_8_0_0/src
cmsenv
cd -
```
Then, check out the package 
```
git clone https://github.com/jaehyeok/nanoprocessing
```

# Fastjet 
The package uses Fastjet [1] for fat-jet clustering (to calculate MJ). So, you need to install and set it up on your machine. Or, include `/cms/ldap_home/jaehyeok/tools/fastjet-install/bin` in your PATH.   

# compilation
The compliation can be done by running `./complie.ch`. It compiles two scripts (`process_nano.cpp` and `skim.cpp`) and copy them to your home directory so that they can be used by condor. 

# Run 
You can run the machinery with `./process_nano.exe`. It takes a 4 arguments. 
```
$ ./process_nano.exe
 Please provide proper arguments

   ./process_nano.exe [input dir] [output dir] [process] [list of processed files]
```
The "list of processed files" is generated by `condor/check_progress.py`.  
An example is 
```
./process_nana.exe /xrootd/store/data/Run2017D/SingleMuon/NANOAOD/Nano14Dec2018-v1 root://cms-xrdr.private.lo:2094//xrd/store/user/jaehyeok/2017v4/2019_10_23/ SingleMuonRun2017D flist_outputdir_xrd_store.txt
```

# Condor submission 
```
cd condor
./submit_all.py [year]
```
where year = 2016, 2017 or 2018. Under ~/flist diretory files that contain the nanoAOD files to be processed will be gemnerated. It list is based on `samples/samples[year].txt`. In addition, condor submit files will be generated in `submit_scripts` directory together with the actuall commands on the terminal. You can just copy the command and paste them in command line. 

# Tools
Check the status of the processing by by `./check_progress.py [year]`. It will prin out a table of status, and generate a list of process files in ~/flist directory. 

[1] http://fastjet.fr/
