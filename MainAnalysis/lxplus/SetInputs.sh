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

# regulate_aa_qcd_aod_15 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F15/221209_191019/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_15_F" 50

# regulate_aa_qcd_aod_15 filtered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F15/230109_194214/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_15_F_R" 50

# regulate_aa_qcd_aod_30 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F30/221209_190930/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_30_F" 50

# regulate_aa_qcd_aod_50 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F50/230109_194133/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_50_F" 50

# regulate_aa_qcd_aod_80 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F80/221209_190714/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_80_F" 50

# regulate_aa_qcd_aod_120 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F120/221209_172335/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_F" 50

# regulate_aa_qcd_aod_120 filtered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F120/230109_194033/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_F_R" 50

# regulate_aa_qcd_aod_170 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F170/230109_194058/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_170_F" 50

# regulate_aa_qcd_aod_15 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF15/221209_191723/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_15_UF" 50

# regulate_aa_qcd_aod_30 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF30/221209_191628/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_30_UF" 50

# regulate_aa_qcd_aod_30 unfiltered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCUF30/230109_194529/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_30_UF_R" 50

# regulate_aa_qcd_aod_50 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF50/221209_191543/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_50_UF" 50

# regulate_aa_qcd_aod_80 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF80/221209_191446/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_80_UF" 50

# regulate_aa_qcd_aod_120 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF120/221209_191355/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_UF" 50

# regulate_aa_qcd_aod_120 unfiltered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCUF120/230109_194502/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_UF_R" 50

# regulate_aa_qcd_aod_170 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF170/221209_191252/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_170_UF" 50