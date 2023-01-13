#!/usr/bin/env bash

# # regulate_aa_data
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/HIHardProbes/HIRun2018A-PbPb18_MiniAODv1-v1/MINIAOD/HIHardProbes/20221205PbPbHP/221205_183819/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_data.conf \
# "regulate_aa_data" 30

# # regulate_aa_qcd_15 filtered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F15/221205_183553/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_15_F" 900

# regulate_aa_qcd_15 filtered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F15/230109_192554/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
"regulate_aa_qcd_15_F_R" 900

# # regulate_aa_qcd_30 filtered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F30/221205_183539/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_30_F" 900

# # regulate_aa_qcd_50 filtered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F50/221205_183510/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_50_F" 900

# regulate_aa_qcd_50 filtered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F50/230109_192611/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
"regulate_aa_qcd_50_F_R" 900

# # regulate_aa_qcd_80 filtered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F80/221205_183454/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_80_F" 900

# regulate_aa_qcd_80 filtered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F80/230109_192636/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
"regulate_aa_qcd_80_F_R" 900

# # regulate_aa_qcd_120 filtered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F120/221205_183425/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_120_F" 900

# # regulate_aa_qcd_170 filtered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMC30F170/221205_183441/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_170_F" 900

# # regulate_aa_qcd_15 unfiltered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMCUF15/221205_183318/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_15_UF" 900

# # regulate_aa_qcd_30 unfiltered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMCUF30/221205_183300/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_30_UF" 900

# # regulate_aa_qcd_50 unfiltered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMCUF50/221205_183220/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_50_UF" 900

# regulate_aa_qcd_50 unfiltered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCUF50/230109_192305/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
"regulate_aa_qcd_50_UF_R" 900

# # regulate_aa_qcd_80 unfiltered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMCUF80/221205_183020/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_80_UF" 900

# regulate_aa_qcd_80 unfiltered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCUF80/230109_191947/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
"regulate_aa_qcd_80_UF_R" 900

# # regulate_aa_qcd_120 unfiltered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMCUF120/221205_182533/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_120_UF" 900

# # regulate_aa_qcd_170 unfiltered
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221205PbPbMCUF170/221205_182507/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
# "regulate_aa_qcd_170_UF" 900

# regulate_aa_qcd_170 unfiltered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-mva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCUF170/230109_191857/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_prelim.conf \
"regulate_aa_qcd_170_UF_R" 900

# # regulate_aa_mb_hydjet
# ./SubmitJobs.sh \
# "/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/MinBias_Hydjet_Drum5F_2018_5p02TeV/HINPbPbSpring21MiniAOD-NoPUmva98_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM/MinBias_Hydjet_Drum5F_2018_5p02TeV/20221205PbPbMCMB/221205_183621/" \
# ../../configs/regulate/preapproval-parallel/regulate_aa_mb_hydjet.conf \
# "regulate_aa_mb_hydjet" 25

# regulate_aa_qcd_extra_15 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v1/AODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCExtra30F15/221209_192400/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_extra_prelim.conf \
"regulate_aa_qcd_extra_15_F" 9000

# regulate_aa_qcd_extra_30 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v1/AODSIM/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCExtra30F30/221209_192253/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_extra_prelim.conf \
"regulate_aa_qcd_extra_30_F" 9000

# regulate_aa_qcd_extra_50 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v2/AODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCExtra30F50/221209_192138/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_extra_prelim.conf \
"regulate_aa_qcd_extra_50_F" 9000

# regulate_aa_qcd_extra_80 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v2/AODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCExtra30F80/221209_192026/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_extra_prelim.conf \
"regulate_aa_qcd_extra_80_F" 9000

# regulate_aa_qcd_extra_120 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v2/AODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCExtra30F120/221209_191940/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_extra_prelim.conf \
"regulate_aa_qcd_extra_120_F" 9000

# regulate_aa_qcd_extra_120 filtered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v2/AODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCExtra30F120/230109_194605/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_extra_prelim.conf \
"regulate_aa_qcd_extra_120_F_R" 9000

# regulate_aa_qcd_extra_170 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-FixL1CaloGT_103X_upgrade2018_realistic_HI_v13_ext1-v2/AODSIM/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCExtra30F170/221209_191824/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_extra_prelim.conf \
"regulate_aa_qcd_extra_170_F" 9000

# regulate_aa_qcd_aod_15 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F15/221209_191019/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_15_F" 9000

# regulate_aa_qcd_aod_15 filtered remainder
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F15/230109_194214/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_15_F_R" 9000

# regulate_aa_qcd_aod_30 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F30/221209_190930/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_30_F" 9000

# regulate_aa_qcd_aod_50 filtered ***
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F50/230109_194133/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_50_F" 9000

# regulate_aa_qcd_aod_80 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F80/221209_190714/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_80_F" 9000

# regulate_aa_qcd_aod_120 filtered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMC30F120/221209_172335/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_F" 9000

# regulate_aa_qcd_aod_120 filtered ***
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F120/230109_194033/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_F" 9000

# regulate_aa_qcd_aod_170 filtered ***
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMC30F170/230109_194058/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_170_F" 9000

# regulate_aa_qcd_aod_15 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF15/221209_191723/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_15_UF" 9000

# regulate_aa_qcd_aod_30 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF30/221209_191628/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_30_UF" 9000 0

# regulate_aa_qcd_aod_30 unfiltered remainder ***
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCUF30/230109_194529/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_30_UF_R" 9000

# regulate_aa_qcd_aod_50 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF50/221209_191543/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_50_UF" 9000

# regulate_aa_qcd_aod_80 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/QCDPhoton_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF80/221209_191446/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_80_UF" 9000

# regulate_aa_qcd_aod_120 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF120/221209_191355/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_UF" 9000

# regulate_aa_qcd_aod_120 unfiltered remainder ***
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-120_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20230109PbPbMCUF120/230109_194502/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_120_UF_R" 9000

# regulate_aa_qcd_aod_170 unfiltered
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbAutumn18DR-mva98_103X_upgrade2018_realistic_HI_v11-v2/AODSIM/QCDPhoton_pThat-170_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/20221209PbPbMCUF170/221209_191252/" \
../../configs/regulate/preapproval-parallel/regulate_aa_qcd_aod_prelim.conf \
"regulate_aa_qcd_aod_170_UF" 9000

# regulate_aa_mb_hydjet_aod
./SubmitJobs.sh \
"/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/MinBias_Hydjet_Drum5F_2018_5p02TeV/HINPbPbAutumn18DR-NoPUmva98_103X_upgrade2018_realistic_HI_v11-v1/AODSIM/MinBias_Hydjet_Drum5F_2018_5p02TeV/20230109PbPbMCMB/230109_205550/" \
../../configs/regulate/preapproval-parallel/regulate_aa_mb_hydjet_aod.conf \
"regulate_aa_mb_hydjet_aod" 90