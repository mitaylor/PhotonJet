#!/usr/bin/env bash

# regulate_aa_data
# ./SubmitJobs_Split.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/HIHardProbes/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIHardProbes/20221205PbPbHP/221205_183819/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_data.conf \
# "regulate_aa_data" 30

# regulate_aa_qcd_15 filtered
./SubmitJobs_Split.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F15/221205_183553/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
"regulate_aa_qcd_15_F" 900

# # regulate_aa_qcd_30 filtered
# ./SubmitJobs_Split.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F30/221205_183539/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_30_F" 900

# # regulate_aa_qcd_50 filtered
# ./SubmitJobs_Split.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F50/221205_183510/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_50_F" 900

# # regulate_aa_qcd_80 filtered
# ./SubmitJobs_Split.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F80/221205_183454/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_80_F" 900

# # regulate_aa_qcd_120 filtered
# ./SubmitJobs_Split.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F120/221205_183425/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_120_F" 900

# # regulate_aa_qcd_170 filtered
# ./SubmitJobs_Split.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F170/221205_183441/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_170_F" 900