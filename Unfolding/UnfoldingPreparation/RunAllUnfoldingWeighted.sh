#!/usr/bin/env bash

set=$1

# AAPriorUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_up.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorUp0.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_up.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorUp1.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_up.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorUp2.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_up.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorUp3.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_merge_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_up.root \
    --ResponseHistogram aa_c_merge_0 \
    --ResponseTruth aa_g_merge_0 \
    --ResponseReco aa_r_merge_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorUpMerge.root

# AAPriorDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_down.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorDown0.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_down.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorDown1.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_down.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorDown2.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_down.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorDown3.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_merge_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_prior_down.root \
    --ResponseHistogram aa_c_merge_0 \
    --ResponseTruth aa_g_merge_0 \
    --ResponseReco aa_r_merge_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAPriorDownMerge.root

# AAVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_stat_response.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAVaryResponse0.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_stat_response.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAVaryResponse1.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_stat_response.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAVaryResponse2.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_stat_response.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAVaryResponse3.root
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_merge_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_aa_stat_response.root \
    --ResponseHistogram aa_c_merge_0 \
    --ResponseTruth aa_g_merge_0 \
    --ResponseReco aa_r_merge_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/AAVaryResponseMerge.root

# PPPriorUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_prior_up.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPPriorUp0.root

# PPPriorDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_prior_down.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPPriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_stat_response.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPVaryResponse0.root

# PPPriorUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_50_90_prior_up.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear90PriorUp0.root

# PPPriorDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_50_90_prior_down.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear90PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_50_90_stat_response.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear90VaryResponse0.root

# PPPriorUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_30_50_prior_up.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear50PriorUp0.root

# PPPriorDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_30_50_prior_down.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear50PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_30_50_stat_response.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear50VaryResponse0.root

# PPPriorUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_10_30_prior_up.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear30PriorUp0.root

# PPPriorDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_10_30_prior_down.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear30PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_10_30_stat_response.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear30VaryResponse0.root

# PPPriorUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_0_10_prior_up.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear10PriorUp0.root

# PPPriorDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_0_10_prior_down.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear10PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/${set}/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/${set}/vacillate_weight_pp_smear_0_10_stat_response.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output OutputWeighted/${set}/PPSmear10VaryResponse0.root