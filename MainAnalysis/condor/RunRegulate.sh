#!/bin/sh

file=$1
output=$2

WorkDir=`pwd`

cd /afs/cern.ch/user/m/mitaylor/cmssw/CMSSW_10_3_3_patch1/src/
eval `scramv1 runtime -sh`
cd $WorkDir

regulate $file $output