#!/usr/bin/env bash

# regulate_aa_data
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/HIHardProbes/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIHardProbes/20221205PbPbHP/221205_183819/" \
../configs/regulate/preapproval-parallel/regulate_aa_data.conf \
"regulate_aa_data" 30
