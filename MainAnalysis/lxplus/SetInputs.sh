#!/usr/bin/env bash

# # regulate_aa_data
# ./SubmitJobs.sh \
# "/eos/cms/store/group/phys_heavyions/mitaylor/PhotonJet/HIHardProbes/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIHardProbes/20230203PbPbHP/230203_165035/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_data.conf \
# "regulate_aa_data" 10

# # regulate_aa_mb_data
# ./SubmitJobs.sh \
# "/eos/cms/store/group/phys_heavyions/mitaylor/PhotonJet/HIMinimumBias4/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIMinimumBias4/20230203PbPbMB/230203_165233/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_mb_data.conf \
# "regulate_aa_mb_data" 10

#####################################################################################################################################################################

# # regulate_aa_qcd_15 filtered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMC30F15/230127_215615/" \
# "..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
# "regulate_aa_qcd_15_F"

# regulate_aa_qcd_30 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMC30F30/230127_215641/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_30_F"

# regulate_aa_qcd_50 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMC30F50/230127_215653/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_50_F"

# regulate_aa_qcd_80 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMC30F80/230127_215705/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_80_F"

# regulate_aa_qcd_120 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMC30F120/230127_215719/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_120_F"

# regulate_aa_qcd_170 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMC30F170/230127_215730/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_170_F"

# regulate_aa_qcd_15 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMCUF15/230127_215848/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_15_UF"

# regulate_aa_qcd_30 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMCUF30/230127_215836/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_30_UF"

# regulate_aa_qcd_50 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMCUF50/230127_215825/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_50_UF"

# regulate_aa_qcd_80 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMCUF80/230127_215812/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_80_UF"

# regulate_aa_qcd_120 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMCUF120/230130_162634/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_120_UF"

# regulate_aa_qcd_170 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230127PbPbMCUF170/230127_215743/" \
"..\/..\/configs\/regulate\/preapproval-parallel\/regulate_aa_qcd_prelim.conf" \
"regulate_aa_qcd_170_UF"