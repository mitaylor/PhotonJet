#!/bin/sh

input=$1

WorkDir=`pwd`

cd /home/submit/mitay/cmssw/CMSSW_10_3_3_patch1/src/
eval `scramv1 runtime -sh`
cd $WorkDir

../bin/regulate ../configs/regulate/ran/${type}.conf /mnt/submit-hi1/d00/higroup/mitay/condor/${type}.root