#!/bin/sh

config=$1
analysis=$2
output=$3
input=$4
mb=$5

WorkDir=`pwd`

source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc7_amd64_gcc700

if [ -r CMSSW_10_3_3_patch1/src ] ; then
    echo release CMSSW_10_3_3_patch1 already exists
else
    scram p CMSSW CMSSW_10_3_3_patch1
fi

cd CMSSW_10_3_3_patch1/src
eval `scramv1 runtime -sh`

cd $WorkDir

export X509_USER_PROXY=${WorkDir}/x509up_u168456

xrdcp ${input} input.root
xrdcp ${mb} mb.root

./binning $config $analysis $output

rm input.root
rm mb.root