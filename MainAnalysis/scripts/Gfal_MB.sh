#!/usr/bin/env bash

for i in {1..7870}
do
    file="HiForestMiniAOD_${i}.root"

    if [ -f ${file} ]
    then
        echo "${file} already transferred"
    else
        gfal-copy gsiftp://gridftp-vanderbilt.sites.opensciencegrid.org/lio/lfs/cms/store/temp/user/mitaylor.9ba36553e001168e583a56880ccb32c070c92c8e/PhotonJet/HIMinimumBias4/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIMinimumBias4/20230130PbPbMB/230130_223511/0000/${file} .
    fi
done