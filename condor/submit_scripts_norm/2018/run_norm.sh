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
ls -rlt

echo ""

# run
echo "./norm_weights.exe $1 $2 $3 $4"
./norm_weights.exe  $1 $2 $3 $4

