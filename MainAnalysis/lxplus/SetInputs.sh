#!/usr/bin/env bash

# regulate_aa_data
./SubmitJobs.sh \
"/eos/cms/store/group/phys_heavyions/mitaylor/PhotonJet/HIHardProbes/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIHardProbes/20230203PbPbHP/230203_165035/" \
../../configs/regulate/preapproval-parallel/regulate_aa_data.conf \
"regulate_aa_data" 10

# regulate_aa_mb_data
./SubmitJobs.sh \
"/eos/cms/store/group/phys_heavyions/mitaylor/PhotonJet/HIMinimumBias4/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIMinimumBias4/20230203PbPbMB/230203_165233/" \
../../configs/regulate/preapproval-parallel/regulate_aa_mb_data.conf \
"regulate_aa_mb_data" 10