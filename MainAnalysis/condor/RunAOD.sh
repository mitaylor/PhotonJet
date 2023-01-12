#!/bin/sh

Input=$1
Number=$2

WorkDir=`pwd`

cd /afs/cern.ch/user/m/mitaylor/cmssw/CMSSW_10_3_3_patch1/src/
eval `scramv1 runtime -sh`
cd /afs/cern.ch/user/m/mitaylor/condor/210928_HIHardProbesForesting/

export X509_USER_PROXY=/afs/cern.ch/user/m/mitaylor/proxy/x509up_u117190

cat forest_AOD_103X_DATA_template.py | sed "s#__INPUT__#$Input#; s#__OUTPUT__#HiForestAOD_$Number.root#" > $WorkDir/run$Number.py
cmsRun $WorkDir/run$Number.py

rm $WorkDir/run$Number.py