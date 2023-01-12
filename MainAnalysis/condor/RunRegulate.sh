#!/bin/sh

file=$1
output=$2

WorkDir=`pwd`

# cd /afs/cern.ch/user/m/mitaylor/cmssw/CMSSW_10_3_3_patch1/src/
# eval `scramv1 runtime -sh`
# cd $WorkDir

source /cvmfs/cms.cern.ch/cmsset_default.sh
# export SCRAM_ARCH=slc6_amd64_gcc700

if [ -r CMSSW_10_3_3_patch1/src ] ; then
 echo release CMSSW_10_3_3_patch1 already exists
else
scram p CMSSW CMSSW_10_3_3_patch1
fi
cd CMSSW_10_3_3_patch1/src
eval `scramv1 runtime -sh`

../regulate $file $output