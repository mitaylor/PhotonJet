#!/usr/bin/env bash

# AANominal: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal3.root

# AANominalMEBS: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_mebs.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominalMEBS0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_mebs.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominalMEBS1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_mebs.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominalMEBS2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_mebs.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominalMEBS3.root

# AAWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej3.root

# AATightPurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity3.root

# AALoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity3.root

# AAQCD: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD3.root

# AAQCDMEBS: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_mebs.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDMEBS0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_mebs.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDMEBS1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_mebs.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDMEBS2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_mebs.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDMEBS3.root

# AAQCDAll: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd_all.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDAll0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd_all.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDAll1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_all.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDAll2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_all.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDAll3.root

# AAQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso3.root

# AAJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp3.root

# AAJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown3.root

# AAJERUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp3.root

# # AAPriorUp: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_up.root \
#     --ResponseHistogram aa_c_0 \
#     --ResponseTruth aa_g_0 \
#     --ResponseReco aa_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorUp0.root
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_up.root \
#     --ResponseHistogram aa_c_1 \
#     --ResponseTruth aa_g_1 \
#     --ResponseReco aa_r_1 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorUp1.root
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_up.root \
#     --ResponseHistogram aa_c_2 \
#     --ResponseTruth aa_g_2 \
#     --ResponseReco aa_r_2 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorUp2.root
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_up.root \
#     --ResponseHistogram aa_c_3 \
#     --ResponseTruth aa_g_3 \
#     --ResponseReco aa_r_3 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorUp3.root

# # AAPriorDown: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_down.root \
#     --ResponseHistogram aa_c_0 \
#     --ResponseTruth aa_g_0 \
#     --ResponseReco aa_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorDown0.root
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_down.root \
#     --ResponseHistogram aa_c_1 \
#     --ResponseTruth aa_g_1 \
#     --ResponseReco aa_r_1 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorDown1.root
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_down.root \
#     --ResponseHistogram aa_c_2 \
#     --ResponseTruth aa_g_2 \
#     --ResponseReco aa_r_2 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorDown2.root
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
#     --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_aa_prior_down.root \
#     --ResponseHistogram aa_c_3 \
#     --ResponseTruth aa_g_3 \
#     --ResponseReco aa_r_3 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/AAPriorDown3.root

# AAVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_stat_response.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAVaryResponse0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_stat_response.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAVaryResponse1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_stat_response.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAVaryResponse2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_stat_response.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAVaryResponse3.root

# AACentUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp3.root

# AACentDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown0.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown1.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown2.root
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown3.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPNominal0.root

# PPWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_wo_ele_rej.root \
    --DataHistogram pp_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_wo_ele_rej.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPWithoutEleRej0.root

# PPTightPurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_tight_purity.root \
    --DataHistogram pp_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPTightPurity0.root

# PPLoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_loose_purity.root \
    --DataHistogram pp_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPLoosePurity0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_qcd.root \
    --DataHistogram pp_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_qcd.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPQCD0.root

# PPQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_qcd_gen_iso.root \
    --DataHistogram pp_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_qcd_gen_iso.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPQCDGenIso0.root

# PPJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_jeu_up.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPJEUUp0.root

# PPJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_jeu_down.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPJEUDown0.root

# PPJERUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_jer_up.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPJERUp0.root

# # PPPriorUp: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp.root \
#     --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_prior_up.root \
#     --ResponseHistogram pp_c_0 \
#     --ResponseTruth pp_g_0 \
#     --ResponseReco pp_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPPriorUp0.root

# # PPPriorDown: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp.root \
#     --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_prior_down.root \
#     --ResponseHistogram pp_c_0 \
#     --ResponseTruth pp_g_0 \
#     --ResponseReco pp_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPPriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_stat_response.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPVaryResponse0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90Nominal0.root

# PPWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90_wo_ele_rej.root \
    --DataHistogram pp_smear_50_90_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_wo_ele_rej.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90WithoutEleRej0.root

# PPTightPurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90_tight_purity.root \
    --DataHistogram pp_smear_50_90_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90TightPurity0.root

# PPLoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90_loose_purity.root \
    --DataHistogram pp_smear_50_90_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90LoosePurity0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90_qcd.root \
    --DataHistogram pp_smear_50_90_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_qcd.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90QCD0.root

# PPQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90_qcd_gen_iso.root \
    --DataHistogram pp_smear_50_90_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_qcd_gen_iso.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90QCDGenIso0.root

# PPJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_jeu_up.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90JEUUp0.root

# PPJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_jeu_down.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90JEUDown0.root

# PPJERUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_jer_up.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90JERUp0.root

# # PPPriorUp: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90.root \
#     --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_prior_up.root \
#     --ResponseHistogram pp_smear_50_90_c_0 \
#     --ResponseTruth pp_smear_50_90_g_0 \
#     --ResponseReco pp_smear_50_90_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear90PriorUp0.root

