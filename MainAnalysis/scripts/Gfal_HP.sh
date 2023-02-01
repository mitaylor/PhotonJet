#!/usr/bin/env bash

for i in {1..1629}
do
    file="HiForestMiniAOD_${i}.root"

    if [ -f ${file} ]
    then
        echo "${file} already transferred"
    else
        gfal-copy gsiftp://gridftp-vanderbilt.sites.opensciencegrid.org/lio/lfs/cms/store/temp/user/mitaylor.9ba36553e001168e583a56880ccb32c070c92c8e/PhotonJet/HIHardProbes/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIHardProbes/20230130PbPbHP/230130_223456/0000/${file} .
    fi
done