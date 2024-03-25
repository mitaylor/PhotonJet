#!/usr/bin/env bash

set=$1
date=$2

mkdir -p /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/

if [[ $set == "pho_60" ]]; then
    primary_reco_bins="0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3"
    binning_reco_bins="20,30,35,40,45,50,55,60,70,80,90,100,120,150,200"
    primary_gen_bins="0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3"
    binning_gen_bins="20,30,40,50,60,70,80,100,120,200"
fi 

if [[ $set == "pho_60_rebin1" ]]; then
    primary_reco_bins="0,0.01,0.02,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2"
    binning_reco_bins="30,40,50,60,70,80,100,120,200"
    primary_gen_bins="0,0.01,0.02,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2"
    binning_gen_bins="30,40,50,60,70,80,100,120,200"
fi 

if [[ $set == "pho_60_rebin2" ]]; then
    primary_reco_bins="0,0.01,0.015,0.02,0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.1,0.12,0.15,0.2"
    binning_reco_bins="30,35,40,50,60,70,80,100,120,200"
    primary_gen_bins="0,0.01,0.016,0.024,0.032,0.04,0.05,0.06,0.08,0.1,0.12,0.15,0.2"
    binning_gen_bins="30,40,50,60,70,80,100,120,200"
fi 

if [[ $set == "pho_60_rebin3" ]]; then
    primary_reco_bins="0,0.01,0.015,0.02,0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.1,0.15,0.3"
    binning_reco_bins="30,35,40,45,50,60,70,80,100,200"
    primary_gen_bins="0,0.01,0.016,0.024,0.032,0.04,0.05,0.06,0.08,0.1,0.15,0.3"
    binning_gen_bins="30,40,50,60,70,80,100,200"
fi 

if [[ $set == "pho_60_rebin4" ]]; then
    primary_reco_bins="0,0.01,0.015,0.02,0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.1,0.15"
    binning_reco_bins="30,35,40,45,50,60,70,80,100,200"
    primary_gen_bins="0,0.01,0.016,0.024,0.032,0.04,0.05,0.06,0.08,0.1,0.15"
    binning_gen_bins="30,40,50,60,70,80,100,200"
fi 

# AADataPythia: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPythia0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_1 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPythia1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_2 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPythia2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_3 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPythia3.root

# AAMCPythia: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
    --ErrorHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AAMCPythia0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_1 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
    --ErrorHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AAMCPythia1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_2 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
    --ErrorHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AAMCPythia2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
    --DataHistogram aa_g_3 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_b.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
    --ErrorHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AAMCPythia3.root

# AADataJewelAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelAA0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelAA1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelAA2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelAA3.root

# AADataJewelNoRecoilAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPbNoRecoil_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelNoRecoilAA0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPbNoRecoil_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelNoRecoilAA1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPbNoRecoil_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelNoRecoilAA2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPbNoRecoil_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelNoRecoilAA3.root

# AADataJewelPP: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tJewelPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelPP0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tJewelPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelPP1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tJewelPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelPP2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tJewelPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataJewelPP3.root

# AADataPyquenAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenAA0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenAA1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenAA2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenAA3.root

# AADataPyquenNoWideAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenNoWidePbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenNoWideAA0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenNoWidePbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenNoWideAA1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenNoWidePbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenNoWideAA2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenNoWidePbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenNoWideAA3.root

# AADataPyquenPP: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tPyquenPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --ResponseTruthEfficiency aa_g_eff_0 \
    --ResponseRecoEfficiency aa_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenPP0.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tPyquenPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --ResponseTruthEfficiency aa_g_eff_1 \
    --ResponseRecoEfficiency aa_r_eff_1 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenPP1.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tPyquenPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --ResponseTruthEfficiency aa_g_eff_2 \
    --ResponseRecoEfficiency aa_r_eff_2 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenPP2.root
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tPyquenPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --ResponseTruthEfficiency aa_g_eff_3 \
    --ResponseRecoEfficiency aa_r_eff_3 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
    --ErrorHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/AADataPyquenPP3.root

# PPDataPythia: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_closure_a.root \
    --DataHistogram pp_g_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_closure_b.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
    --ErrorHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPDataPythia0.root

# PPMCPythia: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_closure_a.root \
    --DataHistogram pp_g_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_closure_b.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp_qcd.root \
    --ErrorHistogram pp_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPMCPythia0.root

# PPDataJewelPP: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tJewelPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
    --ErrorHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPDataJewelPP0.root

# PPDataJewelAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
    --ErrorHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPDataJewelAA0.root

# PPDataJewelNoRecoilAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPbNoRecoil_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
    --ErrorHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPDataJewelNoRecoilAA0.root

# PPDataPyquenPP: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tPyquenPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
    --ErrorHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPDataPyquenPP0.root

# PPDataPyquenAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
    --ErrorHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPDataPyquenAA0.root

# PPDataPyquenNoWideAA: Execute
./ExecuteTheory --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenNoWidePbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --ResponseTruthEfficiency pp_g_eff_0 \
    --ResponseRecoEfficiency pp_r_eff_0 \
    --Error ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
    --ErrorHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output /data/submit/mitay/unfolding/${date}/Input/Theory/${set}/PPDataPyquenNoWideAA0.root