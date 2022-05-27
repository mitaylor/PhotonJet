#!/usr/bin/env bash

# AANominal: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AANominal3.root

# AAWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_wo_ele_rej.root \
    --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_wo_ele_rej.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAWithoutEleRej3.root

# AATightPurity: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_tight_purity.root \
    --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AATightPurity3.root

# AALoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_loose_purity.root \
    --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AALoosePurity3.root

# AAQCD: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCD3.root

# AAQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_qcd_gen_iso.root \
    --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_qcd_gen_iso.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAQCDGenIso3.root

# AAJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_up.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUUp3.root

# AAJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jeu_down.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJEUDown3.root

# AAJERUp: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_jer_up.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AAJERUp3.root

# AACentUp: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_up.root \
    --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_up.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentUp3.root

# AACentDown: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown0.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown1.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown2.root
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_aa_cent_down.root \
    --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_aa_cent_down.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/AACentDown3.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPNominal0.root

# PPWithoutEleRej: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp_wo_ele_rej.root \
    --DataHistogram pp_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp_wo_ele_rej.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPWithoutEleRej0.root

# PPTightPurity: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp_tight_purity.root \
    --DataHistogram pp_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPTightPurity0.root

# PPLoosePurity: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp_loose_purity.root \
    --DataHistogram pp_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPLoosePurity0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp_qcd.root \
    --DataHistogram pp_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp_qcd.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPQCD0.root

# PPQCDGenIso: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp_qcd_gen_iso.root \
    --DataHistogram pp_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp_qcd_gen_iso.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPQCDGenIso0.root

# PPJEUUp: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp_jeu_up.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPJEUUp0.root

# PPJEUDown: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp_jeu_down.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPJEUDown0.root

# PPJERUp: Execute
./Execute --Data ../../MainAnalysis/data/pho_60/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/pho_60/vacillate_pp_jer_up.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3 \
    --BinningRecoBins 20,30,35,40,45,50,55,60,70,80,90,100,120,150,200,250 \
    --PrimaryGenBins 0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3 \
    --BinningGenBins 20,30,40,50,60,70,80,100,120,200 \
    --Output Output/PPJERUp0.root