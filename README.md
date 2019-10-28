nanoprocessing
---------------
This is a package to process CMS nanoAOD to flat ntuples. It is assumed that you are running this package at KISTI. 

# Setup
You can set up environment, you can do the following on a KISTI machine. 
```
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/slc6_amd64_gcc490/cms/cmssw/CMSSW_7_1_0/src
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
An example is 
```
./process_nana.exe /xrootd/store/data/Run2017D/SingleMuon/NANOAOD/Nano14Dec2018-v1 root://cms-xrdr.private.lo:2094//xrd/store/user/jaehyeok/2017v4/   2019_10_23/ SingleMuonRun2017D flist_outputdir_xrd_store.txt
```
# Condor submission 


# Tools


[1] http://fastjet.fr/
