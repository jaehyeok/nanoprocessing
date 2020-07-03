#!/bin/sh

echo "This is where I am"
pwd
echo ""

#
export PATH=/bin:/usr/bin:/usr/local/bin:/cms/ldap_home/jaehyeok/tools/fastjet-install/bin:/usr/texbin/:/opt/pdflabs/pdftk/bin/$PATH

#
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/slc6_amd64_gcc490/cms/cmssw/CMSSW_7_1_0/src
cmsenv
cd -

#
echo "cp -r $HOME/processing/data ."
cp -r $HOME/processing/data .

echo ""

#
ls -rlt

echo ""

# run
echo "./process_nano.exe $1 $2 $3 $4"
./process_nano.exe  $1 $2 $3 $4

# copy
#xrdcp *_TTZToQQ.root root://cms-xrdr.private.lo:2094//xrd/store/user/jaehyeok/2017v4/2019_09_09