# # PPPriorDown: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90.root \
#     --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_prior_down.root \
#     --ResponseHistogram pp_smear_50_90_c_0 \
#     --ResponseTruth pp_smear_50_90_g_0 \
#     --ResponseReco pp_smear_50_90_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear90PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_50_90_stat_response.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear90VaryResponse0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50Nominal0.root

# PPWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50_wo_ele_rej.root \
    --DataHistogram pp_smear_30_50_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_wo_ele_rej.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50WithoutEleRej0.root

# PPTightPurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50_tight_purity.root \
    --DataHistogram pp_smear_30_50_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50TightPurity0.root

# PPLoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50_loose_purity.root \
    --DataHistogram pp_smear_30_50_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50LoosePurity0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50_qcd.root \
    --DataHistogram pp_smear_30_50_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_qcd.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50QCD0.root

# PPQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50_qcd_gen_iso.root \
    --DataHistogram pp_smear_30_50_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_qcd_gen_iso.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50QCDGenIso0.root

# PPJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_jeu_up.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50JEUUp0.root

# PPJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_jeu_down.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50JEUDown0.root

# PPJERUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_jer_up.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50JERUp0.root

# # PPPriorUp: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50.root \
#     --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_prior_up.root \
#     --ResponseHistogram pp_smear_30_50_c_0 \
#     --ResponseTruth pp_smear_30_50_g_0 \
#     --ResponseReco pp_smear_30_50_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear50PriorUp0.root

# # PPPriorDown: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50.root \
#     --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_prior_down.root \
#     --ResponseHistogram pp_smear_30_50_c_0 \
#     --ResponseTruth pp_smear_30_50_g_0 \
#     --ResponseReco pp_smear_30_50_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear50PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_30_50_stat_response.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear50VaryResponse0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30Nominal0.root

# PPWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30_wo_ele_rej.root \
    --DataHistogram pp_smear_10_30_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_wo_ele_rej.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30WithoutEleRej0.root

# PPTightPurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30_tight_purity.root \
    --DataHistogram pp_smear_10_30_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30TightPurity0.root

# PPLoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30_loose_purity.root \
    --DataHistogram pp_smear_10_30_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30LoosePurity0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30_qcd.root \
    --DataHistogram pp_smear_10_30_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_qcd.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30QCD0.root

# PPQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30_qcd_gen_iso.root \
    --DataHistogram pp_smear_10_30_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_qcd_gen_iso.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30QCDGenIso0.root

# PPJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_jeu_up.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30JEUUp0.root

# PPJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_jeu_down.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30JEUDown0.root

# PPJERUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_jer_up.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30JERUp0.root

# # PPPriorUp: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30.root \
#     --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_prior_up.root \
#     --ResponseHistogram pp_smear_10_30_c_0 \
#     --ResponseTruth pp_smear_10_30_g_0 \
#     --ResponseReco pp_smear_10_30_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear30PriorUp0.root

# # PPPriorDown: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30.root \
#     --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_prior_down.root \
#     --ResponseHistogram pp_smear_10_30_c_0 \
#     --ResponseTruth pp_smear_10_30_g_0 \
#     --ResponseReco pp_smear_10_30_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear30PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_10_30_stat_response.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear30VaryResponse0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10Nominal0.root

# PPWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10_wo_ele_rej.root \
    --DataHistogram pp_smear_0_10_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_wo_ele_rej.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10WithoutEleRej0.root

# PPTightPurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10_tight_purity.root \
    --DataHistogram pp_smear_0_10_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10TightPurity0.root

# PPLoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10_loose_purity.root \
    --DataHistogram pp_smear_0_10_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10LoosePurity0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10_qcd.root \
    --DataHistogram pp_smear_0_10_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_qcd.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10QCD0.root

# PPQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10_qcd_gen_iso.root \
    --DataHistogram pp_smear_0_10_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_qcd_gen_iso.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10QCDGenIso0.root

# PPJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_jeu_up.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10JEUUp0.root

# PPJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_jeu_down.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10JEUDown0.root

# PPJERUp: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_jer_up.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10JERUp0.root

# # PPPriorUp: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10.root \
#     --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_prior_up.root \
#     --ResponseHistogram pp_smear_0_10_c_0 \
#     --ResponseTruth pp_smear_0_10_g_0 \
#     --ResponseReco pp_smear_0_10_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear10PriorUp0.root

# # PPPriorDown: Execute
# ./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10.root \
#     --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
#     --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_prior_down.root \
#     --ResponseHistogram pp_smear_0_10_c_0 \
#     --ResponseTruth pp_smear_0_10_g_0 \
#     --ResponseReco pp_smear_0_10_r_0 \
#     --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
#     --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
#     --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
#     --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
#     --Output Output/PPSmear10PriorDown0.root

# PPVaryResponse: Execute
./Execute --Data ../../MainAnalysis/data/preapproval/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/preapproval/vacillate_pp_smear_0_10_stat_response.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPSmear10VaryResponse0.root