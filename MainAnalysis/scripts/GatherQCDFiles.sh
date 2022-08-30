#!/usr/bin/env bash

f_15="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v2/AODSIM/QCDPhoton_pThat-15_Filter30GeV_TuneCP5_5p02TeV_pythia8/20220825PPMC30F15/220826_051338/"
f_30="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v2/AODSIM/QCDPhoton_pThat-30_Filter30GeV_TuneCP5_5p02TeV_pythia8/20220825PPMC30F30/220826_051439/"
f_50="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-50_Filter30GeV_TuneCP5_5p02TeV_pythia8/20220825PPMC30F50/220826_051456/"
f_80="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-80_Filter30GeV_TuneCP5_5p02TeV_pythia8/20220825PPMC30F80/220826_051515/"
f_120="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-120_Filter30GeV_TuneCP5_5p02TeV_pythia8/20220825PPMC30F120/220826_051309/"
f_170="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-170_Filter30GeV_TuneCP5_5p02TeV_pythia8/20220825PPMC30F170/220826_051409/"

uf_15="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-15_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-15_TuneCP5_5p02TeV_pythia8/20220825PPMCUF15/220826_051550/"
uf_30="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-30_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-30_TuneCP5_5p02TeV_pythia8/20220825PPMCUF30/220826_051645/"
uf_50="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-50_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-50_TuneCP5_5p02TeV_pythia8/20220825PPMCUF50/220826_051705/"
uf_80="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-80_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-80_TuneCP5_5p02TeV_pythia8/20220825PPMCUF80/220826_051723/"
uf_120="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-120_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-120_TuneCP5_5p02TeV_pythia8/20220825PPMCUF120/220826_051532/"
uf_170="/mnt/T2_US_MIT/hadoop/cms/store/user/mitaylor/PhotonJet/QCDPhoton_pThat-170_TuneCP5_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV_v1-v1/AODSIM/QCDPhoton_pThat-170_TuneCP5_5p02TeV_pythia8/20220825PPMCUF170/220826_051608/"

./GatherFiles $f_15 f_15.txt
./GatherFiles $f_30 f_30.txt
./GatherFiles $f_50 f_50.txt
./GatherFiles $f_80 f_80.txt
./GatherFiles $f_120 f_120.txt
./GatherFiles $f_170 f_170.txt

./GatherFiles $uf_15 uf_15.txt
./GatherFiles $uf_30 uf_30.txt
./GatherFiles $uf_50 uf_50.txt
./GatherFiles $uf_80 uf_80.txt
./GatherFiles $uf_120 uf_120.txt
./GatherFiles $uf_170 uf_170.txt
