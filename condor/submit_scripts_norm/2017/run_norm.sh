#!/bin/sh

echo "This is where I am"
pwd
echo ""

#
export PATH=/bin:/usr/bin:/usr/local/bin:/cms/ldap_home/jaehyeok/tools/fastjet-install/bin:/usr/texbin/:/opt/pdflabs/pdftk/bin/$PATH

#
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/slc7_amd64_gcc700/cms/cmssw/CMSSW_10_5_0/src
cmsenv
cd -

#
ls -rlt

echo ""

# run
cd ~
echo "./norm_weights.exe $1 $2 $3 $4"
./norm_weights.exe  $1 $2 $3 $4

